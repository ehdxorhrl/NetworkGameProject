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
        int playerType = inputPacket->m_playerID;

        // ObjectManager에서 플레이어 가져오기
        CPlayer* player = ObjectManager::GetInstance().GetPlayerByID(playerType);
        if (player) {
            player->SetInput(inputPacket->inputType, inputPacket->inputTime);
            Position pos = player->GetPos();

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
