#pragma once

class CGameloop
{
public:
	CGameloop();
	~CGameloop();
public:
	void Init();
	void Update(const Input_Packet* inputPacket);
private:
	TimeManager& timeManager = TimeManager::GetInstance();
	SceneManager& SceneManager = SceneManager::GetInstance();
};