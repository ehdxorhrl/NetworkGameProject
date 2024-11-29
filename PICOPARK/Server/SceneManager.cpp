#pragma once
#include "stdafx.h"
#include "CScene.h"
#include "LoadingScene.h"    // LoadingScene 클래스 헤더 파일
#include "Stage1.h"
#include "CGameloop.h"

void SceneManager::Init()
{
	OutputDebugString(L"SceneManager::Init called\n");

	CScene* nScene = nullptr;
	SceneType nSceneType = SceneType::None;

	nScene = new Stage1();
	nSceneType = SceneType::Stage1;

	scene = nScene;
	sceneType = nSceneType;

	if (scene) {
		scene->Init();
	}
}


void SceneManager::Update()
{
	if (scene)
		scene->Update();
	if (scene->IsSceneChangeRequired())
		ChangeScene();
}

void SceneManager::Clear()
{
}

void SceneManager::ChangeScene()
{
	CScene* nScene = nullptr;
	SceneType nSceneType = SceneType::None;

	switch (sceneType)
	{
	case SceneType::Stage1:
		nScene = new Stage1();
		nSceneType = SceneType::Stage1;
		break;
		//case SceneType::Stage1:
		//	nScene = new EndingScene();
		//	nSceneType = SceneType::Ending;
		//	break;
		//}

		if (scene) {
			delete scene;
			scene = nullptr;
		}

		scene = nScene;
		sceneType = nSceneType;
		nScene->Init();
	}
}
