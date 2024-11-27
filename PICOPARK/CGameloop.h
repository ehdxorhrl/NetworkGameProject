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
public:
	HDC GetHDC() const { return hdc; }
	HWND GetHWND() const { return hwnd; }
	void SetServerSocket(SOCKET socket);
	uint32_t GetID() { return PlayerID; }
	void SetID(int _ID) { PlayerID = _ID; }
private:
	HWND hwnd{};
	HDC hdc{};
	TimeManager& timeManager = TimeManager::GetInstance();
	InputManager& inputManager = InputManager::GetInstance();
	SceneManager& SceneManager = SceneManager::GetInstance();
	SOCKET serverSocket;
	uint32_t PlayerID;
};