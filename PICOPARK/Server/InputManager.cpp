#include "stdafx.h"

void InputManager::Init()
{
	states.resize(KEY_TYPE_COUNT, KeyState::None);
}

void InputManager::Update()
{
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)
		return;
	for (uint32_t key = 0; key < KEY_TYPE_COUNT; key++)
	{
		if (asciiKeys[key] & 0x80) // Ű�� �����ִ� ���� 0x80�� ��Ʈ����ũ
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

			//���� �����ӿ� Ű�� ���� ���¶�� Up
			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Up;
			else
				state = KeyState::None;
		}
	}
}
