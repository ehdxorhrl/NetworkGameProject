#pragma once
#include "stdafx.h"
#include "CScene.h"
#include "LoadingScene.h"    // LoadingScene 클래스 헤더 파일
#include "EndingScene.h"
#include "Stage1.h"
#include "Stage2.h"
#include "Stage3.h"
#include "CGameloop.h"

void SceneManager::Init()
{
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

	if (scene->IsSceneChangeRequired())
		ChangeScene();

	if (scene)
		scene->Update();
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
		nScene = new Stage2();
		nSceneType = SceneType::Stage2;
		break;
	case SceneType::Stage2:
		nScene = new Stage3();
		nSceneType = SceneType::Stage3;
		break;
	case SceneType::Stage3:
		nScene = new EndingScene();
		nSceneType = SceneType::Ending;
		break;
	}

	if (scene) {
		delete scene;
		scene = nullptr;
	}

	scene = nScene;
	sceneType = nSceneType;
	nScene->Init();
}
