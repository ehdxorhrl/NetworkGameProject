#pragma once
#include "CScene.h"
#include "CPlayer.h"

class Stage1 :public CScene
{
public:
	Stage1();
	~Stage1() override;

	void Init() override;
	void Update() override;
	void Render(HDC hdc) override;
private:
	CPlayer* player = nullptr;
};

