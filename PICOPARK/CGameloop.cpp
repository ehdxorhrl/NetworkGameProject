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
    static uint64_t lastSentTime = 0; // 마지막 전송된 시간 (초기값은 0으로 설정)

    // 현재 클라이언트 시간을 가져옴 (ms 단위)
    uint64_t currentTime = static_cast<uint64_t>(timeManager.GetDeltaTime() * 1000);

    // 초기화: 첫 패킷 전송 전에 lastSentTime을 currentTime으로 설정
    if (lastSentTime == 0) {
        lastSentTime = currentTime;
    }

    timeAccumulator += timeManager.GetDeltaTime();
    const float PACKET_INTERVAL = 1.0f / 30.0f; // 초당 30회 (약 33ms)

    if (timeAccumulator >= PACKET_INTERVAL) {
        // 주기적으로 패킷 전송
        bool keyflag = false;
        for (int key = 0; key < KEY_TYPE_COUNT; key++) {
            if (inputManager.IsKeyDown(key)) {
                // Input_Packet 생성
                Input_Packet inputPacket;
                inputPacket.m_playerID = GetID(); // 플레이어 ID
                inputPacket.inputType = static_cast<KT>(key); // 키 타입
                inputPacket.inputState = static_cast<KS>(inputManager.GetState(static_cast<KeyType>(key)));
                inputPacket.inputTime = currentTime - lastSentTime; // 상대 시간 계산

                // 패킷 전송
                if (serverSocket != INVALID_SOCKET) {
                    int sendResult = send(serverSocket, reinterpret_cast<const char*>(&inputPacket), sizeof(Input_Packet), 0);
                    if (sendResult == SOCKET_ERROR) {
                        std::cerr << "Failed to send input packet: " << WSAGetLastError() << std::endl;
                    }
                    else {
                        std::cout << "Input packet sent for key: " << key << ", Input Time: " << inputPacket.inputTime << " ms" << std::endl;
                    }
                }
                keyflag = true;
            }
        }

        if (!keyflag) {
            // 입력이 없을 경우 기본 패킷 전송
            Input_Packet inputPacket;
            inputPacket.m_playerID = GetID(); // 플레이어 ID
            inputPacket.inputType = KT::None; // 키 타입
            inputPacket.inputState = KS::None;
            inputPacket.inputTime = 0; // 상대 시간 계산

            // 패킷 전송
            if (serverSocket != INVALID_SOCKET) {
                int sendResult = send(serverSocket, reinterpret_cast<const char*>(&inputPacket), sizeof(Input_Packet), 0);
                if (sendResult == SOCKET_ERROR) {
                    std::cerr << "Failed to send input packet: " << WSAGetLastError() << std::endl;
                }
            }
        }

        lastSentTime = currentTime; // 마지막 전송 시간 갱신
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