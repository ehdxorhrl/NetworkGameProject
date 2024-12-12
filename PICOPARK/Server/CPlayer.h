#pragma once
#include "CObject.h"

const float animationInterval = 0.2;

enum class PlayerType
{
	P1,
	P2,
	None
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
	KT GetKT() { return input; }

	PlayerState GetPstate() const{
		return Pstate;
	}
	void SetPstate(PlayerState _Pstate) { Pstate = _Pstate; }
	PlayerInfo GetPK() const {
		PlayerInfo myinfo;
		myinfo.m_playerID = playerID;
		myinfo.m_size = size;
		myinfo.m_state = Pstate;
		myinfo.m_x = pos.x;
		myinfo.m_y = pos.y;
		myinfo.m_stageNum = stageNum;
		myinfo.m_Movement = Movement;
		myinfo.m_haskey = haskey;
		return myinfo;
	}
	int GetID() const { return playerID; }

	PlayerType GetPtype() const { return Ptype; }

	void SetID(int _id) {
		playerID = _id;
	}

	void SetInput(KT _input, uint64_t _time) {
		if (input != _input) {
			input = _input;
			remainingInputTime = _time;
		}
		else {
			remainingInputTime += _time;
		}
	}
	void SetStageNum(uint32_t _stagenum) {
		stageNum = _stagenum;
	}

	uint32_t GetStageNum() {
		return stageNum;
	}
	void Sethaskey(bool _flag) { haskey = _flag; }
	bool Gethaskey() {return haskey;}
	float GetjumpVelocity() {
		return jumpVelocity;
	}
	void SetjumpVelocity(int _num) {
		jumpVelocity = _num;
	}

	int Getsize() { return size; }

	void IncreaseSize() { size += 10; }
	bool IsSwitchTriggered() const { return switchTriggered; }
	void SetSwitchTriggered(bool triggered) { switchTriggered = triggered; }

	RECT GetBoundingBox() {
		RECT boundingBox;
		boundingBox.left = static_cast<int>(pos.x - size / 2);
		boundingBox.top = static_cast<int>(pos.y - size / 2);
		boundingBox.right = static_cast<int>(pos.x + size / 2);
		boundingBox.bottom = static_cast<int>(pos.y + size / 2);
		return boundingBox;
	}
private:
	PlayerType Ptype = PlayerType::None;
	PlayerState Pstate = PlayerState::Idle;
	float animationTimer = 0.0f;
	float jumpVelocity = 0.0f;
	float gravity = 980.0f;
	float jumpStrength = 450.0f;
	bool isJumping = false;
	bool isOnGround = false;
	bool haskey = false;
	int Movement = 0;
	int dir = 0;
	int stageNum;
	uint32_t playerID;
	KT input;
	uint64_t remainingInputTime = 0;
	bool switchTriggered = false;
};
