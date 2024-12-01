#pragma once

class CGameloop
{
public:
	CGameloop();
	~CGameloop();
public:
	void Init();
	void Update(const Input_Packet* inputPacket);
	void Update();
private:
	TimeManager& timeManager = TimeManager::GetInstance();
	SceneManager& SceneManager = SceneManager::GetInstance();
	ObjectManager& objectManager = ObjectManager::GetInstance();
};