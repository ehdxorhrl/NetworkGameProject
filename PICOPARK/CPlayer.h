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

	void Render(HDC hdc) override;
	void Init() override;
	void Update() override;
public:
	void UpdateAnimation(float deltaTime);
	PlayerType GetPtype() { return Ptype; }
	uint32_t GetPlayerID() { return playerID; }
	void SetPlayerID(uint32_t _playerID) { playerID = _playerID; }
	void Setinfo(ObjectInfo_Packet* _info);
	int GetstageNum() { 
		return stageNum; 
	}
private:
	CImage PImage[5];
	PlayerType Ptype = PlayerType::None;
	PlayerState Pstate = PlayerState::Idle;
	float animationTimer = 0.0f;
	int Movement = 0;
	int stageNum;
	uint32_t playerID;
	ObjectInfo_Packet info;
};
