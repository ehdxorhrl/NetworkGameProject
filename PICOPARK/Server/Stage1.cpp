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

    // 플레이어 추가
    ObjectManager& manager = ObjectManager::GetInstance();
    CPlayer* player1 = manager.AddPlayer(PlayerType::P1);
    player1->SetPos(150, 675); // P1 초기 위치 설정
    player1->SetStageNum(0);

    //CPlayer* player2 = manager.AddPlayer(PlayerType::P2);
    //player2->SetPos(650, 675); // P2 초기 위치 설정

    std::cout << "Players initialized:\n";
    for (const auto* obj : manager.GetObjects()) {
        if (const auto* player = dynamic_cast<const CPlayer*>(obj)) {
            std::cout << "  Player ID: " << player->GetPK().m_playerID
                << ", Type: " << static_cast<int>(player->GetPtype())
                << ", X: " << player->GetPos().x
                << ", Y: " << player->GetPos().y << "\n";
        }
    }

    currentMapID = 0;
}


void Stage1::Update()
{
    CPlayer* player = ObjectManager::GetInstance().GetPlayer(); // 플레이어 객체 가져오기

    int x, y;
    if (player) {
        Position pos = player->GetPos(); // GetPos 사용
        x = pos.x;
        y = pos.y;

        if ((currentMapID == 1 && x < 0) || (currentMapID == 0 && x >= 800)) {
            TransitionToNextMap(); // 경계를 넘으면 다음 맵으로 전환
        }
        else if ((currentMapID == 0 && x < 0)) {
            player->SetPos(0, y); // SetPos 사용
        }
        else if ((currentMapID == 1 && x >= 800)) {
            player->SetPos(800, y); // SetPos 사용
        }
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
        player->SetStageNum(0);
    }
    if (currentMapID == 0)
        currentMapID = 1;
    else if (currentMapID == 1)
        currentMapID = 0;
    player->SetStageNum(currentMapID);
}

void Stage1::InitializeMap() {
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
