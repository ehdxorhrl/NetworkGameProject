#pragma once
#include "CObject.h"

const float animationInterval = 0.2;

enum class PlayerType
{
	None,
	P1, // 현재 플레이어
	P2 // 다른 네트워크 플레이어
};

class CPlayer :public CObject
{
public:
	CPlayer();
	CPlayer(PlayerType playerType) : Ptype(playerType) {
		Init();
	}
	~CPlayer() override;

	void Init() override;
	void Update() override;
public:
	void UpdateAnimation(float deltaTime);
	PlayerType GetPtype() { return Ptype; }
	uint32_t GetID() { return playerID; }
	bool CheckHorizontalCollision(int direction);
	RECT GetBoundingBox() const;
	bool CheckCollisionWithBlocks(const RECT& area);
	bool CheckVerticalCollision();
	void SetPosition(float x, float y) {
		pos.x = x;
		pos.y = y;
	}

	PlayerState GetPstate() const{
		return Pstate;
	}
	PlayerInfo GetPK() const {
		PlayerInfo myinfo;
		myinfo.m_playerID = playerID;
		myinfo.m_size = size;
		myinfo.m_state = Pstate;
		myinfo.m_x = pos.x;
		myinfo.m_y = pos.y;

		return myinfo;
	}
	PlayerType GetPtype() const { return Ptype; }
	void SetID(uint32_t _id) {
		playerID = _id;
	}

	void SetInput(KT _input, uint64_t _time) {
		input = _input;
		remainingInputTime += _time;
	};
private:
	PlayerType Ptype = PlayerType::None;
	PlayerState Pstate = PlayerState::Idle;
	float animationTimer = 0.0f;
	float jumpVelocity = 0.0f;
	float gravity = 980.0f;
	float jumpStrength = 450.0f;
	bool isJumping = false;
	bool isOnGround = false;
	int Movement = 0;
	int dir = 0;
	uint32_t playerID;
	KT input;
	uint64_t remainingInputTime = 0;
};
