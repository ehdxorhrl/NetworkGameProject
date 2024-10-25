#include "stdafx.h"

EndingScene::EndingScene()
{
}

EndingScene::~EndingScene()
{
}

void EndingScene::Init()
{
	Background.Load(L"End.png");
	_elapsedTime = 0;
    shouldChangeScene = false;
}

void EndingScene::Update()
{
    float deltaTime = TimeManager::GetInstance().GetDeltaTime();

    _elapsedTime += deltaTime;

    if (_elapsedTime >= 5.0f) { 
        shouldChangeScene = true;
        _elapsedTime = 0.f;
    }
}

void EndingScene::Render(HDC hdc)
{
    Background.Draw(hdc, 0, 0, MAPSIZE, MAPSIZE, 0, 0, 800, 800);
}
