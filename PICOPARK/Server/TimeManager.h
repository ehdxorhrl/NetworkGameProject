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

	float GetDeltaTime() const { return deltaTime; }
	uint32_t GetFps() { return fps; }
private:
	uint64_t frequency = 0;
	uint64_t prevCount = 0;
	float deltaTime = 0.f;
private:
	uint32_t frameCount = 0;
	float frameTime = 0.f;
	uint32_t fps = 0;
};

