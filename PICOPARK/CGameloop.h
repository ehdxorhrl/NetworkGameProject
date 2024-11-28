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
	void SetID(int _id) { playerID = _id; }
	int GetID() const { return playerID; }
	void SetServerSocket(SOCKET socket);

private:
	SOCKET serverSocket; // 서버와의 연결 소켓
	HWND hwnd{};
	HDC hdc{};
	TimeManager& timeManager = TimeManager::GetInstance();
	InputManager& inputManager = InputManager::GetInstance();
	SceneManager& SceneManager = SceneManager::GetInstance();
	int playerID;
};