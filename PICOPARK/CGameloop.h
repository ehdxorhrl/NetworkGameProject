#pragma once

class CGameloop
{
public:
	CGameloop();
	~CGameloop();
public:
	void Init(HWND hWnd);
	void Update();
	void Render();
	HDC GetHDC() const { return hdc; }
private:
	HWND hwnd{};
	HDC hdc{};
	TimeManager& timeManager = TimeManager::GetInstance();
	InputManager& inputManager = InputManager::GetInstance();
	SceneManager& SceneManager = SceneManager::GetInstance();
};