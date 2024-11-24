#pragma once
#include "CObject.h"
#include "stdafx.h"

class CBullet :public CObject
{
public:
	CBullet();
	~CBullet() override;

	void Render(HDC hdc) override;
	void Init() override;
	void Update() override;
private:
	CImage Image;
	int dir;
};

