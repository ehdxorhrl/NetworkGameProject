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
		PImage[0].Load(L"assets/����1.jpg"); // ���� �̹���
		PImage[1].Load(L"assets/�̵�1.jpg"); // �̵� �̹���
		PImage[2].Load(L"assets/����1.jpg"); // ���� �̹���
		PImage[3].Load(L"assets/����1.png"); // ���� �̹���
	}
	else {
		PImage[0].Load(L"assets/����2.jpg"); // ���� �̹���
		PImage[1].Load(L"assets/�̵�2.jpg"); // �̵� �̹���
		PImage[2].Load(L"assets/����2.jpg"); // ���� �̹���
		PImage[3].Load(L"assets/����2.png"); // ���� �̹���
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
			// ���� ����
			isJumping = true;
			jumpVelocity = -jumpStrength; // ���� ��� (���� ����)
			Pstate = PlayerState::Jump;
			return;
		}

		// �¿� �̵� ó��
		if (input.IsKeyDown(VK_LEFT)) {
			dir = -1;
		}
		else if (input.IsKeyDown(VK_RIGHT)) {
			dir = 1;
		}

		// �̵� ���·� ��ȯ
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
		jumpVelocity += gravity * deltaTime; // �߷¿� ���� �ӵ� ���� (�ϰ�)

		// ���� �����ߴ��� Ȯ��
		if (y >= groundY) {
			y = groundY; // ���� ��ġ ����
			isJumping = false;
			Pstate = PlayerState::Idle; // ������ ������ �ٽ� Idle ���·�
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
			// ���� ����
			isJumping = true;
			jumpVelocity = -jumpStrength; // ���� ��� (���� ����)
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
	// �ִϸ��̼� Ÿ�̸� ������Ʈ
	animationTimer += deltaTime;

	// 1�ʸ��� �ִϸ��̼� ������ ����
	if (animationTimer >= animationInterval) {
		Movement = (Movement + 1) % 2; // �ִϸ��̼� ������ ��ȯ (0�� 1 ���� �ݺ�)
		animationTimer = 0.0f; // Ÿ�̸� �ʱ�ȭ
	}
}

