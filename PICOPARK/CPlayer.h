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

	void Render(HDC hdc) override;
	void Init() override;
	void Update() override;
public:
	void UpdateAnimation(float deltaTime);
	PlayerType GetPtype() { return Ptype; }
	void Setinfo(ObjectInfo_Packet* _info) { info = _info; }
private:
	CImage PImage[5];
	PlayerType Ptype = PlayerType::None;
	PlayerState Pstate = PlayerState::Idle;
	float animationTimer = 0.0f;
	int Movement = 0;
	int dir = 0;
	ObjectInfo_Packet* info;
};
