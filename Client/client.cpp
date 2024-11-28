#include <winsock2.h>
#include <ws2tcpip.h> // inet_pton 및 inet_ntop 사용
#include <iostream>
#include <cstring>
#include "Packet.h" // Ensure this contains packet definitions
#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // 서버 IP
#define SERVER_PORT 8080      // 서버 포트
 
constexpr int BUF_SIZE = 1024;
char buffer[BUF_SIZE];
int Size = 0;

// 타임아웃 설정 함수
bool SetSocketTimeout(SOCKET& sock, int timeoutInSeconds) {
    int timeoutInMillis = timeoutInSeconds * 1000;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis)) == SOCKET_ERROR) {
        std::cerr << "Failed to set recv timeout: " << WSAGetLastError() << std::endl;
        return false;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis)) == SOCKET_ERROR) {
        std::cerr << "Failed to set send timeout: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

// 패킷 전송 함수
void SendPacket(SOCKET& sock, const void* packet, size_t packetSize) {
    if (send(sock, reinterpret_cast<const char*>(packet), packetSize, 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send packet: " << WSAGetLastError() << std::endl;
    }
    else
    {
        std::cout << "Success" << std::endl;
    }
}

// 패킷 수신 함수
void RecvPackets(SOCKET& sock) {
    char recvBuffer[1024];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "recv() failed or connection closed: " << WSAGetLastError() << std::endl;
            break;
        }

        uint8_t opCode = recvBuffer[0];
        std::cout << "Received packet with opCode: " << (int)opCode << ", size: " << bytesReceived << " bytes" << std::endl;

        // 패킷 처리 로직 (예시)
        switch (opCode) {
        case 11: {
            GTime_Packet gTimePacket;
            memcpy(&gTimePacket, recvBuffer, sizeof(GTime_Packet));
            std::cout << "Game Time: " << gTimePacket.gameTime << "ms" << std::endl;
            break;
        }
        case 13: {
            ObjectInfo_Packet objectInfo;
            memcpy(&objectInfo, recvBuffer, sizeof(ObjectInfo_Packet));
            std::cout << "Received Object Info for Scene: " << (int)objectInfo.m_scene << std::endl;
            break;
        }
        default:
            std::cout << "Unknown opCode: " << (int)opCode << std::endl;
            break;
        }
    }
}

template <typename Type>
void writeBuffer(Type data)
{
    memcpy(buffer + Size, &data, sizeof(Type));
    Size += sizeof(Type);
}

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;

    // Winsock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // 소켓 생성
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    // inet_pton을 사용하여 IP 주소 변환
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << SERVER_IP << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // 서버 연결
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server at " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    // 타임아웃 설정
    if (!SetSocketTimeout(clientSocket, 5)) {
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // 패킷 보내기 예제
   
    short opcode = 0;
    while (true)
    {
        Size = 0;

        Input_Packet input_packet = { SceneType::Main, 0, KeyType::Up, KeyState::Press };
        opcode = 1;

        writeBuffer<short>(opcode);
        writeBuffer<Input_Packet>(input_packet);

        GTime_Packet gTime_packet = { 1 };
        opcode = 11;
        
        writeBuffer<short>(opcode);
        writeBuffer<GTime_Packet>(gTime_packet);

        PlayerIDResponsePacket playerIDResponsePacket = {  };
        opcode = 12;

        writeBuffer<short>(opcode);
        writeBuffer<PlayerIDResponsePacket>(playerIDResponsePacket);

        DoorInfo door = {
            .m_x = 0,
            .m_y = 0,
            .m_Isopen = true
        };

        PlayerInfo playerInfo[2] = {
            {
                .m_playerID = 0,
                .m_x = 1,
                .m_y = 1
            },
            {
                .m_playerID = 1,
                .m_x = 2,
                .m_y = 2
            }
        };

        KeyInfo keyinfo =
        {
            .m_x = 3,
            .m_y = 3,
            .m_haskey = true
        };

        ObjectInfo_Packet objectInfo_packet = { 
            .m_scene = SceneType::Intro,
            .m_player = {playerInfo[0], playerInfo[1]},
            .m_key = keyinfo,
            .m_door = door
        };
        opcode = 13;

        writeBuffer<short>(opcode);
        writeBuffer<ObjectInfo_Packet>(objectInfo_packet);

        EndGame_Packet endGame_packet = { false };
        opcode = 101;

        writeBuffer<short>(opcode);
        writeBuffer<EndGame_Packet>(endGame_packet);

        SendPacket(clientSocket, &Size, sizeof(int));
        SendPacket(clientSocket, buffer, Size);
    }
    

    // 서버로부터 패킷 받기
    RecvPackets(clientSocket);

    // 소켓 정리
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
