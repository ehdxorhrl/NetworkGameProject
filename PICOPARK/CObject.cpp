#include "stdafx.h"
#include "CObject.h"

//PLAYER::PLAYER()
//{
//	size = 25;
//
//	isjump = false;
//	isjumpDown = false;
//	isjumpCnt = 0;
//	isjumpDownCnt = 0;
//
//	shooting = false;
//	isshooting = false;
//	shootingCnt = 0;
//	direction = 1;	//1.left	2.right		3.up	4.down
//
//	isOnGround = false;
//	isOnPlayer = false;
//	isFlying = false;
//
//	haveKey = false;
//	exit = false;
//	exitAll = false;
//
//	ismoving = false;
//	isdeath = false;
//	imageCnt = 0;
//}
//
//PLAYER::~PLAYER()
//{
//}
//
//void PLAYER::Init()
//{
//	size = 25;
//
//	isjump = false;
//	isjumpDown = false;
//	isjumpCnt = 0;
//	isjumpDownCnt = 0;
//
//	shooting = false;
//	isshooting = false;
//	shootingCnt = 0;
//	direction = 1;	//1.left	2.right		3.up	4.down
//
//	isOnGround = false;
//	isOnPlayer = false;
//	isFlying = false;
//
//	haveKey = false;
//	exit = false;
//	exitAll = false;
//
//	ismoving = false;
//	isdeath = false;
//	imageCnt = 0;
//}
//
//void PLAYER::UpdateMovement()
//{
//	if (ismoving) {
//		imageCnt++;
//		imageCnt = imageCnt % 2;
//	}
//	else {
//		imageCnt = 0;
//	}
//	ismoving = false;
//}
//
//void PLAYER::RenewPos()
//{
//	x1 = x - size;
//	x2 = x + size;
//	y1 = y - size;
//	y2 = y + size;
//}
//
//void PLAYER::Draw(CImage* P1Image, CImage* P2Image, HDC mdc, int playerIndex) {
//	int imageIndex = isdeath ? 3 : (isjump ? 2 : imageCnt);
//	CImage* playerImage = (playerIndex == 0) ? P1Image : P2Image;
//	playerImage[imageIndex].Draw(mdc, x1, y1, size * 2, size * 2, 0, 0, 100, 100);
//}
//
//BULLET::BULLET()
//{
//	size = 10;
//	select = false;
//	direction = 0;	//1.left	2.right
//}
//
//BULLET::~BULLET()
//{
//}

CObject::CObject()
{
}

CObject::~CObject()
{
}
