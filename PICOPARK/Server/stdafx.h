#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <windows.h> //--- ������ ��� ����
#include <ws2tcpip.h> // inet_pton �� inet_ntop ���
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <tchar.h>
#include <random>
#include <math.h>
#include <cstring>
#include <mmsystem.h>
#include <thread>
#include <atlImage.h>
#include <cstdint>
#include <unordered_map>

#pragma comment(lib, "winmm.lib")

const int NUM_PLAYERS = 2;
const int MAPSIZE = 800;
const int BLOCK_SIZE = 100; // �� ������ �ȼ� ũ��
const int CANNON_SIZE = 25;
const int BUFFER_SIZE = 1024;

using namespace std;
using std::vector;
using std::array;

extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_int_distribution<int> dis1;
extern std::uniform_int_distribution<int> dis2;

enum class PlayerState
{
	Idle,
	Move,
	Jump,
	MoveAndJump,
	Die,
	Shooting
};

struct PlayerInfo
{
	uint32_t m_playerID;
	float m_x, m_y;
	int m_size;
	PlayerState m_state;
	
	bool operator==(const PlayerInfo& other) const {
		return m_playerID == other.m_playerID &&
			m_x == other.m_x &&
			m_y == other.m_y &&
			m_state == other.m_state;
	}

	bool operator!=(const PlayerInfo& other) const {
		return !(*this == other);
	}
};

enum class KT { Left_Mouse = VK_LBUTTON, Up = VK_UP, Down = VK_DOWN, Left = VK_LEFT, Right = VK_RIGHT, R = 'R', None }; // Key type
enum class KS { None, Press, Down, Up, End }; // Key state
enum class ST { None, Intro, Main, Stage1, Stage2, Stage3, Ending }; // Scene type

#include "Packet.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "CObject.h"