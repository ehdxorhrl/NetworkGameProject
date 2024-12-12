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
    if (haskey) {
        KeyImage.Draw(hdc, drawX-20, drawY-20, 20, 20, 0, 0, 100, 100);
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

    KeyImage.Load(L"assets/¿­¼è.png");

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

void CPlayer::Setinfo(PlayerInfo* _info) {
    pos.x = _info->m_x;
    pos.y = _info->m_y;
    Pstate = _info->m_state;
    stageNum = _info->m_stageNum;
    Movement = _info->m_Movement;
    haskey = _info->m_haskey;
    size = _info->m_size;
}
