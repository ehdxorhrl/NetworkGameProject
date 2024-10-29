#include "stdafx.h"

void InputManager::Init(HWND hwnd)
{
	hwnd = hwnd;
	states.resize(KEY_TYPE_COUNT, KeyState::None);
}

void InputManager::Update()
{
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)
		return;
	for (uint32_t key = 0; key < KEY_TYPE_COUNT; key++)
	{
		if (asciiKeys[key] & 0x80) // 키가 눌려있는 상태 0x80은 비트마스크
		{
			KeyState& state = states[key];

			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Press;
			else
				state = KeyState::Down;
		}
		else
		{
			KeyState& state = states[key];

			//이전 프레임에 키를 누른 상태라면 Up
			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Up;
			else
				state = KeyState::None;
		}
	}

	::GetCursorPos(&mousePos);  // 커서의 좌표를 가져온다
	::ScreenToClient(hwnd, &mousePos);
}
