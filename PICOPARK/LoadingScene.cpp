#pragma once
#include "stdafx.h"
#include "LoadingScene.h"

LoadingScene::LoadingScene()
{
}

LoadingScene::~LoadingScene()
{
}

void LoadingScene::Init()
{
	HRESULT result = LoadingImage.Load(L"assets/로딩창.png");
	if (FAILED(result)) {
		MessageBox(NULL, L"Failed to load image: assets/로딩창.png", L"Error", MB_OK);
	}
}

void LoadingScene::Update()
{

}

void LoadingScene::Render(HDC hdc)
{
	OutputDebugString(L"LoadingScene::Render called\n");
	LoadingImage.Draw(hdc, 0, 0, MAPSIZE, MAPSIZE, 0, 0, 800, 800);
}
