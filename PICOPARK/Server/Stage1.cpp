#include "stdafx.h"
#include "Stage1.h"

Stage1::Stage1()
{
}

Stage1::~Stage1()
{
    ObjectManager::GetInstance().Clear();
}
void Stage1::Init()
{
    InitializeMap();

    // 플레이어 추가
    ObjectManager& manager = ObjectManager::GetInstance();
    CPlayer* player1 = manager.AddPlayer(PlayerType::P1);
    player1->SetPos(150, 675);
    player1->SetStageNum(0);
    player1->SetID(static_cast<int>(player1->GetPtype()));

    CPlayer* player2 = manager.AddPlayer(PlayerType::P2);
    player2->SetPos(650, 675);
    player2->SetStageNum(0);
    player2->SetID(static_cast<int>(player2->GetPtype()));

    std::cout << "Players initialized:\n";
    for (const auto* obj : manager.GetObjects()) {
        if (const auto* player = dynamic_cast<const CPlayer*>(obj)) {
            //std::cout << "  Player ID: " << player->GetPK().m_playerID
            //    << ", Type: " << static_cast<int>(player->GetPtype())
            //    << ", X: " << player->GetPos().x
            //    << ", Y: " << player->GetPos().y << "\n"
            //    << ", ID: " << player->GetID() << "\n";
        }
    }

    currentMapID = 0;
}


void Stage1::Update()
{
    for (int i = 0; i < 2; i++) {
        CPlayer* player = ObjectManager::GetInstance().GetPlayerByID(i); // 플레이어 객체 가져오기

        if (player) {
            Position pos = player->GetPos();
            int x = pos.x;
            int y = pos.y;

            if ((player->GetStageNum() == 1 && x < 0) || (player->GetStageNum() == 0 && x >= 800)) {
                TransitionToNextMap(player); // 플레이어 ID 기반으로 맵 전환
            }
            else if (player->GetStageNum() == 0 && x < 0) {
                player->SetPos(0, y);
            }
            else if (player->GetStageNum() == 1 && x >= 800) {
                player->SetPos(800, y);
            }
        }
    }
    
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects) {
        obj->Update();
    }
}

void Stage1::TransitionToNextMap(CPlayer* player) {
    if (!player) return;

    Position pos = player->GetPos();
    int x = (player->GetStageNum() == 1) ? 800 : 0;
    player->SetPos(x, pos.y);

    // 플레이어의 stageNum을 반대 맵으로 설정
    player->SetStageNum(player->GetStageNum() == 0 ? 1 : 0);
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
