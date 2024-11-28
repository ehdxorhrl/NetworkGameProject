#include "CBullet.h"

CBullet::CBullet()
{
}

CBullet::~CBullet()
{
}

void CBullet::Render(HDC hdc)
{
	float drawX = pos.x - size / 2;
	float drawY = pos.y - size / 2;

	Image.Draw(hdc, drawX, drawY, size, size, 0, 0, 100, 100);
}

void CBullet::Init()
{
	Image.Load(L"assets/ÃÑ¾Ë1.jpg");
	size = 10;
}

void CBullet::Update()
{
}
