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

#pragma comment(lib, "winmm.lib")

const int NUM_PLAYERS = 2;
const int MAPSIZE = 800;
const int BLOCK_SIZE = 100; // �� ����� �ȼ� ũ��
const int CANNON_SIZE = 25;
const int BUFFER_SIZE = 1024;

using namespace std;
using std::vector;
using std::array;

extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_int_distribution<int> dis1;
extern std::uniform_int_distribution<int> dis2;


#include "resource.h"
#include "Packet.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SceneManager.h"

#include "CObject.h"

