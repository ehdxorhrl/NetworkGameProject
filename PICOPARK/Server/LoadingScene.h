#pragma once
#include "CScene.h"

class LoadingScene :public CScene
{
public:
	LoadingScene();
	~LoadingScene() override;

	void Init() override;
	void Update() override;

	void SetServerSignalReceived(bool received) { serverSignalReceived = received; }
	bool IsServerSignalReceived() const { return serverSignalReceived; }

private:
	bool serverSignalReceived = false;
};

