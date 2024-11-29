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

    static float timeAccumulator = 0.0f; // �ð� ���� ����
    timeAccumulator += timeManager.GetDeltaTime();

    const float PACKET_INTERVAL = 1.0f / 30.0f; // �ʴ� 30ȸ (�� 33ms)

    if (timeAccumulator >= PACKET_INTERVAL) {
        // �ֱ������θ� ��Ŷ ����
        for (int key = 0; key < KEY_TYPE_COUNT; key++) {
            if (inputManager.IsKeyDown(key)) {
                // Input_Packet ����
                Input_Packet inputPacket;
                inputPacket.m_playerID = GetID(); // �÷��̾� ID
                inputPacket.inputType = static_cast<KT>(key); // Ű Ÿ��
                inputPacket.inputState = static_cast<KS>(inputManager.GetState(static_cast<KeyType>(key)));
                inputPacket.inputTime = static_cast<uint64_t>(timeManager.GetDeltaTime() * 1000);

                // ��Ŷ ����
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

        timeAccumulator -= PACKET_INTERVAL; // ���ݸ�ŭ �ð� ����
    }

    SceneManager.Update();
}

void CGameloop::Render() {
    HDC mdc;
    HBITMAP hBitmap;
    RECT rt;

    GetClientRect(hwnd, &rt);
    hdc = GetDC(hwnd);
    mdc = CreateCompatibleDC(hdc); // �޸� DC ����
    hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom); // ��Ʈ�� ����
    SelectObject(mdc, hBitmap); // ��Ʈ���� �޸� DC�� ����

    // ����� ��� �簢������ ä��
    Rectangle(mdc, 0, 0, rt.right, rt.bottom);

    // �� �Ŵ��� ������
    if (&SceneManager) {
        SceneManager.Render(mdc);
        OutputDebugString(L"SceneManager::Render called\n");
    }

    // �� ������ ������ ȭ�鿡 ����
    BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY);

    // GDI ��ü ���󺹱� �� �ڿ� ����
    DeleteObject(hBitmap); // ��Ʈ�� ����
    DeleteDC(mdc); // �޸� DC ����
    ReleaseDC(hwnd, hdc);
}

void CGameloop::SetServerSocket(SOCKET socket) {
    serverSocket = socket;
}