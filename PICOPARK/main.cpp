#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include "CGameloop.h"
#pragma comment(lib, "ws2_32.lib")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Program gimal project";
CGameloop loop;
//SOCKET g_serverSocket = INVALID_SOCKET;

//HANDLE hCommThread;  // Communication 스레드 핸들

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

//bool ConnectToServer(const char* serverIP, int port) {
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        MessageBox(NULL, L"WSAStartup 실패", L"Error", MB_OK);
//        return false;
//    }
//
//    g_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//    if (g_serverSocket == INVALID_SOCKET) {
//        MessageBox(NULL, L"소켓 생성 실패", L"Error", MB_OK);
//        WSACleanup();
//        return false;
//    }
//
//    sockaddr_in serverAddr;
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_port = htons(port);
//    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
//
//    if (connect(g_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
//        MessageBox(NULL, L"서버 연결 실패", L"Error", MB_OK);
//        closesocket(g_serverSocket);
//        WSACleanup();
//        return false;
//    }
//
//    MessageBox(NULL, L"서버 연결 성공", L"Info", MB_OK);
//    return true;
//}

// Communication 스레드 함수 - 수신 데이터를 바로 처리
//DWORD WINAPI CommunicationThreadFunc(LPVOID lpParam) {
//    char buffer[256];
//
//    while (true) {
//        // 서버로부터 데이터 수신
//        int bytesReceived = recv(g_serverSocket, buffer, sizeof(buffer) - 1, 0);
//        if (bytesReceived > 0) {
//            buffer[bytesReceived] = '\0';  // 수신된 데이터에 null-terminator 추가
//            loop.UpdateGameState(buffer);  // 수신된 데이터를 바로 게임 상태에 반영
//        }
//        else if (bytesReceived == 0 || bytesReceived == SOCKET_ERROR) {
//            break;  // 서버 연결 끊김 또는 오류 발생 시 종료
//        }
//
//        // SendQueue 없이 바로 데이터를 서버로 전송
//        std::string dataToSend = loop.GetDataToSend();  // 전송할 데이터를 게임 로직에서 가져옴
//        if (!dataToSend.empty()) {
//            send(g_serverSocket, dataToSend.c_str(), dataToSend.size(), 0);
//        }
//    }
//
//    return 0;
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    // 명령어 인자 파싱 (IP와 포트번호)
    //std::string ip;
    //int port;
    //std::istringstream iss(lpszCmdParam);

    //if (!(iss >> ip >> port)) {
    //    MessageBox(NULL, L"IP 주소와 포트 번호를 올바르게 입력하세요", L"Error", MB_OK);
    //    return -1;
    //}

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
        0, 0, MAPSIZE, MAPSIZE, NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 서버 연결
    //if (!ConnectToServer(ip.c_str(), port)) {
    //    return -1;
    //}

    // Communication 스레드 생성
    //DWORD threadId;
    //hCommThread = CreateThread(NULL, 0, CommunicationThreadFunc, NULL, 0, &threadId);
    //if (hCommThread == NULL) {
    //    MessageBox(NULL, L"Communication 스레드 생성 실패", L"Error", MB_OK);
    //    return -1;
    //}

    // 메시지 루프와 게임 루프 (메인 스레드)
    ZeroMemory(&Message, sizeof(Message));
    while (Message.message != WM_QUIT) {
        if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else {
            loop.Update();  // 게임 상태 업데이트
            loop.Render();  // 렌더링
        }
    }

    // 스레드가 종료될 때까지 대기
    //WaitForSingleObject(hCommThread, INFINITE);

    // 스레드 핸들 닫기
    //CloseHandle(hCommThread);

    //closesocket(g_serverSocket);
    WSACleanup();
    return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg) {
    case WM_CREATE:
        loop.Init(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return (DefWindowProc(hWnd, iMsg, wParam, lParam));
    }
    return 0;
}