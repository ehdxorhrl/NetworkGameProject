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

	std::queue<std::unique_ptr<BasePacket>> RecvQueue; // ��Ŷ ť
	CRITICAL_SECTION UpdateCS; // ����ȭ�� ���� ũ��Ƽ�� ����

private:
	SOCKET serverSocket; // �������� ���� ����
	HWND hwnd{};
	HDC hdc{};
	TimeManager& timeManager = TimeManager::GetInstance();
	InputManager& inputManager = InputManager::GetInstance();
	SceneManager& SceneManager = SceneManager::GetInstance();

	int currentMapID;
	int playerID;
};