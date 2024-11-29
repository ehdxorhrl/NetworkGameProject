#pragma once
#include "stdafx.h"
#include "CScene.h"
#include "ObjectManager.h"
#include "CPlayer.h"

class Stage1 :public CScene
{
public:
	Stage1();
	~Stage1() override;

	void Init() override;
	void Update() override;

public:
	void InitializeMap();
	void TransitionToNextMap();
	int(*GetMapData(int mapID))[8] { return maps[mapID]; }
	int GetCurrentMapID() const { return currentMapID; }
private:
	int currentMapID;
	int maps[2][8][8]{};
};

