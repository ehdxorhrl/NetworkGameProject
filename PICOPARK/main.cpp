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
    char buffer[1024];

    // 소켓 타임아웃 설정
    int timeoutInMillis = 100; // 100ms 타임아웃
    setsockopt(g_serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis));
    setsockopt(g_serverSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis));

    while (true) {
        int bytesReceived = recv(g_serverSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            // 받은 데이터 처리
            OutputDebugStringA(buffer);
            OutputDebugStringA("\n");
        }
        else if (bytesReceived == 0) {
            // 서버가 연결을 종료
            OutputDebugString(L"Server closed connection\n");
            break;
        }
        else if (bytesReceived == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT) {
                // 타임아웃 발생, 계속 루프 유지
                OutputDebugString(L"Socket recv timeout\n");
            }
            else {
                // 치명적인 소켓 오류 발생
                std::wstring errorMsg = L"Socket error: " + std::to_wstring(error);
                OutputDebugString(errorMsg.c_str());
                break;
            }
        }

        // 필요한 경우 Sleep으로 CPU 사용량 제어
        Sleep(1);
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
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszCmdParam, int nCmdShow) {
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    gGameStartEvent = CreateEvent(
        NULL,   // 기본 보안 속성
        TRUE,   // 수동 재설정 (TRUE: 수동, FALSE: 자동)
        FALSE,  // 초기 상태는 비신호 상태
        NULL    // 이벤트 이름 (익명)
    );

    if (!gGameStartEvent) {
        MessageBox(NULL, L"이벤트 생성 실패", L"Error", MB_OK);
        return -1;
    }

    // 기본 IP 및 포트 설정
    std::string ip = "127.0.0.1";
    int port = 8080;

    // 명령어 인자 파싱
    if (lpszCmdParam && strlen(lpszCmdParam) > 0) {
        std::istringstream iss(lpszCmdParam);
        if (!(iss >> ip >> port)) {
            MessageBox(NULL, L"IP 주소와 포트 번호를 올바르게 입력하세요", L"Error", MB_OK);
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

    RECT rect = { 0, 0, 800, 800 }; // 원하는 클라이언트 영역 크기
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE); // 전체 창 크기 조정

    hWnd = CreateWindow(
        lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
        rect.bottom - rect.top, NULL, NULL, hInstance, NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 서버 연결
    if (!ConnectToServer(ip.c_str(), port)) {
        return -1;
    }

    // Communication 스레드 생성
    DWORD commThreadId;
    hCommThread = CreateThread(NULL, 0, CommunicationThreadFunc, NULL, 0, &commThreadId);
    if (hCommThread == NULL) {
        MessageBox(NULL, L"Communication 스레드 생성 실패", L"Error", MB_OK);
        return -1;
    }

    // 메시지 루프와 게임 루프
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