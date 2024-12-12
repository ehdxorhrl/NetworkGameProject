#pragma once
#include "stdafx.h"
#include "CScene.h"
#include "ObjectManager.h"
#include "CPlayer.h"

class Stage3 :public CScene
{
public:
	Stage3();
	~Stage3() override;

	void Init() override;
	void Update() override;
public:
	void InitializeMap();
	void TransitionToNextMap(CPlayer* player);
	int(*GetMapData(int mapID))[8] { return maps[mapID]; }
	int GetCurrentMapID() const { return currentMapID; }
	bool IsDoorOpen() { return openDoor; }
	bool IsSwitchPressed() { return switchpress; }
private:
	int currentMapID;
	int maps[3][8][8]{};
	bool openDoor = false;
	bool switchpress = false;
};

