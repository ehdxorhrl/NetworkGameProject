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
HANDLE hCommThread = NULL;  // Communication 스레드 핸들
HANDLE hServerSignalThread = NULL;  // 서버 신호 대기 스레드 핸들

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// Communication 스레드 함수 - 수신 데이터를 바로 처리
DWORD WINAPI CommunicationThreadFunc(LPVOID lpParam) {
    char buffer[BUFFER_SIZE];

    while (true) {
        // 서버에서 데이터 수신
        int bytesReceived = recv(g_serverSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            uint8_t packetType = buffer[0]; // 첫 바이트는 packetType으로 가정
            switch (packetType) {
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
                        sceneManager.ChangeScene(); // 씬 전환
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

        Sleep(1); // CPU 사용량 제어
    }
    return 0;
}

// 서버 연결 함수
bool ConnectToServer(const char* serverIP, int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        MessageBox(NULL, L"WSAStartup 실패", L"Error", MB_OK);
        return false;
    }

    g_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_serverSocket == INVALID_SOCKET) {
        MessageBox(NULL, L"소켓 생성 실패", L"Error", MB_OK);
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    if (connect(g_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        std::string errorMsg = "서버 연결 실패: " + std::to_string(errorCode);
        MessageBoxA(NULL, errorMsg.c_str(), "Error", MB_OK);
        closesocket(g_serverSocket);
        WSACleanup();
        return false;
    }

    return true;
}

// WinMain 함수
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    HWND hWnd;
    MSG msg;
    WNDCLASSEX wcex;

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
        MessageBox(nullptr, L"Call to RegisterClassEx failed!", L"Error", NULL);
        return 1;
    }

    hWnd = CreateWindow(
        L"GameClass",
        L"Game Title",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hWnd) {
        MessageBox(nullptr, L"Call to CreateWindow failed!", L"Error", NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    CGameloop gameLoop;
    gameLoop.Init(hWnd);

    // 소켓 연결 설정
    if (ConnectToServer("127.0.0.1", 9000)) {
        gameLoop.SetServerSocket(g_serverSocket); // 서버 소켓 전달
    }

    // 게임 루프 실행
    const int FRAME_INTERVAL = 33; // 초당 30프레임 = 약 33ms
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime).count() >= FRAME_INTERVAL) {
            lastFrameTime = currentTime;

            // 게임 업데이트 및 렌더링
            gameLoop.Update();
            gameLoop.Render();
        }

        Sleep(1); // CPU 사용량 제어
    }

    // 종료 처리
    closesocket(g_serverSocket);
    WSACleanup();

    return (int)msg.wParam;
}

// 윈도우 프로시저
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
//// 패킷 전송 함수
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
//// 패킷 수신 함수
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
//        // 패킷 처리 로직 (예시)
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