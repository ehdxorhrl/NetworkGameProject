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
        PImage[0].Load(L"assets/Â÷·Ç1.jpg");
        PImage[1].Load(L"assets/ÀÌµ¿1.jpg");
        PImage[2].Load(L"assets/Á¡ÇÁ1.jpg");
        PImage[3].Load(L"assets/Á×À½1.png");
    }
    else {
        PImage[0].Load(L"assets/Â÷·Ç2.jpg");
        PImage[1].Load(L"assets/ÀÌµ¿2.jpg");
        PImage[2].Load(L"assets/Á¡ÇÁ2.jpg");
        PImage[3].Load(L"assets/Á×À½2.png");
    }

    size = 50;
    speed = 350;
    stageNum = 0;
}

void CPlayer::Update() {
    if (info.m_player.m_stageNum < 10) {
        pos.x = info.m_player.m_x;
        pos.y = info.m_player.m_y;
        Pstate = info.m_player.m_state;
        stageNum = info.m_player.m_stageNum;
    }
}


void CPlayer::UpdateAnimation(float deltaTime) {
    animationTimer += deltaTime;

    if (animationTimer >= animationInterval) {
        Movement = (Movement + 1) % 2;
        animationTimer = 0.0f;
    }
}
