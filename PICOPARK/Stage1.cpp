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
    BlockImage.Load(L"assets/��1.png");

    InitializeMap();

    // Use ObjectManager to create and manage the player
    CPlayer* player = ObjectManager::GetInstance().CreateObject<CPlayer>();

    if (player->GetPtype() == PlayerType::P1)
        player->SetPos(150, 675);
    else
        player->SetPos(650, 675);
}

void Stage1::Update()
{
    CPlayer* player = ObjectManager::GetInstance().GetPlayer(); // �÷��̾� ��ü ��������
    if (!player)
        return;

    Position pos = player->GetPos();
    int x = pos.x;
    int y = pos.y;

    if ((currentMapID==1 && x < 0) || (currentMapID == 0 && x >= 800)) {
        TransitionToNextMap(); // ��踦 ������ ���� ������ ��ȯ
    }
    else if((currentMapID == 0 && x < 0))
    {
        player->SetPos(0, y);
    }
    else if ((currentMapID == 1 && x >= 800))
    {
        player->SetPos(800, y);
    }

    // Update all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects)
    {
        obj->Update();
    }
}

void Stage1::Render(HDC hdc)
{
    for(int i=0; i<8; i++)
        for (int j = 0; j < 8; j++)
        {
            if(maps[currentMapID][i][j]==1)
                BlockImage.Draw(hdc, j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, 0, 0, 100, 100);
        }

    // Render all objects managed by ObjectManager
    const auto& objects = ObjectManager::GetInstance().GetObjects();
    for (auto* obj : objects)
    {
        obj->Render(hdc);
    }
}

void Stage1::TransitionToNextMap()
{
    // �÷��̾ ���� ������ ��ġ�� �̵�
    CPlayer* player = ObjectManager::GetInstance().GetPlayer();
    Position pos = player->GetPos();
    if (player)
    {
        int x = (currentMapID==1) ? 800 : 0;
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
