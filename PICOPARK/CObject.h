#pragma once

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

class CObject
{
public:
	CObject();
	virtual ~CObject();
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render(HDC hdc) = 0;
protected:
	ObjectType Otype = ObjectType::None;
	float x = 0, y = 0;
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
//	int type;    // 타입 = 블럭 CNT
//
//public:
//	BLOCK() : x(0), y(0), type(-1), select(false) {} // 기본 생성자 추가
//	~BLOCK() {}
//};