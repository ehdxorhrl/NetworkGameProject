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
	virtual void Render(HDC hdc) = 0;
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




//enum Ptype
//{
//	One = 0,
//	Two
//};
//
//enum class Btype
//{
//	PLAYER = 0,
//	CANNON
//};
//
//class PLAYER
//{
//public:
//	int x, y;
//	int x1, y1, x2, y2;
//	int type;
//	int size = 25;
//
//	bool isjump = false;
//	bool isjumpDown = false;
//	int isjumpCnt = 0;
//	int isjumpDownCnt = 0;
//
//	bool shooting = false;
//	bool isshooting = false;
//	int shootingCnt = 0;
//	int direction = 1;	//1.left	2.right		3.up	4.down
//
//	bool isOnGround = false;
//	bool isOnPlayer = false;
//	bool isFlying = false;
//
//	bool haveKey = false;
//	bool exit = false;
//	bool exitAll = false;
//
//	int ismoving = false;
//	bool isdeath = false;
//	int imageCnt = 0;
//public:
//	PLAYER();
//	~PLAYER();
//	void Init();
//	void UpdateMovement();
//	void RenewPos();
//	void Draw(CImage* P1Image, CImage* P2Image, HDC mdc, int playerIndex);
//};
//
//class BULLET
//{
//public:
//	int x, y;
//	int x1, y1, x2, y2;
//	Btype type;
//	int size = 10;
//	bool select = false;
//	int direction = 0;	//1.left	2.right
//public:
//	BULLET();
//	~BULLET();
//};
//
//class BLOCK {
//public:
//	int x, y;
//	bool select;
//	int type;    // Ÿ�� = �� CNT
//
//public:
//	BLOCK() : x(0), y(0), type(-1), select(false) {} // �⺻ ������ �߰�
//	~BLOCK() {}
//};