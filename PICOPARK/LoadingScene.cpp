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
	HRESULT result = LoadingImage.Load(L"assets/�ε�â.png");
	if (FAILED(result)) {
		MessageBox(NULL, L"Failed to load image: assets/�ε�â.png", L"Error", MB_OK);
	}
}

void LoadingScene::Update()
{

	//if (waitResult == WAIT_OBJECT_0) {
	//	// �̺�Ʈ�� ��ȣ ���¶�� �� ��ȯ
	//	SceneManager::GetInstance().ChangeScene();
	//}
}

void LoadingScene::Render(HDC hdc)
{
	OutputDebugString(L"LoadingScene::Render called\n");
	LoadingImage.Draw(hdc, 0, 0, MAPSIZE, MAPSIZE, 0, 0, 800, 800);
}
