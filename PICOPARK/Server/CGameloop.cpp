#include "stdafx.h"
#include "CGameloop.h"

CGameloop::CGameloop() {}

CGameloop::~CGameloop() {}

void CGameloop::Init() {
    timeManager.Init();
    SceneManager.Init();
}

void CGameloop::Update(const Input_Packet* inputPacket) {

    CScene* currentScene = SceneManager.GetInstance().GetCurrentScene();
    if (currentScene && inputPacket) {
        uint32_t playerType = inputPacket->m_playerID;

        // ObjectManager에서 플레이어 가져오기
        CPlayer* player = nullptr;
        const auto& objects = ObjectManager::GetInstance().GetObjects();
        for (auto* obj : objects) {
            if (CPlayer* p = dynamic_cast<CPlayer*>(obj)) {
                if(playerType == p->GetID())
                player = p;
                break;
            }
        }
        std::cerr << inputPacket->m_playerID << "\n";
        // player가 nullptr인지 확인
        if (player) {
            player->SetInput(inputPacket->inputType, inputPacket->inputTime);
            Position pos = player->GetPos(); // GetPos 사용
            std::cout << "Updated Player: ID = " << static_cast<int>(player->GetPtype())
                << ", X = " << pos.x
                << ", Y = " << pos.y << std::endl;
        }
        else {
            std::cerr << "Error: Player with ID " << inputPacket->m_playerID << " not found.\n";
        }
    }

    timeManager.Update();
    SceneManager.Update();
}

void CGameloop::Update()
{
    timeManager.Update();
    SceneManager.Update();
}
