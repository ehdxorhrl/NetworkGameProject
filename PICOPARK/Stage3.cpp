#include "Stage3.h"

Stage3::Stage3()
{
}

Stage3::~Stage3()
{
    // Clear all objects managed by ObjectManager when the stage is destroyed.
    ObjectManager::GetInstance().Clear();
}

void Stage3::Init()
{
    currentMapID = 0;

    BlockImage[1].Load(L"assets/��1.png");
    BlockImage[2].Load(L"assets/����.png");
    BlockImage[3].Load(L"assets/������.png");
    BlockImage[4].Load(L"assets/������.png");
    BlockImage[5].Load(L"assets/��3.png");
    BlockImage[6].Load(L"assets/�ʷϽ���ġX.png");

    InitializeMap();

    // Use ObjectManager to create and manage the player
    CPlayer* player1 = ObjectManager::GetInstance().CreateObject<CPlayer>(PlayerType::P1);
    player1->SetPlayerID(0); // ID ����

    CPlayer* player2 = ObjectManager::GetInstance().CreateObject<CPlayer>(PlayerType::P2);
    player2->SetPlayerID(1); // ID ����

}

void Stage3::Update()
{
    for (int i = 0; i < 2; i++) {
        CPlayer* player = ObjectManager::GetInstance().GetPlayerByID(i);
        if (player->Gethaskey()) {
            maps[0][6][1] = 0;
            for (int row = 0; row < 8; ++row) {
                for (int col = 0; col < 8; ++col) {
                    if (maps[2][row][col] == 1) {
                        maps[2][row][col] = 6; // ��� Ȱ��ȭ
                    }
                }
            }
        }
    }

    if (openDoor)
        maps[2][2][7] = 5;

    if (switchpress) {
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (maps[0][row][col] == 6) {
                    maps[0][row][col] = 1; // ��� Ȱ��ȭ
                }
            }
        }
    }


    // Update all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects)
    {
        obj->Update();
    }
}

void Stage3::Render(HDC hdc, uint32_t mapid)
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (maps[mapid][i][j] != 0)
                BlockImage[maps[mapid][i][j] - 1].Draw(hdc, j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, 0, 0, 100, 100);

    // Render all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects) {
        if (auto* player = dynamic_cast<CPlayer*>(obj)) {
            if (player->GetstageNum() == mapid) {
                player->Render(hdc);
            }
        }
        else {
            obj->Render(hdc); // �Ϲ� ��ü�� �״�� ������
        }
    }
}

void Stage3::InitializeMap() {  // �Ϲݺ� 2, ���� 3, �� 4 or 5, ������ 1 or 6, ���� ����ġ 7 or 8, �Ķ� ����ġ 9, 10
    // 1-1 �� ������ ����
    int tempMap1[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {6, 6, 6, 0, 0, 0, 0, 0},
        {0, 3, 6, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

    // 1-2 �� ������ ����
    int tempMap2[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

    int tempMap3[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 4},
        {0, 0, 0, 0, 0, 0, 2, 2},
        {0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 7, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

    // �� ������ ����
    memcpy(maps[0], tempMap1, sizeof(tempMap1));
    memcpy(maps[1], tempMap2, sizeof(tempMap2));
    memcpy(maps[2], tempMap3, sizeof(tempMap3));
}
