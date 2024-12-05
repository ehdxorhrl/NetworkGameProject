#include "stdafx.h"
#include "CPlayer.h"
#include "Stage1.h"

CPlayer::CPlayer() {}

CPlayer::~CPlayer() {}

void CPlayer::Render(HDC hdc) {
    float drawX = pos.x - size / 2;
    float drawY = pos.y - size / 2;

    if (Pstate == PlayerState::Idle)
        PImage[0].Draw(hdc, drawX, drawY, size, size, 0, 0, 100, 100);
    else if (Pstate == PlayerState::Jump)
        PImage[2].Draw(hdc, drawX, drawY, size, size, 0, 0, 100, 100);
    else if (Pstate == PlayerState::Die)
        PImage[3].Draw(hdc, drawX, drawY, size, size, 0, 0, 100, 100);
    else if (Pstate == PlayerState::Move) {
        if (Movement)
            PImage[1].Draw(hdc, drawX, drawY, size, size, 0, 0, 100, 100);
        else
            PImage[0].Draw(hdc, drawX, drawY, size, size, 0, 0, 100, 100);
    }
}

void CPlayer::Init() {
    if (Ptype == PlayerType::P1) {
        PImage[0].Load(L"assets/차렷1.jpg");
        PImage[1].Load(L"assets/이동1.jpg");
        PImage[2].Load(L"assets/점프1.jpg");
        PImage[3].Load(L"assets/죽음1.png");
    }
    else {
        PImage[0].Load(L"assets/차렷2.jpg");
        PImage[1].Load(L"assets/이동2.jpg");
        PImage[2].Load(L"assets/점프2.jpg");
        PImage[3].Load(L"assets/죽음2.png");
    }

    size = 50;
    speed = 350;
    stageNum = 0;
}

void CPlayer::Update() {
    
}


void CPlayer::UpdateAnimation(float deltaTime) {
    animationTimer += deltaTime;

    if (animationTimer >= animationInterval) {
        Movement = (Movement + 1) % 2;
        animationTimer = 0.0f;
    }
}

void CPlayer::Setinfo(ObjectInfo_Packet* _info) {
    pos.x = _info->m_player[playerID].m_x;
    pos.y = _info->m_player[playerID].m_y;
    Pstate = _info->m_player[playerID].m_state;
    stageNum = _info->m_player[playerID].m_stageNum;

    // 패킷 포인터의 메모리 해제
}
