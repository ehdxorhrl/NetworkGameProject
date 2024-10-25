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
	HDC GetHDC() const { return _hdc; }
private:
	HWND _hwnd{};
	HDC _hdc{};
	TimeManager& _timeManager = TimeManager::GetInstance();
	InputManager& _inputManager = InputManager::GetInstance();
	SceneManager& _SceneManager = SceneManager::GetInstance();
};