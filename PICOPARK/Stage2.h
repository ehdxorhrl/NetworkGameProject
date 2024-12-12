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
	void Render(HDC hdc, uint32_t mapid) override;


public:
	void InitializeMap();
	int(*GetMapData(int mapID))[8] { return maps[mapID]; }
	int GetCurrentMapID() const { return currentMapID; }
	void IsDoorOpen() { openDoor = true; }
private:
	int currentMapID;
	int maps[3][8][8]{};
	CImage BlockImage[10];
	bool openDoor = false;
};

