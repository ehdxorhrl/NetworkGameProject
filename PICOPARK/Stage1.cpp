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

    BlockImage[0].Load(L"assets/블럭1.png");
    BlockImage[1].Load(L"assets/열쇠.png");
    BlockImage[2].Load(L"assets/닫은문.png");
    BlockImage[3].Load(L"assets/열린문.png");
    BlockImage[4].Load(L"assets/블럭2.png");

    InitializeMap();

    // Use ObjectManager to create and manage the player
    CPlayer* player = ObjectManager::GetInstance().CreateObject<CPlayer>();

}

void Stage1::Update()
{
    CPlayer* player = ObjectManager::GetInstance().GetPlayer(); // 플레이어 객체 가져오기

    if (!player) {
        return;
    }

    // Update all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects)
    {
        obj->Update();
    }

    currentMapID = player->GetstageNum();
}

void Stage1::Render(HDC hdc)
{
    bool Initflag = false;
    // Render all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects)
    {
        obj->Render(hdc);
        Initflag = true;
    }

    if(Initflag){
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if(maps[currentMapID][i][j] != 0)
                BlockImage[maps[currentMapID][i][j]-1].Draw(hdc, j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, 0, 0, 100, 100);}
}

void Stage1::InitializeMap() {  // 일반블럭 1, 열쇠 2, 문 3 or 4, 투명블록 5 
    // 1-1 맵 데이터 설정
    int tempMap1[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 3},
        {0, 0, 0, 0, 5, 5, 5, 5},
        {0, 0, 0, 5, 0, 0, 0, 0},
        {2, 0, 5, 0, 0, 0, 0, 0},
        {1, 1, 1, 0, 1, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 1, 0, 0, 1, 1}
    };

    // 1-2 맵 데이터 설정
    int tempMap2[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0},
        {1, 1, 0, 0, 0, 0, 0, 1},                      
        {0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 1, 1, 1},
        {1, 1, 0, 0, 1, 1, 1, 1}
    };

    // 맵 데이터 복사
    memcpy(maps[0], tempMap1, sizeof(tempMap1));
    memcpy(maps[1], tempMap2, sizeof(tempMap2));

    currentMapID = 0; // 초기 맵 설정
}
