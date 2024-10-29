#include "Stage1.h"

Stage1::Stage1()
{

}

Stage1::~Stage1()
{
}

void Stage1::Init()
{
	player = new CPlayer();
	player->Init();
}

void Stage1::Update()
{
	player->Update();
}

void Stage1::Render(HDC hdc)
{
	player->Render(hdc);
}
