#pragma once

enum Ptype
{
	PICO = 0,
	PARK
};

enum class Btype
{
	PLAYER = 0,
	CANNON
};

class PLAYER
{
public:
	int x, y;
	int x1, y1, x2, y2;
	int type;
	int size = 25;

	bool isjump = false;
	bool isjumpDown = false;
	int isjumpCnt = 0;
	int isjumpDownCnt = 0;

	bool shooting = false;
	bool isshooting = false;
	int shootingCnt = 0;
	int direction = 1;	//1.left	2.right		3.up	4.down

	bool isOnGround = false;
	bool isOnPlayer = false;
	bool isFlying = false;

	bool haveKey = false;
	bool exit = false;
	bool exitAll = false;

	int ismoving = false;
	bool isdeath = false;
	int imageCnt = 0;
public:
	PLAYER();
	~PLAYER();
	void Init();
	void UpdateMovement();
	void RenewPos();
	void Draw(CImage* P1Image, CImage* P2Image, HDC mdc, int playerIndex);
};

class BULLET
{
public:
	int x, y;
	int x1, y1, x2, y2;
	Btype type;
	int size = 10;
	bool select = false;
	int direction = 0;	//1.left	2.right
public:
	BULLET();
	~BULLET();
};

class BLOCK {
public:
	int x, y;
	bool select;
	int type;    // 타입 = 블럭 CNT

public:
	BLOCK() : x(0), y(0), type(-1), select(false) {} // 기본 생성자 추가
	~BLOCK() {}
};