#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include "CGameloop.h"
#pragma comment(lib, "ws2_32.lib")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Program gimal project";
CGameloop loop;
//SOCKET g_serverSocket = INVALID_SOCKET;

//HANDLE hCommThread;  // Communication ������ �ڵ�

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

//bool ConnectToServer(const char* serverIP, int port) {
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        MessageBox(NULL, L"WSAStartup ����", L"Error", MB_OK);
//        return false;
//    }
//
//    g_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//    if (g_serverSocket == INVALID_SOCKET) {
//        MessageBox(NULL, L"���� ���� ����", L"Error", MB_OK);
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
//        MessageBox(NULL, L"���� ���� ����", L"Error", MB_OK);
//        closesocket(g_serverSocket);
//        WSACleanup();
//        return false;
//    }
//
//    MessageBox(NULL, L"���� ���� ����", L"Info", MB_OK);
//    return true;
//}

// Communication ������ �Լ� - ���� �����͸� �ٷ� ó��
//DWORD WINAPI CommunicationThreadFunc(LPVOID lpParam) {
//    char buffer[256];
//
//    while (true) {
//        // �����κ��� ������ ����
//        int bytesReceived = recv(g_serverSocket, buffer, sizeof(buffer) - 1, 0);
//        if (bytesReceived > 0) {
//            buffer[bytesReceived] = '\0';  // ���ŵ� �����Ϳ� null-terminator �߰�
//            loop.UpdateGameState(buffer);  // ���ŵ� �����͸� �ٷ� ���� ���¿� �ݿ�
//        }
//        else if (bytesReceived == 0 || bytesReceived == SOCKET_ERROR) {
//            break;  // ���� ���� ���� �Ǵ� ���� �߻� �� ����
//        }
//
//        // SendQueue ���� �ٷ� �����͸� ������ ����
//        std::string dataToSend = loop.GetDataToSend();  // ������ �����͸� ���� �������� ������
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

    // ��ɾ� ���� �Ľ� (IP�� ��Ʈ��ȣ)
    //std::string ip;
    //int port;
    //std::istringstream iss(lpszCmdParam);

    //if (!(iss >> ip >> port)) {
    //    MessageBox(NULL, L"IP �ּҿ� ��Ʈ ��ȣ�� �ùٸ��� �Է��ϼ���", L"Error", MB_OK);
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

    // ���� ����
    //if (!ConnectToServer(ip.c_str(), port)) {
    //    return -1;
    //}

    // Communication ������ ����
    //DWORD threadId;
    //hCommThread = CreateThread(NULL, 0, CommunicationThreadFunc, NULL, 0, &threadId);
    //if (hCommThread == NULL) {
    //    MessageBox(NULL, L"Communication ������ ���� ����", L"Error", MB_OK);
    //    return -1;
    //}

    // �޽��� ������ ���� ���� (���� ������)
    ZeroMemory(&Message, sizeof(Message));
    while (Message.message != WM_QUIT) {
        if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else {
            loop.Update();  // ���� ���� ������Ʈ
            loop.Render();  // ������
        }
    }

    // �����尡 ����� ������ ���
    //WaitForSingleObject(hCommThread, INFINITE);

    // ������ �ڵ� �ݱ�
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