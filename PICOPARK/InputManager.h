#pragma once

enum class KeyType
{
	Left_Mouse = VK_LBUTTON,

	Up = VK_UP,
	Down = VK_DOWN,
	Left = VK_LEFT,
	Right = VK_RIGHT,

	Space = VK_SPACE,

	R = 'R',
	r = 'r',
	None
};

enum class KeyState
{
	None,
	Press,
	Down,
	Up,

	End
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32_t>(UINT8_MAX) + 1,
	KEY_STATE_COUNT = static_cast<int32_t>(KeyState::End)
};

class InputManager
{
public:
	static InputManager& GetInstance()
	{
		static InputManager instance; // C++11 이후의 thread-safe 정적 변수 초기화
		return instance;
	}
	// 싱글톤 패턴이기 때문에 복사 생성자와 대입 연산자를 삭제하여 인스턴스가 복사되지 않도록 함
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager() = default;
	~InputManager() = default;

	void Init(HWND hwnd);
	void Update();
	bool IsKeyDown(int key) { return states[key] == KeyState::Down || states[key] == KeyState::Press; }
	bool GetButton(KeyType key) { return GetState(key) == KeyState::Press; }
	bool GetButtonDown(KeyType key) { return GetState(key) == KeyState::Down; }
	bool GetButtonUp(KeyType key) { return GetState(key) == KeyState::Up; }
	POINT GetMousePos() { return mousePos; }
	KeyState GetState(KeyType key) { return states[static_cast<uint8_t>(key)]; }

private:
	HWND hwnd;
	vector<KeyState> states;
	POINT mousePos;
}; 

