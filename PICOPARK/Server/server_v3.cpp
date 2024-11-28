#include <winsock2.h>
#include <iostream>
#include <queue>
#include <memory>
#include <chrono>
#include <windows.h>
#include "Packet.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 9000
#define MAX_CLIENTS 1
#define BUFFER_SIZE 512

// 임계영역
CRITICAL_SECTION RecvQueueCS;
CRITICAL_SECTION SendQueueCS;

// 큐
std::queue<std::unique_ptr<BasePacket>> RecvQueue;
std::queue<std::unique_ptr<BasePacket>> SendQueue;

// 클라이언트 소켓
SOCKET clientSockets[MAX_CLIENTS];

// Communication Thread 함수
DWORD WINAPI CommunicationThread(LPVOID lpParam) {
    int playerID = *(int*)lpParam;
    SOCKET clientSocket = clientSockets[playerID];
    delete (int*)lpParam;

    char buffer[BUFFER_SIZE];

    while (true) {
        // 클라이언트로부터 데이터 수신
        int recvSize = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (recvSize <= 0) {
            std::cerr << "Player " << playerID << " disconnected.\n";
            closesocket(clientSocket);
            clientSockets[playerID] = INVALID_SOCKET;
            return 0; // 스레드 종료
        }

        if (recvSize >= sizeof(Input_Packet)) {
            Input_Packet receivedPacket;
            memcpy(&receivedPacket, buffer, sizeof(Input_Packet));

            std::cout << "Received Input_Packet from Player " << playerID << ":\n";
            std::cout << "  Packet Type: " << static_cast<int>(receivedPacket.packetType) << "\n";
            std::cout << "  Player ID: " << receivedPacket.m_playerID << "\n";
            std::cout << "  Input Type: " << static_cast<int>(receivedPacket.inputType) << "\n";
            std::cout << "  Input State: " << static_cast<int>(receivedPacket.inputState) << "\n";
        }
        else {
            std::cerr << "Invalid packet size received: " << recvSize << " bytes.\n";
        }

        auto packet = std::make_unique<Input_Packet>();
        memcpy(packet.get(), buffer, sizeof(Input_Packet));

        // RecvQueue에 입력 데이터 저장
        EnterCriticalSection(&RecvQueueCS);
        RecvQueue.push(std::move(packet));
        LeaveCriticalSection(&RecvQueueCS);

        // SendQueue에서 데이터를 가져와 클라이언트에게 전송
        EnterCriticalSection(&SendQueueCS);
        if (!SendQueue.empty()) {
            auto sendPacket = std::move(SendQueue.front());
            SendQueue.pop();
            LeaveCriticalSection(&SendQueueCS);

            send(clientSocket, reinterpret_cast<char*>(sendPacket.get()), sizeof(ObjectInfo_Packet), 0);
        }
        else {
            LeaveCriticalSection(&SendQueueCS);
        }

        Sleep(1); // CPU 사용량 제어
    }
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    InitializeCriticalSection(&RecvQueueCS);
    InitializeCriticalSection(&SendQueueCS);

    int clientCount = 0;
    HANDLE communicationThreads[MAX_CLIENTS];

    // 클라이언트 연결 및 처리
    while (clientCount < MAX_CLIENTS) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        clientSockets[clientCount] = clientSocket;

        std::cout << "Client with ID " << clientCount << " connected.\n";

        int* param = new int(clientCount);
        communicationThreads[clientCount] = CreateThread(
            nullptr,
            0,
            CommunicationThread,
            param,
            0,
            nullptr
        );

        clientCount++;
    }

    // 게임 시작 신호 전송
    auto gameStartPacket = std::make_unique<GameStart_Packet>();
    gameStartPacket->isGameStarted = true;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clientSockets[i] != INVALID_SOCKET) {
            int sendResult = send(clientSockets[i],
                reinterpret_cast<const char*>(gameStartPacket.get()),
                sizeof(GameStart_Packet),
                0);

            if (sendResult == SOCKET_ERROR) {
                std::cerr << "Failed to send game start packet to Client ID " << i << ".\n";
            }
            else {
                std::cout << "Game start packet sent to Client ID " << i << ".\n";
            }
        }
    }

    // 패킷 전송 루프
    auto lastUpdate = std::chrono::high_resolution_clock::now();
    const std::chrono::milliseconds updateInterval(30); // 약 33ms

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate) >= updateInterval) {
            lastUpdate = now;

            // RecvQueue에서 데이터를 TempQueue로 이동
            std::queue<std::unique_ptr<BasePacket>> TempQueue;

            EnterCriticalSection(&RecvQueueCS);
            while (!RecvQueue.empty()) {
                TempQueue.push(std::move(RecvQueue.front()));
                RecvQueue.pop();
            }
            LeaveCriticalSection(&RecvQueueCS);

            // TempQueue 데이터 처리
            while (!TempQueue.empty()) {
                auto packet = std::move(TempQueue.front());
                TempQueue.pop();

                // Input_Packet 처리
                if (packet->packetType == 1) {
                    auto inputPacket = std::unique_ptr<Input_Packet>(static_cast<Input_Packet*>(packet.release()));
                    std::cout << "Processing Input_Packet from Player " << inputPacket->m_playerID << ":\n";
                    std::cout << "  Input Type: " << static_cast<int>(inputPacket->inputType) << "\n";
                    std::cout << "  Input State: " << static_cast<int>(inputPacket->inputState) << "\n";

                    // 업데이트 로직 (예: 플레이어 위치 업데이트)
                    auto objectPacket = std::make_unique<ObjectInfo_Packet>();
                    objectPacket->m_scene = ST::Main;
                    objectPacket->m_player[inputPacket->m_playerID].m_x += 10.0f; // 예시 업데이트
                    objectPacket->m_player[inputPacket->m_playerID].m_y += 5.0f;

                    // SendQueue에 추가
                    EnterCriticalSection(&SendQueueCS);
                    SendQueue.push(std::move(objectPacket));
                    LeaveCriticalSection(&SendQueueCS);
                }
            }

            // SendQueue에서 패킷 전송
            EnterCriticalSection(&SendQueueCS);
            while (!SendQueue.empty()) {
                auto sendPacket = std::move(SendQueue.front());
                SendQueue.pop();

                for (int i = 0; i < MAX_CLIENTS; ++i) {
                    if (clientSockets[i] != INVALID_SOCKET) {
                        send(clientSockets[i], reinterpret_cast<char*>(sendPacket.get()), sizeof(ObjectInfo_Packet), 0);
                    }
                }
            }
            LeaveCriticalSection(&SendQueueCS);
        }

        Sleep(1); // CPU 사용량 제어
    }

    // 종료 처리
    for (int i = 0; i < clientCount; ++i) {
        if (communicationThreads[i]) {
            WaitForSingleObject(communicationThreads[i], INFINITE);
            CloseHandle(communicationThreads[i]);
        }
    }

    DeleteCriticalSection(&RecvQueueCS);
    DeleteCriticalSection(&SendQueueCS);

    for (int i = 0; i < clientCount; ++i) {
        if (clientSockets[i] != INVALID_SOCKET) {
            closesocket(clientSockets[i]);
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}