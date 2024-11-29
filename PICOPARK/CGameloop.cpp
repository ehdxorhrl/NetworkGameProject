#include "stdafx.h"
#include "CGameloop.h"

CGameloop::CGameloop() {}

CGameloop::~CGameloop() {}

void CGameloop::Init(HWND hWnd) {
    hwnd = hWnd;
    timeManager.Init();
    inputManager.Init(hwnd);
    SceneManager.Init();
}

void CGameloop::Update() {
    timeManager.Update();
    inputManager.Update();

    static float timeAccumulator = 0.0f; // 시간 누적 변수
    timeAccumulator += timeManager.GetDeltaTime();

    const float PACKET_INTERVAL = 1.0f / 30.0f; // 초당 30회 (약 33ms)

    if (timeAccumulator >= PACKET_INTERVAL) {
        // 주기적으로만 패킷 전송
        for (int key = 0; key < KEY_TYPE_COUNT; key++) {
            if (inputManager.IsKeyDown(key)) {
                // Input_Packet 생성
                Input_Packet inputPacket;
                inputPacket.m_playerID = GetID(); // 플레이어 ID
                inputPacket.inputType = static_cast<KT>(key); // 키 타입
                inputPacket.inputState = static_cast<KS>(inputManager.GetState(static_cast<KeyType>(key)));
                inputPacket.inputTime = static_cast<uint64_t>(timeManager.GetDeltaTime() * 1000);

                // 패킷 전송
                if (serverSocket != INVALID_SOCKET) {
                    int sendResult = send(serverSocket, reinterpret_cast<const char*>(&inputPacket), sizeof(Input_Packet), 0);
                    if (sendResult == SOCKET_ERROR) {
                        std::cerr << "Failed to send input packet: " << WSAGetLastError() << std::endl;
                    }
                    else {
                        std::cout << "Input packet sent for key: " << key << std::endl;
                    }
                }
            }
        }

        timeAccumulator -= PACKET_INTERVAL; // 간격만큼 시간 차감
    }

    SceneManager.Update();
}

void CGameloop::Render() {
    HDC mdc;
    HBITMAP hBitmap;
    RECT rt;

    GetClientRect(hwnd, &rt);
    hdc = GetDC(hwnd);
    mdc = CreateCompatibleDC(hdc); // 메모리 DC 생성
    hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom); // 비트맵 생성
    SelectObject(mdc, hBitmap); // 비트맵을 메모리 DC에 선택

    // 배경을 흰색 사각형으로 채움
    Rectangle(mdc, 0, 0, rt.right, rt.bottom);

    // 씬 매니저 렌더링
    if (&SceneManager) {
        SceneManager.Render(mdc);
        OutputDebugString(L"SceneManager::Render called\n");
    }

    // 백 버퍼의 내용을 화면에 복사
    BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY);

    // GDI 객체 원상복귀 및 자원 해제
    DeleteObject(hBitmap); // 비트맵 해제
    DeleteDC(mdc); // 메모리 DC 해제
    ReleaseDC(hwnd, hdc);
}

void CGameloop::SetServerSocket(SOCKET socket) {
    serverSocket = socket;
}