#pragma once
#include "stdafx.h"

enum class ObjectType
{
	None,
	Player,
	Player_Bullet,
	Block,   // 일반 블럭
	Brown_Block,   // 깨지는 블럭
	Yellow_Block,  // 키를 먹으면 생기는 블럭
	Green_Block,  // 초록 스위치 밟으면 생기는 블럭
	Door,  // 문
	Key,  // 키
	Cannon,   // 대포
	Cannon_Bullet,  // 대포알
	Red_Switch,  // 밟으면 커지는 스위치
	Blue_Switch,  // 밟으면 작아지는 스위치
	Green_Switch  // 밟으면 블럭이 나타나는 스위치
};

struct Position {
	float x = 0.0f;
	float y = 0.0f;

	Position() = default;
	Position(float _x, float _y) : x(_x), y(_y) {}

	Position operator+(const Position& other) const {
		return Position(x + other.x, y + other.y);
	}

	Position operator-(const Position& other) const {
		return Position(x - other.x, y - other.y);
	}

	void Set(float _x, float _y) {
		x = _x;
		y = _y;
	}
};

class CObject
{
public:
	CObject();
	virtual ~CObject();

	virtual void Init() = 0;
	virtual void Update() = 0;
public:
	ObjectType GetObjectType() { return Otype; }
	Position GetPos() const { return pos; }
	void SetPos(const Position& _pos) { pos = _pos; }
	void SetPos(float _x, float _y) { pos.Set(_x, _y); }
protected:
	ObjectType Otype = ObjectType::None;
	Position pos;
	int size = 0, speed = 0;
};