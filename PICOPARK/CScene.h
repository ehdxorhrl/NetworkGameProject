#pragma once
#include "stdafx.h"

class CScene
{
public:
	CScene();
	virtual ~CScene();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render(HDC hdc, uint32_t mapid) = 0;
	bool IsSceneChangeRequired() { return shouldChangeScene; }

protected:
	bool shouldChangeScene = false;
};

