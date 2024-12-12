#include "stdafx.h"
#include "Stage2.h"

Stage2::Stage2()
{
}

Stage2::~Stage2()
{
    ObjectManager::GetInstance().Clear();
}
void Stage2::Init()
{

    ObjectManager::GetInstance().Clear();

    InitializeMap();

    // 플레이어 추가
    ObjectManager& manager = ObjectManager::GetInstance();
    CPlayer* player1 = manager.AddPlayer(PlayerType::P1);
    player1->SetPos(50, 675);
    player1->SetStageNum(0);
    player1->SetID(static_cast<int>(player1->GetPtype()));

    CPlayer* player2 = manager.AddPlayer(PlayerType::P2);
    player2->SetPos(150, 675);
    player2->SetStageNum(0);
    player2->SetID(static_cast<int>(player2->GetPtype()));

}


void Stage2::Update() {
    bool clearstage = true;

    CPlayer* players[2] = {
       ObjectManager::GetInstance().GetPlayerByID(0),
       ObjectManager::GetInstance().GetPlayerByID(1)
    };

    for (int i = 0; i < 2; i++) {
        CPlayer* player = players[i]; // 플레이어 객체 가져오기

        if (!player) continue;

        Position pos = player->GetPos();
        RECT playerBox = player->GetBoundingBox(); // 플레이어 바운딩 박스 가져오기

        // 충돌 검사: 플레이어 바운딩 박스가 맵 경계 내에 있는지 확인
        for (int y = playerBox.top / BLOCK_SIZE; y <= playerBox.bottom / BLOCK_SIZE; ++y) {
            for (int x = playerBox.left / BLOCK_SIZE; x <= playerBox.right / BLOCK_SIZE; ++x) {
                if (x < 0 || x >= 8 || y < 0 || y >= 8) continue; // 맵 경계 초과 시 스킵

                int currentBlock = maps[player->GetStageNum()][y][x];

                // 열쇠(`2`)를 먹었는지 확인
                if (currentBlock == 3) {
                    maps[1][y][x] = 0; // 열쇠를 제거
                    player->Sethaskey(true);

                    // `5` 값의 블록 활성화
                    for (int row = 0; row < 8; ++row) {
                        for (int col = 0; col < 8; ++col) {
                            if (maps[2][row][col] == 1) {
                                maps[2][row][col] = 6; // 블록 활성화
                            }
                        }
                    }
                }
                // 열쇠를 가진 상태로 문(`4`)에 도달했는지 확인
                else if (currentBlock == 4 && player->Gethaskey()) {
                    maps[2][y][x] = 5; // 문을 열고 열쇠를 제거
                    openDoor = true;
                    player->Sethaskey(false);
                }
                // 플레이어가 열린 문(`5`)를 통해 이동하려는 경우
                else if (currentBlock == 5 && player->GetKT() == KT::Up) {
                    player->SetPstate(PlayerState::intothedoor);            
                }
                // 스위치(`7`)에 점프했는지 확인
                else if (currentBlock == 7 && player->GetPstate() == PlayerState::Jump && player->GetjumpVelocity()>0) {
                    player->SetPosition(650, 175);
                }
                else if(player->GetjumpVelocity() == 0){
                    player->SetSwitchTriggered(false); // 스위치 상태 초기화
                }
            }
        }

        // 맵 경계 밖으로 나가면 맵 전환 처리
        if ((player->GetStageNum() == 1 && pos.x < 0) || (player->GetStageNum() == 0 && pos.x >= 800) || (player->GetStageNum() == 1 && pos.x >= 800) || (player->GetStageNum() == 2 && pos.x < 0)) {
            TransitionToNextMap(player); // 맵 전환
        }
        else if (player->GetStageNum() == 0 && pos.x < 0) {
            player->SetPos(0, pos.y); // 맵 왼쪽 경계로 이동
        }
        else if (player->GetStageNum() == 2 && pos.x >= 800) {
            player->SetPos(800, pos.y); // 맵 오른쪽 경계로 이동
        }

        if (player->GetPstate() != PlayerState::intothedoor) {
            clearstage = false;
        }
    }

    if (clearstage) {
        shouldChangeScene = true; // 스테이지 클리어 처리
    }

    // 모든 객체 업데이트
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects) {
        obj->Update();
    }
}

void Stage2::TransitionToNextMap(CPlayer* player) {
    if (!player) return;

    Position pos = player->GetPos();

    // 다음 맵으로 전환
    if (player->GetStageNum() == 0) {
        player->SetPos(0, pos.y); // 다음 맵의 시작 위치로 이동
        player->SetStageNum(1);   // 0 -> 1
    }
    else if (player->GetStageNum() == 1 && pos.x >= 800) {
        player->SetPos(0, pos.y); // 다음 맵의 시작 위치로 이동
        player->SetStageNum(2);   // 1 -> 2
    }
    else if (player->GetStageNum() == 1 && pos.x < 0) {
        player->SetPos(800, pos.y); // 이전 맵의 끝 위치로 이동
        player->SetStageNum(0);     // 1 -> 0
    }
    else if (player->GetStageNum() == 2) {
        player->SetPos(800, pos.y); // 이전 맵의 끝 위치로 이동
        player->SetStageNum(1);     // 2 -> 1
    }
}

void Stage2::InitializeMap() { // 일반블럭 2, 열쇠 3, 문 4 or 5, 투명블록 1 or 6, 빨간 스위치 7 or 8, 파란 스위치 9, 10
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


bool Stage2::CheckCollisionWithBlock(CPlayer* player, int blockType) {
    RECT playerBox = player->GetBoundingBox();

    for (int y = playerBox.top / BLOCK_SIZE; y <= playerBox.bottom / BLOCK_SIZE; ++y) {
        for (int x = playerBox.left / BLOCK_SIZE; x <= playerBox.right / BLOCK_SIZE; ++x) {
            if (x < 0 || x >= 8 || y < 0 || y >= 8) continue; // 맵 범위 체크

            if (maps[player->GetStageNum()][y][x] == blockType) {
                return true; // 충돌 발생
            }
        }
    }
    return false; // 충돌 없음
}