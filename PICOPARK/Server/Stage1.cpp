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

    // �÷��̾� ����
    CPlayer* player = ObjectManager::GetInstance().CreateObject<CPlayer>();

    // �÷��̾� �Ӽ� ����
    if (player) {
        if (player->GetPtype() == PlayerType::P1)
            player->SetPos(150, 675);
        else
            player->SetPos(650, 675);

        Position pos = player->GetPos(); // GetPos ���
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
    CPlayer* player = ObjectManager::GetInstance().GetPlayer(); // �÷��̾� ��ü ��������
    if (!player)
        return;

    Position pos = player->GetPos(); // GetPos ���
    int x = pos.x;
    int y = pos.y;

    if ((currentMapID == 1 && x < 0) || (currentMapID == 0 && x >= 800)) {
        TransitionToNextMap(); // ��踦 ������ ���� ������ ��ȯ
    }
    else if ((currentMapID == 0 && x < 0)) {
        player->SetPos(0, y); // SetPos ���
    }
    else if ((currentMapID == 1 && x >= 800)) {
        player->SetPos(800, y); // SetPos ���
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
    }
    if (currentMapID == 0)
        currentMapID = 1;
    else if (currentMapID == 1)
        currentMapID = 0;
    else
        return;
}

void Stage1::InitializeMap() {
    // 1-1 �� ������ ����
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
