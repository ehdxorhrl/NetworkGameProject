#pragma once
#include <windows.h>
#include <cstdint>
#include <vector>
#include <memory>
#include "stdafx.h"

#pragma pack(push, 1) // 1����Ʈ ����

// Base Packet
struct BasePacket {
    uint8_t packetType; // Packet type
    ~BasePacket() = default; // ���� �Ҹ��� ����
};

// Input Packet
struct Input_Packet : BasePacket {
    uint32_t m_playerID;
    KT inputType;
    KS inputState;
    uint64_t inputTime;

    Input_Packet() {
        packetType = 1;
        inputTime = 0;
    }
};

// Player ID Response Packet
struct PlayerIDResponsePacket : BasePacket {
    uint32_t m_playerID;
    bool isSuccess;

    PlayerIDResponsePacket() { packetType = 12; }
};

// Object Info Packet
struct ObjectInfo_Packet : BasePacket {
    PlayerInfo m_player;
    ObjectInfo_Packet() { packetType = 13; }
};

// Game Start Packet
struct GameStart_Packet : BasePacket {
    bool isGameStarted;

    GameStart_Packet() {
        packetType = 102; // Initialize packet type
    }
};

#pragma pack(pop) // ���� ������� ����
