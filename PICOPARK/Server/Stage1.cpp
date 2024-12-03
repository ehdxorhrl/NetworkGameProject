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

    // �÷��̾� �߰�
    ObjectManager& manager = ObjectManager::GetInstance();
    CPlayer* player1 = manager.AddPlayer(PlayerType::P1);
    player1->SetPos(150, 675); // P1 �ʱ� ��ġ ����
    player1->SetStageNum(0);

    //CPlayer* player2 = manager.AddPlayer(PlayerType::P2);
    //player2->SetPos(650, 675); // P2 �ʱ� ��ġ ����

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
    CPlayer* player = ObjectManager::GetInstance().GetPlayer(); // �÷��̾� ��ü ��������

    int x, y;
    if (player) {
        Position pos = player->GetPos(); // GetPos ���
        x = pos.x;
        y = pos.y;

        if ((currentMapID == 1 && x < 0) || (currentMapID == 0 && x >= 800)) {
            TransitionToNextMap(); // ��踦 ������ ���� ������ ��ȯ
        }
        else if ((currentMapID == 0 && x < 0)) {
            player->SetPos(0, y); // SetPos ���
        }
        else if ((currentMapID == 1 && x >= 800)) {
            player->SetPos(800, y); // SetPos ���
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
    // �÷��̾ ���� ������ ��ġ�� �̵�
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
    // 1-1 �� ������ ����
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

    // 1-2 �� ������ ����
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

    // �� ������ ����
    memcpy(maps[0], tempMap1, sizeof(tempMap1));
    memcpy(maps[1], tempMap2, sizeof(tempMap2));

    currentMapID = 0; // �ʱ� �� ����
}
