#pragma once
#include "CScene.h"

class EndingScene : public CScene
{
public:
	EndingScene();
	~EndingScene() override;

	void Init() override;
	void Update() override;
	void Render(HDC hdc, uint32_t mapid) override;
private:
	CImage Background;
	float elapsedTime;
};

