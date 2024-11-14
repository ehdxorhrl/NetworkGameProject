#pragma once
#include <windows.h>
#include <cstdint>
#include <vector>
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

// 클라이언트 -> 서버 패킷

struct Input_Packet  // 입력 키 패킷 type = 1
{
    uint8_t packetType = 1;      // 패킷 종류 식별
    SceneType m_scene;             // 어느씬에서 입력인지
    uint32_t m_playerID;         // 플레이어 ID
    KeyType inputType;           // 입력 타입 (방향키, 점프키 등)
    KeyState inputState;         // 입력 상태 (Press, Down, Up)
};


struct PlayerIDRequest_Packet // ID 요청 패킷 type = 2
{
    uint8_t packetType = 2;
};

// 서버 -> 클라이언트

struct GTime_Packet // 게임 시간 패킷 type = 11
{
    uint8_t packetType = 11;
    uint32_t gameTime;         // 게임 시간(초 또는 밀리초 단위)
};

struct PlayerIDResponsePacket // ID 응답 패킷 type = 12
{
    uint8_t packetType = 12;
    uint32_t m_playerID;       // 서버에서 부여한 고유한 플레이어 ID
    bool isSuccess;            // 요청 성공 여부
};

struct ObjectInfo_Packet // 오브젝트 정보 패킷 type = 13
{
    uint8_t packetType = 13;
    SceneType m_scene;
    PlayerInfo m_player[2];
    KeyInfo m_key;
    DoorInfo m_door;
};

struct EndGame_Packet // 종료 신호 패킷 type = 101
{
    uint8_t packetType = 101;
    bool isGameEnded;          // 게임 종료 여부
};