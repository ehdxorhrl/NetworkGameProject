#pragma once
#include <windows.h>
#include <cstdint>
#include <vector>
#include <memory>

// Enums
enum class PS { Idle, Move, Jump, MoveAndJump, Die };  // Player state
enum class KT { Left_Mouse = VK_LBUTTON, Up = VK_UP, Down = VK_DOWN, Left = VK_LEFT, Right = VK_RIGHT, R = 'R' }; // Key type
enum class KS { None, Press, Down, Up, End }; // Key state
enum class ST { None, Intro, Main, Stage1, Stage2, Stage3, Ending }; // Scene type

#pragma pack(push, 1) // 1바이트 정렬

// Base Packet
struct BasePacket {
    uint8_t packetType; // Packet type
    ~BasePacket() = default; // 가상 소멸자 제거
};

// Input Packet
struct Input_Packet : BasePacket {
    uint32_t m_playerID;
    KT inputType;
    KS inputState;

    Input_Packet() { packetType = 1; }
};

// Player ID Response Packet
struct PlayerIDResponsePacket : BasePacket {
    uint32_t m_playerID;
    bool isSuccess;

    PlayerIDResponsePacket() { packetType = 12; }
};

// Object Info Packet
struct ObjectInfo_Packet : BasePacket {
    ST m_scene;
    struct PlayerInfo {
        uint32_t m_playerID;
        float m_x, m_y;
        int m_size;
        PS m_state;
        bool m_haskey;
    } m_player[2];
    struct KeyInfo {
        float m_x, m_y;
        bool m_haskey;
    } m_key;

    ObjectInfo_Packet() { packetType = 13; }
};

// Game Start Packet
struct GameStart_Packet : BasePacket {
    bool isGameStarted;

    GameStart_Packet() {
        packetType = 102; // Initialize packet type
    }
};

#pragma pack(pop) // 정렬 원래대로 복구