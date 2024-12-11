#include "stdafx.h"
#include "Stage3.h"

Stage3::Stage3()
{
}

Stage3::~Stage3()
{
    ObjectManager::GetInstance().Clear();
}
void Stage3::Init()
{
    InitializeMap();

    // �÷��̾� �߰�
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


void Stage3::Update()
{
    bool clearstage = true;
    for (int i = 0; i < 2; i++) {
        CPlayer* player = ObjectManager::GetInstance().GetPlayerByID(i); // �÷��̾� ��ü ��������

        if (player) {
            Position pos = player->GetPos();
            int x = static_cast<int>(pos.x / BLOCK_SIZE);
            int y = static_cast<int>(pos.y / BLOCK_SIZE);

            // ����(`2`)�� �Ծ����� Ȯ��
            if (!(x < 0 || x >= 8 || y < 0 || y >= 8)) {
                if (maps[1][y][x] == 3) {
                    maps[1][y][x] = 0; // ���踦 ����
                    player->Sethaskey(true);

                    // `5` ���� ��� Ȱ��ȭ
                    for (int row = 0; row < 8; ++row) {
                        for (int col = 0; col < 8; ++col) {
                            if (maps[2][row][col] == 1) {
                                maps[2][row][col] = 6; // ��� Ȱ��ȭ
                            }
                        }
                    }
                }
                if (maps[player->GetStageNum()][y][x] == 4 && player->Gethaskey()) {
                    maps[2][y][x] = 5; // ���踦 ����      
                    player->Sethaskey(false);
                }

                if (maps[player->GetStageNum()][y][x] == 5 && player->GetKT() == KT::Up) {
                    player->SetPstate(PlayerState::intothedoor);
                }
            }

            if ((player->GetStageNum() == 1 && pos.x < 0) || (player->GetStageNum() == 0 && pos.x >= 800)) {
                TransitionToNextMap(player); // �� ��ȯ
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

void Stage3::TransitionToNextMap(CPlayer* player) {
    if (!player) return;

    Position pos = player->GetPos();
    int x = (player->GetStageNum() == 1) ? 800 : 0;
    player->SetPos(x, pos.y);

    // �÷��̾��� stageNum�� �ݴ� ������ ����
    player->SetStageNum(player->GetStageNum() == 0 ? 1 : 0);
}

void Stage3::InitializeMap() {
    // 1-1 �� ������ ����
    int tempMap1[8][8] = {
        {0, 0, 0, 0, 0, 0, 2, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 2, 2},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 6, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

    // 1-2 �� ������ ����
    int tempMap2[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 2},
        {0, 0, 0, 0, 0, 3, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {0, 0, 0, 0, 0, 0, 0, 2},
        {0, 0, 0, 0, 0, 0, 0, 2},
        {0, 0, 0, 0, 0, 0, 0, 2},
        {0, 0, 0, 4, 0, 0, 0, 0},
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

    // �� ������ ����
    memcpy(maps[0], tempMap1, sizeof(tempMap1));
    memcpy(maps[1], tempMap2, sizeof(tempMap2));
    memcpy(maps[2], tempMap3, sizeof(tempMap3));
}
