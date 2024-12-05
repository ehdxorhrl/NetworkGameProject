#pragma once
#include "CScene.h"

class Intro : public CScene
{
public:
	Intro();
	~Intro() override;

	void Init() override;
	void Update() override;
	void Render(HDC hdc, uint32_t mapid) override;
private:
	CImage Background;
};