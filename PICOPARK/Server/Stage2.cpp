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


void Stage2::Update() {
    bool clearstage = true;

    CPlayer* players[2] = {
       ObjectManager::GetInstance().GetPlayerByID(0),
       ObjectManager::GetInstance().GetPlayerByID(1)
    };

    for (int i = 0; i < 2; i++) {
        CPlayer* player = players[i]; // �÷��̾� ��ü ��������

        if (!player) continue;

        Position pos = player->GetPos();
        RECT playerBox = player->GetBoundingBox(); // �÷��̾� �ٿ�� �ڽ� ��������

        // �浹 �˻�: �÷��̾� �ٿ�� �ڽ��� �� ��� ���� �ִ��� Ȯ��
        for (int y = playerBox.top / BLOCK_SIZE; y <= playerBox.bottom / BLOCK_SIZE; ++y) {
            for (int x = playerBox.left / BLOCK_SIZE; x <= playerBox.right / BLOCK_SIZE; ++x) {
                if (x < 0 || x >= 8 || y < 0 || y >= 8) continue; // �� ��� �ʰ� �� ��ŵ

                int currentBlock = maps[player->GetStageNum()][y][x];

                // ����(`2`)�� �Ծ����� Ȯ��
                if (currentBlock == 3) {
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
                // ���踦 ���� ���·� ��(`4`)�� �����ߴ��� Ȯ��
                else if (currentBlock == 4 && player->Gethaskey()) {
                    maps[2][y][x] = 5; // ���� ���� ���踦 ����
                    openDoor = true;
                    player->Sethaskey(false);
                }
                // �÷��̾ ���� ��(`5`)�� ���� �̵��Ϸ��� ���
                else if (currentBlock == 5 && player->GetKT() == KT::Up) {
                    player->SetPstate(PlayerState::intothedoor);            
                }
                // ����ġ(`7`)�� �����ߴ��� Ȯ��
                else if (currentBlock == 7 && player->GetPstate() == PlayerState::Jump && player->GetjumpVelocity()>0) {
                    player->SetPosition(650, 175);
                }
                else if(player->GetjumpVelocity() == 0){
                    player->SetSwitchTriggered(false); // ����ġ ���� �ʱ�ȭ
                }
            }
        }

        // �� ��� ������ ������ �� ��ȯ ó��
        if ((player->GetStageNum() == 1 && pos.x < 0) || (player->GetStageNum() == 0 && pos.x >= 800) || (player->GetStageNum() == 1 && pos.x >= 800) || (player->GetStageNum() == 2 && pos.x < 0)) {
            TransitionToNextMap(player); // �� ��ȯ
        }
        else if (player->GetStageNum() == 0 && pos.x < 0) {
            player->SetPos(0, pos.y); // �� ���� ���� �̵�
        }
        else if (player->GetStageNum() == 2 && pos.x >= 800) {
            player->SetPos(800, pos.y); // �� ������ ���� �̵�
        }

        if (player->GetPstate() != PlayerState::intothedoor) {
            clearstage = false;
        }
    }

    if (clearstage) {
        shouldChangeScene = true; // �������� Ŭ���� ó��
    }

    // ��� ��ü ������Ʈ
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects) {
        obj->Update();
    }
}

void Stage2::TransitionToNextMap(CPlayer* player) {
    if (!player) return;

    Position pos = player->GetPos();

    // ���� ������ ��ȯ
    if (player->GetStageNum() == 0) {
        player->SetPos(0, pos.y); // ���� ���� ���� ��ġ�� �̵�
        player->SetStageNum(1);   // 0 -> 1
    }
    else if (player->GetStageNum() == 1 && pos.x >= 800) {
        player->SetPos(0, pos.y); // ���� ���� ���� ��ġ�� �̵�
        player->SetStageNum(2);   // 1 -> 2
    }
    else if (player->GetStageNum() == 1 && pos.x < 0) {
        player->SetPos(800, pos.y); // ���� ���� �� ��ġ�� �̵�
        player->SetStageNum(0);     // 1 -> 0
    }
    else if (player->GetStageNum() == 2) {
        player->SetPos(800, pos.y); // ���� ���� �� ��ġ�� �̵�
        player->SetStageNum(1);     // 2 -> 1
    }
}

void Stage2::InitializeMap() { // �Ϲݺ� 2, ���� 3, �� 4 or 5, ������ 1 or 6, ���� ����ġ 7 or 8, �Ķ� ����ġ 9, 10
    // 1-1 �� ������ ����
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

    // 1-2 �� ������ ����
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

    // �� ������ ����
    memcpy(maps[0], tempMap1, sizeof(tempMap1));
    memcpy(maps[1], tempMap2, sizeof(tempMap2));
    memcpy(maps[2], tempMap3, sizeof(tempMap3));
}


bool Stage2::CheckCollisionWithBlock(CPlayer* player, int blockType) {
    RECT playerBox = player->GetBoundingBox();

    for (int y = playerBox.top / BLOCK_SIZE; y <= playerBox.bottom / BLOCK_SIZE; ++y) {
        for (int x = playerBox.left / BLOCK_SIZE; x <= playerBox.right / BLOCK_SIZE; ++x) {
            if (x < 0 || x >= 8 || y < 0 || y >= 8) continue; // �� ���� üũ

            if (maps[player->GetStageNum()][y][x] == blockType) {
                return true; // �浹 �߻�
            }
        }
    }
    return false; // �浹 ����
}