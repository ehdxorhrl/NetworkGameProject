#pragma once
#include <WinUser.h>
#include <cstdint>
#include "CObject.h"

enum class PlayerState
{
	Idle,
	Move,
	Jump,
	MoveAndJump,
	Die
};

enum class KeyType
{
	Left_Mouse = VK_LBUTTON,

	Up = VK_UP,
	Down = VK_DOWN,
	Left = VK_LEFT,
	Right = VK_RIGHT,

	R = 'R'
};

enum class KeyState
{
	None,
	Press,
	Down,
	Up,

	End
};

enum class SceneType
{
	None,
	Intro,
	Main,
	Stage1,
	Stage2,
	Stage3,
	Ending
};

struct PlayerInfo
{
	uint32_t m_playerID;
	float m_x, m_y;
	int m_size;
	PlayerState m_state;
	bool m_haskey;
	int m_movement;
	bool m_restart;
};

struct KeyInfo
{
	float m_x, m_y;
	bool m_haskey;
};

struct DoorInfo
{
	float m_x, m_y;
	bool m_Isopen;
};

///////////////////////////////////////////////////////

struct Input_Packet
{
	uint8_t packetType = 1;
	SceneType m_scene;
	uint32_t m_playerID;
	KeyType inputType;
	KeyState inputState;
};

struct PlayerIDRequest_Packet
{
	uint8_t packetType;
};