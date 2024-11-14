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

// Ŭ���̾�Ʈ -> ���� ��Ŷ

struct Input_Packet  // �Է� Ű ��Ŷ type = 1
{
    uint8_t packetType = 1;      // ��Ŷ ���� �ĺ�
    SceneType m_scene;             // ��������� �Է�����
    uint32_t m_playerID;         // �÷��̾� ID
    KeyType inputType;           // �Է� Ÿ�� (����Ű, ����Ű ��)
    KeyState inputState;         // �Է� ���� (Press, Down, Up)
};


struct PlayerIDRequest_Packet // ID ��û ��Ŷ type = 2
{
    uint8_t packetType = 2;
};

// ���� -> Ŭ���̾�Ʈ

struct GTime_Packet // ���� �ð� ��Ŷ type = 11
{
    uint8_t packetType = 11;
    uint32_t gameTime;         // ���� �ð�(�� �Ǵ� �и��� ����)
};

struct PlayerIDResponsePacket // ID ���� ��Ŷ type = 12
{
    uint8_t packetType = 12;
    uint32_t m_playerID;       // �������� �ο��� ������ �÷��̾� ID
    bool isSuccess;            // ��û ���� ����
};

struct ObjectInfo_Packet // ������Ʈ ���� ��Ŷ type = 13
{
    uint8_t packetType = 13;
    SceneType m_scene;
    PlayerInfo m_player[2];
    KeyInfo m_key;
    DoorInfo m_door;
};

struct EndGame_Packet // ���� ��ȣ ��Ŷ type = 101
{
    uint8_t packetType = 101;
    bool isGameEnded;          // ���� ���� ����
};