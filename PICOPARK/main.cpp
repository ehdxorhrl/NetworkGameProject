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

HANDLE hCommThread = NULL;  // Communication ������ �ڵ�
HANDLE hServerSignalThread = NULL;  // ���� ��ȣ ��� ������ �ڵ�

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// Communication ������ �Լ� - ���� �����͸� �ٷ� ó��
DWORD WINAPI CommunicationThreadFunc(LPVOID lpParam) {
    char buffer[BUFFER_SIZE];

    while (true) {
        // �������� ������ ����
        int bytesReceived = recv(g_serverSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            uint8_t packetType = buffer[0]; // ù ����Ʈ�� packetType���� ����
            switch (static_cast<int>(packetType)) {
            case 12: { // PlayerIDResponsePacket
                if (bytesReceived == sizeof(PlayerIDResponsePacket)) {
                    PlayerIDResponsePacket idResponse;
                    memcpy(&idResponse, buffer, sizeof(PlayerIDResponsePacket));
                    if (idResponse.isSuccess) {
                        loop.SetID(idResponse.m_playerID);
                    }
                    else {
                        OutputDebugString(L"Failed to receive Player ID.\n");
                        closesocket(g_serverSocket);
                        return 1;
                    }
                }
                else {
                    OutputDebugString(L"Invalid PlayerIDResponsePacket size.\n");
                }
                break;
            }
            case 102: { // GameStart_Packet           
                if (bytesReceived == sizeof(GameStart_Packet)) {
                    GameStart_Packet startPacket;
                    memcpy(&startPacket, buffer, sizeof(GameStart_Packet));
                    if (startPacket.isGameStarted) {
                        OutputDebugString(L"Game Start Packet received.\n");
                        SceneManager& sceneManager = SceneManager::GetInstance();
                        sceneManager.ChangeScene(); // �� ��ȯ

                    }
                    else {
                        OutputDebugString(L"Game Start Packet indicates game not started.\n");
                    }
                }
                else {
                    OutputDebugString(L"Invalid GameStart_Packet size.\n");
                }
                break;
            }
            default:
                OutputDebugString(L"Unhandled packet type.\n");
                break;
            }
        }
        else if (bytesReceived == 0) {
            OutputDebugString(L"Server closed connection.\n");
            break;
        }
        else if (bytesReceived == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error != WSAETIMEDOUT) {
                OutputDebugString(L"Socket error, closing thread.\n");
                break;
            }
        }

       // Sleep(1); // CPU ��뷮 ����
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
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    HWND hWnd;
    MSG Message;
    WNDCLASSEX wcex; // ����ü ����
    char buffer[BUFFER_SIZE]; // ������ ���� ����
    g_hInst = hInstance;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"GameClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex)) {
        DWORD error = GetLastError();
        std::wstring errorMsg = L"RegisterClassEx failed with error code: " + std::to_wstring(error);
        MessageBox(nullptr, errorMsg.c_str(), L"Error", NULL);
        return 1;
    }

    hWnd = CreateWindow(
        L"GameClass",
        L"Game Title",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 800,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // ����� �Ķ���� �Ľ�
    std::wstring cmdLine(lpCmdLine);
    std::wistringstream cmdStream(cmdLine);
    std::wstring ip;
    int port = 9000;
    if (!(cmdStream >> ip >> port)) {
        ip = L"127.0.0.1";
        port = 9000;
    }

    // ���� ����
    std::string ipStr(ip.begin(), ip.end());
    if (!ConnectToServer(ipStr.c_str(), port)) {
        MessageBox(nullptr, L"Failed to connect to server!", L"Error", NULL);
        return 1;
    }

    hCommThread = CreateThread(
        NULL, 0,
        CommunicationThreadFunc,
        NULL, 0, NULL
    );

    if (!hCommThread) {
        MessageBox(nullptr, L"Failed to create communication thread!", L"Error", NULL);
        return 1;
    }

    

    if (!hWnd) {
        MessageBox(nullptr, L"Call to CreateWindow failed!", L"Error", NULL);
        return 1;
    }

    loop.SetServerSocket(g_serverSocket); // ���� ����

    ZeroMemory(&Message, sizeof(Message));
    while (Message.message != WM_QUIT) {
        if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else{   // ���� ������Ʈ �� ������
            loop.Update();
            loop.Render();
        }

        Sleep(1); // CPU ��뷮 ����
    }

    // ���� ó��
    closesocket(g_serverSocket);
    WSACleanup();

    return (int)Message.wParam;
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