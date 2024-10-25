#pragma once
class TimeManager
{
public:
	static TimeManager& GetInstance()
	{
		static TimeManager instance;
		return instance;
	}
	TimeManager(const TimeManager&) = delete;
	TimeManager& operator=(const TimeManager&) = delete;
	TimeManager() = default;
	~TimeManager() = default;

	void Init();
	void Update();

	float GetDeltaTime() const { return _deltaTime; }
	uint32_t GetFps() { return _fps; }
private:
	uint64_t _frequency = 0;
	uint64_t _prevCount = 0;
	float _deltaTime = 0.f;
private:
	uint32_t _frameCount = 0;
	float _frameTime = 0.f;
	uint32_t _fps = 0;
};

