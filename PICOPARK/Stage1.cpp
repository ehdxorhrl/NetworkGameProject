#include "Stage1.h"

Stage1::Stage1()
{
}

Stage1::~Stage1()
{
    // Clear all objects managed by ObjectManager when the stage is destroyed.
    ObjectManager::GetInstance().Clear();
}

void Stage1::Init()
{
    currentMapID = 0;

    BlockImage[1].Load(L"assets/블럭1.png");
    BlockImage[2].Load(L"assets/열쇠.png");
    BlockImage[3].Load(L"assets/닫은문.png");
    BlockImage[4].Load(L"assets/열린문.png");
    BlockImage[5].Load(L"assets/블럭2.png");

    InitializeMap();

    // Use ObjectManager to create and manage the player
    CPlayer* player1 = ObjectManager::GetInstance().CreateObject<CPlayer>(PlayerType::P1);
    player1->SetPlayerID(0); // ID 설정
    player1->SetPos(150, 675);

    CPlayer* player2= ObjectManager::GetInstance().CreateObject<CPlayer>(PlayerType::P2);
    player2->SetPlayerID(1); // ID 설정
    player2->SetPos(650, 675);

}

void Stage1::Update()
{
    for (int i = 0; i < 2; i++) {
        CPlayer* player = ObjectManager::GetInstance().GetPlayerByID(i);
        if (player->Gethaskey()) {
            maps[0][3][0] = 0;
            for (int row = 0; row < 8; ++row) {
                for (int col = 0; col < 8; ++col) {
                    if (maps[0][row][col] == 1) {
                        maps[0][row][col] = 6; // 블록 활성화
                    }
                }
            }
        }
    }

    if (openDoor)
        maps[0][0][7] = 5;

    // Update all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects)
    {
        obj->Update();
    }
}

void Stage1::Render(HDC hdc, uint32_t mapid)
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

void Stage1::InitializeMap() {  // 일반블럭 1, 열쇠 2, 문 3 or 4, 투명블록 5 
    // 1-1 맵 데이터 설정
    int tempMap1[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 4},
        {0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 1, 0, 0, 0, 0},
        {3, 0, 1, 0, 0, 0, 0, 0},
        {2, 2, 2, 0, 2, 0, 2, 2},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 0, 2, 0, 0, 2, 2}
    };

    // 1-2 맵 데이터 설정
    int tempMap2[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 0, 0},
        {2, 2, 0, 0, 0, 0, 0, 2},
        {0, 0, 0, 0, 0, 0, 2, 2},
        {0, 0, 0, 0, 0, 2, 2, 2},
        {2, 2, 0, 0, 2, 2, 2, 2}
    };

    // 맵 데이터 복사
    std::memcpy(maps[0], tempMap1, sizeof(tempMap1));
    std::memcpy(maps[1], tempMap2, sizeof(tempMap2));

}
