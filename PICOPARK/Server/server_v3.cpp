#include "stdafx.h"
#include <winsock2.h>
#include <iostream>
#include <queue>
#include <memory>
#include <chrono>
#include <windows.h>
#include "CGameloop.h"
#include "CPlayer.h"


#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 9000
#define MAX_CLIENTS 1
#define BUFFER_SIZE 512

CGameloop loop;

int cnt = 0;

// �Ӱ迵��
CRITICAL_SECTION RecvQueueCS;
CRITICAL_SECTION SendQueueCS;

// ť
std::queue<std::unique_ptr<BasePacket>> RecvQueue;
std::queue<std::unique_ptr<BasePacket>> SendQueue;

// Ŭ���̾�Ʈ ����
SOCKET clientSockets[MAX_CLIENTS];

// Communication Thread �Լ�
DWORD WINAPI CommunicationThread(LPVOID lpParam) {
    int playerID = *(int*)lpParam;
    SOCKET clientSocket = clientSockets[playerID];
    delete (int*)lpParam;

    char buffer[BUFFER_SIZE];

    while (true) {
        // Ŭ���̾�Ʈ�κ��� ������ ����
        int recvSize = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (recvSize <= 0) {
            std::cerr << "Player " << playerID << " disconnected.\n";
            closesocket(clientSocket);
            clientSockets[playerID] = INVALID_SOCKET;
            return 0; // ������ ����
        }

        auto packet = std::make_unique<Input_Packet>();
        memcpy(packet.get(), buffer, sizeof(Input_Packet));

        // ����� ���
        std::cout << "Received Input_Packet from Player " << playerID << ":" << std::endl;
        std::cout << "  Packet Type: " << static_cast<int>(packet->packetType) << std::endl;
        std::cout << "  Player ID: " << packet->m_playerID << std::endl;
        std::cout << "  Input Type: " << static_cast<int>(packet->inputType) << std::endl;
        std::cout << "  Input State: " << static_cast<int>(packet->inputState) << std::endl;
        std::cout << "  inputTime: " << packet->inputTime << std::endl;

        // RecvQueue�� �߰�
        EnterCriticalSection(&RecvQueueCS);
        RecvQueue.push(std::move(packet));
        LeaveCriticalSection(&RecvQueueCS);

        // SendQueue���� �����͸� ������ Ŭ���̾�Ʈ���� ����
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

        Sleep(1); // CPU ��뷮 ����
    }
}

int main() {
    loop.Init();
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

    // Ŭ���̾�Ʈ ���� �� ó��
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

    // ���� ���� ��ȣ ����
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

    // ��Ŷ ���� ����
    auto lastUpdate = std::chrono::high_resolution_clock::now();
    const std::chrono::milliseconds updateInterval(30); // �� 33ms

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate) >= updateInterval) {
            lastUpdate = now;

            // RecvQueue���� �����͸� TempQueue�� �̵�
            std::queue<std::unique_ptr<BasePacket>> TempQueue;

            EnterCriticalSection(&RecvQueueCS);
            while (!RecvQueue.empty()) {
                TempQueue.push(std::move(RecvQueue.front()));
                RecvQueue.pop();
            }
            LeaveCriticalSection(&RecvQueueCS);

            // TempQueue ������ ó��
            while (!TempQueue.empty()) {
                auto packet = std::move(TempQueue.front());
                TempQueue.pop();

                // Input_Packet ó��
                if (packet->packetType == 1) {
                    auto inputPacket = std::unique_ptr<Input_Packet>(static_cast<Input_Packet*>(packet.release()));
                    std::cout << "Input Packet Details:\n";
                    std::cout << "  Player ID: " << inputPacket->m_playerID << "\n";
                    std::cout << "  Input Type: " << static_cast<int>(inputPacket->inputType) << "\n";
                    std::cout << "  Input State: " << static_cast<int>(inputPacket->inputState) << "\n";
                    std::cout << "  Input Time: " << inputPacket->inputTime << "\n";

                    loop.Update(inputPacket.get()); // �Է��� �ִ� ��쿡 ������Ʈ
                }

                // ���� ��Ŷ ���� �� ����
                auto objectPacket = std::make_unique<ObjectInfo_Packet>();
                const auto& objects = ObjectManager::GetInstance().GetObjects();

                for (auto* obj : objects) {
                    if (CPlayer* player = dynamic_cast<CPlayer*>(obj)) {
                        PlayerInfo playerInfo = player->GetPK();
                        std::cout << "Sending Player Info: ID = " << playerInfo.m_playerID
                            << ", X = " << playerInfo.m_x
                            << ", Y = " << playerInfo.m_y
                            << ", Num = " << playerInfo.m_stageNum << std::endl;

                        objectPacket->m_player = playerInfo; // ��Ŷ�� �߰�
                    }
                }

                EnterCriticalSection(&SendQueueCS);
                SendQueue.push(std::move(objectPacket));
                LeaveCriticalSection(&SendQueueCS);
            }
        }

        Sleep(1); // CPU ��뷮 ����
    }

    // ���� ó��
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