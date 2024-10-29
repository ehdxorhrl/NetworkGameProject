#pragma once
#include "CScene.h"

class MainScene :public CScene
{
public:
	MainScene();
	~MainScene() override;

	void Init() override;
	void Update() override;
	void Render(HDC hdc) override;
};

