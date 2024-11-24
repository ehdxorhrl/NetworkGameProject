#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "CGameloop.h"
#include "LoadingScene.h"
#pragma comment(lib, "ws2_32.lib")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Program gimal project";
CGameloop loop;
SOCKET g_serverSocket = INVALID_SOCKET;

HANDLE gGameStartEvent = NULL;
HANDLE hCommThread = NULL;  // Communication ������ �ڵ�
HANDLE hServerSignalThread = NULL;  // ���� ��ȣ ��� ������ �ڵ�

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// Communication ������ �Լ� - ���� �����͸� �ٷ� ó��
DWORD WINAPI CommunicationThreadFunc(LPVOID lpParam) {
    char buffer[1024];

    // ���� Ÿ�Ӿƿ� ����
    int timeoutInMillis = 100; // 100ms Ÿ�Ӿƿ�
    setsockopt(g_serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis));
    setsockopt(g_serverSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis));

    while (true) {
        int bytesReceived = recv(g_serverSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            // ���� ������ ó��
            OutputDebugStringA(buffer);
            OutputDebugStringA("\n");
        }
        else if (bytesReceived == 0) {
            // ������ ������ ����
            OutputDebugString(L"Server closed connection\n");
            break;
        }
        else if (bytesReceived == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT) {
                // Ÿ�Ӿƿ� �߻�, ��� ���� ����
                OutputDebugString(L"Socket recv timeout\n");
            }
            else {
                // ġ������ ���� ���� �߻�
                std::wstring errorMsg = L"Socket error: " + std::to_wstring(error);
                OutputDebugString(errorMsg.c_str());
                break;
            }
        }

        // �ʿ��� ��� Sleep���� CPU ��뷮 ����
        Sleep(1);
    }

    return 0;
}

// ���� ���� �Լ�
bool ConnectToServer(const char* serverIP, int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        MessageBox(NULL, L"WSAStartup ����", L"Error", MB_OK);
        return false;
    }

    g_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_serverSocket == INVALID_SOCKET) {
        MessageBox(NULL, L"���� ���� ����", L"Error", MB_OK);
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    if (connect(g_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        std::string errorMsg = "���� ���� ����: " + std::to_string(errorCode);
        MessageBoxA(NULL, errorMsg.c_str(), "Error", MB_OK);
        closesocket(g_serverSocket);
        WSACleanup();
        return false;
    }

    return true;
}

// WinMain �Լ�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszCmdParam, int nCmdShow) {
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    gGameStartEvent = CreateEvent(
        NULL,   // �⺻ ���� �Ӽ�
        TRUE,   // ���� �缳�� (TRUE: ����, FALSE: �ڵ�)
        FALSE,  // �ʱ� ���´� ���ȣ ����
        NULL    // �̺�Ʈ �̸� (�͸�)
    );

    if (!gGameStartEvent) {
        MessageBox(NULL, L"�̺�Ʈ ���� ����", L"Error", MB_OK);
        return -1;
    }

    // �⺻ IP �� ��Ʈ ����
    std::string ip = "127.0.0.1";
    int port = 8080;

    // ��ɾ� ���� �Ľ�
    if (lpszCmdParam && strlen(lpszCmdParam) > 0) {
        std::istringstream iss(lpszCmdParam);
        if (!(iss >> ip >> port)) {
            MessageBox(NULL, L"IP �ּҿ� ��Ʈ ��ȣ�� �ùٸ��� �Է��ϼ���", L"Error", MB_OK);
            return -1;
        }
    }

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_HAND);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_QUESTION);
    RegisterClassEx(&WndClass);

    RECT rect = { 0, 0, 800, 800 }; // ���ϴ� Ŭ���̾�Ʈ ���� ũ��
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE); // ��ü â ũ�� ����

    hWnd = CreateWindow(
        lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
        rect.bottom - rect.top, NULL, NULL, hInstance, NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // ���� ����
    if (!ConnectToServer(ip.c_str(), port)) {
        return -1;
    }

    // Communication ������ ����
    DWORD commThreadId;
    hCommThread = CreateThread(NULL, 0, CommunicationThreadFunc, NULL, 0, &commThreadId);
    if (hCommThread == NULL) {
        MessageBox(NULL, L"Communication ������ ���� ����", L"Error", MB_OK);
        return -1;
    }

    // �޽��� ������ ���� ����
    bool gameStarted = false;
    ZeroMemory(&Message, sizeof(Message));
    while (Message.message != WM_QUIT) {
        if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else {
            loop.Update();
            loop.Render();
        }
        Sleep(1);
    }

    CloseHandle(hCommThread);
    CloseHandle(gGameStartEvent);
    closesocket(g_serverSocket);
    WSACleanup();

    return Message.wParam;
}

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    switch (iMessage) {
    case WM_CREATE:
        loop.Init(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, iMessage, wParam, lParam);
    }
    return 0;
}



//bool SetSocketTimeout(SOCKET& sock, int timeoutInSeconds) {
//    int timeoutInMillis = timeoutInSeconds * 1000;
//    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis)) == SOCKET_ERROR) {
//        std::cerr << "Failed to set recv timeout: " << WSAGetLastError() << std::endl;
//        return false;
//    }
//    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis)) == SOCKET_ERROR) {
//        std::cerr << "Failed to set send timeout: " << WSAGetLastError() << std::endl;
//        return false;
//    }
//    return true;
//}
//
//// ��Ŷ ���� �Լ�
//void SendPacket(SOCKET& sock, const void* packet, size_t packetSize) {
//    if (send(sock, reinterpret_cast<const char*>(packet), packetSize, 0) == SOCKET_ERROR) {
//        std::cerr << "Failed to send packet: " << WSAGetLastError() << std::endl;
//    }
//    else
//    {
//        std::cout << "Success" << std::endl;
//    }
//}
//
//// ��Ŷ ���� �Լ�
//void RecvPackets(SOCKET& sock) {
//    char recvBuffer[1024];
//    int bytesReceived;
//
//    while (true) {
//        bytesReceived = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
//        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
//            std::cerr << "recv() failed or connection closed: " << WSAGetLastError() << std::endl;
//            break;
//        }
//
//        uint8_t opCode = recvBuffer[0];
//        std::cout << "Received packet with opCode: " << (int)opCode << ", size: " << bytesReceived << " bytes" << std::endl;
//
//        // ��Ŷ ó�� ���� (����)
//        switch (opCode) {
//        case 11: {
//            GTime_Packet gTimePacket;
//            memcpy(&gTimePacket, recvBuffer, sizeof(GTime_Packet));
//            std::cout << "Game Time: " << gTimePacket.gameTime << "ms" << std::endl;
//            break;
//        }
//        case 13: {
//            ObjectInfo_Packet objectInfo;
//            memcpy(&objectInfo, recvBuffer, sizeof(ObjectInfo_Packet));
//            std::cout << "Received Object Info for Scene: " << (int)objectInfo.m_scene << std::endl;
//            break;
//        }
//        default:
//            std::cout << "Unknown opCode: " << (int)opCode << std::endl;
//            break;
//        }
//    }
//}