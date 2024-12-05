#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "CGameloop.h"
#include "LoadingScene.h"
#include "ObjectManager.h"
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
        int bytesReceived = recv(g_serverSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            uint8_t packetType = buffer[0]; // ��Ŷ Ÿ��
            std::unique_ptr<BasePacket> packet;

            switch (packetType) {
            case 12: { // PlayerIDResponsePacket
                packet = std::make_unique<PlayerIDResponsePacket>();
                memcpy(packet.get(), buffer, sizeof(PlayerIDResponsePacket));
                break;
            }
            case 13: { // ObjectInfo_Packet
                packet = std::make_unique<ObjectInfo_Packet>();
                memcpy(packet.get(), buffer, sizeof(ObjectInfo_Packet));
                break;
            }
            case 102: { // GameStart_Packet
                packet = std::make_unique<GameStart_Packet>();
                memcpy(packet.get(), buffer, sizeof(GameStart_Packet));
                break;
            }
            default:
                OutputDebugString(L"Unhandled packet type.\n");
                continue;
            }

            EnterCriticalSection(&loop.UpdateCS);
            loop.RecvQueue.push(std::move(packet));
            LeaveCriticalSection(&loop.UpdateCS);
        }
        else if (bytesReceived <= 0) {
            if (bytesReceived == 0) {
                OutputDebugString(L"Server closed connection.\n");
            }
            else {
                OutputDebugString(L"Socket error.\n");
            }
            break;
        }

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

    loop.Init(hWnd);

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
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, iMessage, wParam, lParam);
    }
    return 0;
}