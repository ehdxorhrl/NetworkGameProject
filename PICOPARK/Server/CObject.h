#pragma once
#include "stdafx.h"

enum class ObjectType
{
	None,
	Player,
	Player_Bullet,
	Block,   // �Ϲ� ��
	Brown_Block,   // ������ ��
	Yellow_Block,  // Ű�� ������ ����� ��
	Green_Block,  // �ʷ� ����ġ ������ ����� ��
	Door,  // ��
	Key,  // Ű
	Cannon,   // ����
	Cannon_Bullet,  // ������
	Red_Switch,  // ������ Ŀ���� ����ġ
	Blue_Switch,  // ������ �۾����� ����ġ
	Green_Switch  // ������ ���� ��Ÿ���� ����ġ
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