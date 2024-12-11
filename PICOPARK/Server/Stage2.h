#pragma once
#include "stdafx.h"
#include "CScene.h"
#include "ObjectManager.h"
#include "CPlayer.h"

class Stage2 :public CScene
{
public:
	Stage2();
	~Stage2() override;

	void Init() override;
	void Update() override;

public:
	void InitializeMap();
	void TransitionToNextMap(CPlayer* player);
	int(*GetMapData(int mapID))[8] { return maps[mapID]; }
	int GetCurrentMapID() const { return currentMapID; }
	bool IsDoorOpen() { return openDoor; }
private:
	int currentMapID;
	int maps[3][8][8]{};
	bool openDoor = false;
};

