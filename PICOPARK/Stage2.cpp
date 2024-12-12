#include "Stage2.h"

Stage2::Stage2()
{
}

Stage2::~Stage2()
{
    // Clear all objects managed by ObjectManager when the stage is destroyed.
    ObjectManager::GetInstance().Clear();
}

void Stage2::Init()
{
    currentMapID = 0;

    BlockImage[1].Load(L"assets/블럭1.png");
    BlockImage[2].Load(L"assets/열쇠.png");
    BlockImage[3].Load(L"assets/닫은문.png");
    BlockImage[4].Load(L"assets/열린문.png");
    BlockImage[5].Load(L"assets/블럭2.png");
    BlockImage[6].Load(L"assets/빨간스위치X.png");
    BlockImage[7].Load(L"assets/빨간스위치O.png");
    BlockImage[8].Load(L"assets/파란스위치X.png");
    BlockImage[9].Load(L"assets/파란스위치O.png");

    InitializeMap();

    // Use ObjectManager to create and manage the player
    CPlayer* player1 = ObjectManager::GetInstance().CreateObject<CPlayer>(PlayerType::P1);
    player1->SetPlayerID(0); // ID 설정

    CPlayer* player2 = ObjectManager::GetInstance().CreateObject<CPlayer>(PlayerType::P2);
    player2->SetPlayerID(1); // ID 설정

}

void Stage2::Update()
{
    for (int i = 0; i < 2; i++) {
        CPlayer* player = ObjectManager::GetInstance().GetPlayerByID(i);
        if (player->Gethaskey()) {
            maps[1][1][5] = 0;
            for (int row = 0; row < 8; ++row) {
                for (int col = 0; col < 8; ++col) {
                    if (maps[2][row][col] == 1) {
                        maps[2][row][col] = 6; // 블록 활성화
                    }
                }
            }
        }
    }

    if (openDoor)
        maps[2][2][7] = 5;

    // Update all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects)
    {
        obj->Update();
    }
}

void Stage2::Render(HDC hdc, uint32_t mapid)
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
            obj->Render(hdc); // 일반 객체는 그대로 렌더링
        }
    }
}

void Stage2::InitializeMap() {  // 일반블럭 2, 열쇠 3, 문 4 or 5, 투명블록 1 or 6, 빨간 스위치 7 or 8, 파란 스위치 9, 10
    // 1-1 맵 데이터 설정
    int tempMap1[8][8] = {
        {0, 0, 0, 0, 0, 0, 2, 0},
        {0, 0, 0, 0, 0, 0, 9, 0},
        {0, 0, 0, 0, 0, 0, 2, 2},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 7, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

    // 1-2 맵 데이터 설정
    int tempMap2[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 2},
        {0, 0, 0, 0, 0, 3, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {0, 0, 0, 0, 0, 0, 0, 2},
        {0, 0, 0, 0, 0, 0, 0, 2},
        {0, 0, 0, 0, 0, 0, 0, 2},
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
        {0, 0, 0, 1, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

    // 맵 데이터 복사
    memcpy(maps[0], tempMap1, sizeof(tempMap1));
    memcpy(maps[1], tempMap2, sizeof(tempMap2));
    memcpy(maps[2], tempMap3, sizeof(tempMap3));
}
