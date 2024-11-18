#include "stdafx.h"
#include "EndingScene.h"

EndingScene::EndingScene()
{
}

EndingScene::~EndingScene()
{
}

void EndingScene::Init()
{
	Background.Load(L"assets/End.png");
	elapsedTime = 0;
    shouldChangeScene = false;
}

void EndingScene::Update()
{
    float deltaTime = TimeManager::GetInstance().GetDeltaTime();

    elapsedTime += deltaTime;

    if (elapsedTime >= 5.0f) { 
        shouldChangeScene = true;
        elapsedTime = 0.f;
    }
}

void EndingScene::Render(HDC hdc)
{
    Background.Draw(hdc, 0, 0, MAPSIZE, MAPSIZE, 0, 0, 800, 800);
}
