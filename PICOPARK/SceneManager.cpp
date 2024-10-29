#include "stdafx.h"
#include "CScene.h"
#include "Intro.h"        // Intro 클래스 헤더 파일
#include "MainScene.h"    // MainScene 클래스 헤더 파일
#include "Stage1.h"
#include "EndingScene.h"  // EndingScene 클래스 헤더 파일

void SceneManager::Init()
{

	CScene* nScene = nullptr;
	SceneType nSceneType = SceneType::None;

	nScene = new Intro();
	nSceneType = SceneType::Intro;

	scene = nScene;
	sceneType = nSceneType;
	nScene->Init();
}


void SceneManager::Update()
{
	if(scene)
		scene->Update();
	if (scene->IsSceneChangeRequired())
		ChangeScene();
}

void SceneManager::Render(HDC hdc)
{
	if (scene)
		scene->Render(hdc);
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
	case SceneType::Intro:
		//nScene = new MainScene();
		//nSceneType = SceneType::Main;
		nScene = new Stage1();
		nSceneType = SceneType::Stage1;
		break;
	case SceneType::Stage1:
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
