#include "stdafx.h"
#include "CPlayer.h"

CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Render(HDC hdc)
{
    if(Pstate == PlayerState::Idle)
        PImage[0].Draw(hdc, x-size/2, y - size/2, size, size, 0, 0, 100, 100);
	else if(Pstate == PlayerState::Jump)
		PImage[2].Draw(hdc, x - size / 2, y - size / 2, size, size, 0, 0, 100, 100);
	else if(Pstate == PlayerState::Die)
		PImage[3].Draw(hdc, x - size / 2, y - size / 2, size, size, 0, 0, 100, 100);
	else if (Pstate == PlayerState::Move)
	{
		if(Movement)
			PImage[1].Draw(hdc, x - size / 2, y - size / 2, size, size, 0, 0, 100, 100);
		else
			PImage[0].Draw(hdc, x - size / 2, y - size / 2, size, size, 0, 0, 100, 100);
	}
}

void CPlayer::Init()
{
	if (Ptype == PlayerType::P1) {
		PImage[0].Load(L"assets/차렷1.jpg"); // 차렷 이미지
		PImage[1].Load(L"assets/이동1.jpg"); // 이동 이미지
		PImage[2].Load(L"assets/점프1.jpg"); // 점프 이미지
		PImage[3].Load(L"assets/죽음1.png"); // 죽음 이미지
	}
	else {
		PImage[0].Load(L"assets/차렷2.jpg"); // 차렷 이미지
		PImage[1].Load(L"assets/이동2.jpg"); // 이동 이미지
		PImage[2].Load(L"assets/점프2.jpg"); // 점프 이미지
		PImage[3].Load(L"assets/죽음2.png"); // 죽음 이미지
	}

	x = 400, y = 400;
	size = 50;
	speed = 350;
}

void CPlayer::Update()
{
	InputManager& input = InputManager::GetInstance();
	float deltaTime = TimeManager::GetInstance().GetDeltaTime();

	// state_mechine
	switch (Pstate)
	{
	case PlayerState::Idle:
	{
		if (input.IsKeyDown(VK_UP) && !isJumping) {
			// 점프 시작
			isJumping = true;
			jumpVelocity = -jumpStrength; // 위로 상승 (음수 방향)
			Pstate = PlayerState::Jump;
			return;
		}

		// 좌우 이동 처리
		if (input.IsKeyDown(VK_LEFT)) {
			dir = -1;
		}
		else if (input.IsKeyDown(VK_RIGHT)) {
			dir = 1;
		}

		// 이동 상태로 전환
		bool isMoving = input.IsKeyDown(VK_LEFT) || input.IsKeyDown(VK_RIGHT);
		if (isMoving) {
			x += static_cast<float>(speed) * deltaTime * dir;
			Pstate = PlayerState::Move;
			Movement = (Movement + 1) % 2;
		}
		break;
	}
	case PlayerState::Jump:
		y += jumpVelocity * deltaTime;
		jumpVelocity += gravity * deltaTime; // 중력에 따라 속도 증가 (하강)

		// 땅에 도착했는지 확인
		if (y >= groundY) {
			y = groundY; // 땅에 위치 고정
			isJumping = false;
			Pstate = PlayerState::Idle; // 점프가 끝나면 다시 Idle 상태로
		}

		if (input.IsKeyDown(VK_LEFT)) {
			x -= static_cast<float>(speed) * deltaTime;
		}
		else if (input.IsKeyDown(VK_RIGHT)) {
			x += static_cast<float>(speed) * deltaTime;
		}

		break;
	case PlayerState::Move:
		if (input.IsKeyDown(VK_UP) && !isJumping) {
			// 점프 시작
			isJumping = true;
			jumpVelocity = -jumpStrength; // 위로 상승 (음수 방향)
			Pstate = PlayerState::Jump;
			return;
		}

		if (input.IsKeyDown(VK_LEFT) || input.IsKeyDown(VK_RIGHT)){
			dir = input.IsKeyDown(VK_LEFT) ? -1 : 1;
			x += static_cast<float>(speed) * deltaTime * dir;
			UpdateAnimation(deltaTime);
		}
		else {
			Pstate = PlayerState::Idle;
			Movement = 0;
		}
		break;
	}
}

void CPlayer::UpdateAnimation(float deltaTime) {
	// 애니메이션 타이머 업데이트
	animationTimer += deltaTime;

	// 1초마다 애니메이션 프레임 변경
	if (animationTimer >= animationInterval) {
		Movement = (Movement + 1) % 2; // 애니메이션 프레임 전환 (0과 1 사이 반복)
		animationTimer = 0.0f; // 타이머 초기화
	}
}

