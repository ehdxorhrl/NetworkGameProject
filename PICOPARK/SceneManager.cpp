#include "stdafx.h"

void SceneManager::Init()
{

	CScene* nScene = nullptr;
	SceneType nSceneType = SceneType::None;

	nScene = new Intro();
	nSceneType = SceneType::Intro;

	_scene = nScene;
	_sceneType = nSceneType;
	nScene->Init();
}


void SceneManager::Update()
{
	if(_scene)
		_scene->Update();
	if (_scene->IsSceneChangeRequired())
		ChangeScene();
}

void SceneManager::Render(HDC hdc)
{
	if (_scene)
		_scene->Render(hdc);
}

void SceneManager::Clear()
{
}

void SceneManager::ChangeScene()
{
	CScene* nScene = nullptr;
	SceneType nSceneType = SceneType::None;

	switch (_sceneType)
	{
	case SceneType::Intro:
		nScene = new MainScene();
		nSceneType = SceneType::Main;
		break;
	case SceneType::Main:
		nScene = new EndingScene();
		nSceneType = SceneType::Ending;
		break;
	}

	if (_scene) {
		delete _scene;
		_scene = nullptr;
	}

	_scene = nScene;
	_sceneType = nSceneType;
	nScene->Init();
}
