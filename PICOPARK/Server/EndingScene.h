#pragma once
#include "CScene.h"

class EndingScene :public CScene
{
public:
	EndingScene();
	~EndingScene() override;

	void Init() override;
	void Update() override;

	void SetServerSignalReceived(bool received) { serverSignalReceived = received; }
	bool IsServerSignalReceived() const { return serverSignalReceived; }

private:
	bool serverSignalReceived = false;
};

