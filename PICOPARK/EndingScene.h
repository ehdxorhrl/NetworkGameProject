#pragma once

class EndingScene : public CScene
{
public:
	EndingScene();
	~EndingScene() override;

	void Init() override;
	void Update() override;
	void Render(HDC hdc) override;
private:
	CImage Background;
	float _elapsedTime;
};

