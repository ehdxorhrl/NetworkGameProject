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
    ObjectManager::GetInstance().Clear();

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

}


void Stage1::Update()
{
    CPlayer* players[2] = {
        ObjectManager::GetInstance().GetPlayerByID(0),
        ObjectManager::GetInstance().GetPlayerByID(1)
    };

    if (players[0]->GetKT() == KT::R || players[1]->GetKT() == KT::R) {
        Init();
        return;
    }

    bool clearstage = true;
    for (int i = 0; i < 2; i++) {
        CPlayer* player = ObjectManager::GetInstance().GetPlayerByID(i); // 플레이어 객체 가져오기

        if (player) {
            Position pos = player->GetPos();
            int x = static_cast<int>(pos.x / BLOCK_SIZE);
            int y = static_cast<int>(pos.y / BLOCK_SIZE);

            // 열쇠(`2`)를 먹었는지 확인
            if (!(x < 0 || x>=8 || y < 0 || y>=8)) {
                if (maps[player->GetStageNum()][y][x] == 3) {
                    maps[player->GetStageNum()][y][x] = 0; // 열쇠를 제거
                    player->Sethaskey(true);

                    // `5` 값의 블록 활성화
                    for (int row = 0; row < 8; ++row) {
                        for (int col = 0; col < 8; ++col) {
                            if (maps[player->GetStageNum()][row][col] == 1) {
                                maps[player->GetStageNum()][row][col] = 6; // 블록 활성화
                            }
                        }
                    }
                }
                if (maps[player->GetStageNum()][y][x] == 4 && player->Gethaskey()) {
                    maps[player->GetStageNum()][y][x] = 5; // 열쇠를 제거  
                    openDoor = true;
                    player->Sethaskey(false);
                }

                if (maps[player->GetStageNum()][y][x] == 5 && player->GetKT() == KT::Up) {
                    player->SetPstate(PlayerState::intothedoor);
                }
            }
      
            if ((player->GetStageNum() == 1 && pos.x < 0) || (player->GetStageNum() == 0 && pos.x >= 800)) {
                TransitionToNextMap(player); // 맵 전환
            }
            else if (player->GetStageNum() == 0 && pos.x < 0) {
                player->SetPos(0, pos.y);
            }
            else if (player->GetStageNum() == 1 && pos.x >= 800) {
                player->SetPos(800, pos.y);
            }

            if (player->GetPstate() != PlayerState::intothedoor)
                clearstage = false;
        }
    }

    if (clearstage)
        shouldChangeScene = true;
    
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
        {0, 0, 0, 0, 0, 0, 0, 4},
        {0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 1, 0, 0, 0, 0},
        {3, 0, 1, 0, 0, 0, 0, 0},
        {2, 2, 2, 0, 2, 0, 2, 2},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2}
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
    memcpy(maps[0], tempMap1, sizeof(tempMap1));
    memcpy(maps[1], tempMap2, sizeof(tempMap2));

}
