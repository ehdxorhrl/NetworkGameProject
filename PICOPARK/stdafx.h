#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h> //--- 윈도우 헤더 파일
#include <iostream>
#include <sstream>
#include <map>
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
const int CANNON_SIZE = 25;

using namespace std;
using std::map;
using std::vector;
using std::array;

extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_int_distribution<int> dis1;
extern std::uniform_int_distribution<int> dis2;

#include "resource.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SceneManager.h"

#include "CObject.h"

