#include "stdafx.h"
#include "Intro.h"

Intro::Intro()
{
}

Intro::~Intro()
{
}

void Intro::Init()
{
	Background.Load(L"assets/Intro.png");
}

void Intro::Update()
{
	InputManager& input = InputManager::GetInstance();
	if (input.IsKeyDown(VK_RETURN))
		shouldChangeScene = true;
}

void Intro::Render(HDC hdc)
{
	Background.Draw(hdc, 0, 0, MAPSIZE, MAPSIZE, 0, 0, 800, 800);
}
