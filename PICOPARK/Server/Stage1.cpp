#include "stdafx.h"
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
    InitializeMap();

    // 플레이어 생성
    CPlayer* player = ObjectManager::GetInstance().CreateObject<CPlayer>();

    // 플레이어 속성 설정
    if (player) {
        if (player->GetPtype() == PlayerType::P1)
            player->SetPos(150, 675);
        else
            player->SetPos(650, 675);

        Position pos = player->GetPos(); // GetPos 사용
        std::cout << "Player added: ID = " << static_cast<int>(player->GetPtype())
            << ", X = " << pos.x
            << ", Y = " << pos.y << std::endl;
    }
    else {
        std::cerr << "Error: Player creation failed!\n";
    }
}

void Stage1::Update()
{
    CPlayer* player = ObjectManager::GetInstance().GetPlayer(); // 플레이어 객체 가져오기
    if (!player)
        return;

    Position pos = player->GetPos(); // GetPos 사용
    int x = pos.x;
    int y = pos.y;

    if ((currentMapID == 1 && x < 0) || (currentMapID == 0 && x >= 800)) {
        TransitionToNextMap(); // 경계를 넘으면 다음 맵으로 전환
    }
    else if ((currentMapID == 0 && x < 0)) {
        player->SetPos(0, y); // SetPos 사용
    }
    else if ((currentMapID == 1 && x >= 800)) {
        player->SetPos(800, y); // SetPos 사용
    }

    // Update all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects) {
        obj->Update();
    }
}

void Stage1::TransitionToNextMap()
{
    // 플레이어를 맵의 적절한 위치로 이동
    CPlayer* player = ObjectManager::GetInstance().GetPlayer();
    Position pos = player->GetPos();
    if (player)
    {
        int x = (currentMapID == 1) ? 800 : 0;
        player->SetPos(x, pos.y);
    }
    if (currentMapID == 0)
        currentMapID = 1;
    else if (currentMapID == 1)
        currentMapID = 0;
    else
        return;
}

void Stage1::InitializeMap() {
    // 1-1 맵 데이터 설정
    int tempMap1[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
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
