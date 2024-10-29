#pragma once
#include "CObject.h"

const float animationInterval = 0.2;

enum class PlayerType
{
	None,
	P1, // 현재 플레이어
	P2 // 다른 네트워크 플레이어
};

enum class PlayerState
{
	Idle,
	Move,
	Jump,
	MoveAndJump,
	Die
};

class CPlayer :public CObject
{
public:
	CPlayer();
	CPlayer(PlayerType playerType) : Ptype(playerType) {
		Init();
	}
	~CPlayer() override;

	void Render(HDC hdc) override;
	void Init() override;
	void Update() override;
public:
	void UpdateAnimation(float deltaTime);
private:
	CImage PImage[5];
	PlayerType Ptype = PlayerType::None;
	PlayerState Pstate = PlayerState::Idle;
	float animationTimer = 0.0f;
	float jumpVelocity = 0.0f;
	float gravity = 980.0f;
	float jumpStrength = 450.0f;
	float groundY = 400.0f;
	bool isJumping = false;
	int Movement = 0;
	int dir = 0;
};
