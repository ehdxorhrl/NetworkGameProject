#pragma once

class CScene
{
public:
	CScene();
	virtual ~CScene();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render(HDC hdc) = 0;
	bool IsSceneChangeRequired() { return shouldChangeScene; }

protected:
	bool shouldChangeScene = false;
};

