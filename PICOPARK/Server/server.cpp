#include "stdafx.h"
#include <winsock2.h>
#include <iostream>
#include <queue>
#include <memory>
#include <chrono>
#include <windows.h>
#include "CGameloop.h"
#include "CPlayer.h"
#include "Stage1.h"
#include "Stage2.h"
#include "Stage3.h"


#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 6112
#define MAX_CLIENTS 2
#define BUFFER_SIZE 512

CGameloop loop;

void SendPlayerID(SOCKET clientSocket, uint32_t playerID);

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

        auto packet = std::make_unique<Input_Packet>();
        memcpy(packet.get(), buffer, sizeof(Input_Packet));

        EnterCriticalSection(&RecvQueueCS);
        RecvQueue.push(std::move(packet));
        LeaveCriticalSection(&RecvQueueCS);

        EnterCriticalSection(&SendQueueCS);
        while (!SendQueue.empty()) {
            auto sendPacket = std::move(SendQueue.front());
            SendQueue.pop();
            LeaveCriticalSection(&SendQueueCS);

            // 모든 클라이언트에게 동일한 패킷 전송
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clientSockets[i] != INVALID_SOCKET) {
                    int sendResult = send(clientSockets[i], reinterpret_cast<char*>(sendPacket.get()), sizeof(ObjectInfo_Packet), 0);
                    if (sendResult == SOCKET_ERROR) {
                        std::cerr << "Failed to send packet to Client ID " << i << ": " << WSAGetLastError() << "\n";
                    }
                    else {
                        std::cout << "Packet sent to Client ID " << i << "\n";
                    }
                }
            }

            EnterCriticalSection(&SendQueueCS);
        }
        LeaveCriticalSection(&SendQueueCS);
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

        uint32_t playerID = static_cast<uint32_t>(clientCount); // 간단히 clientCount를 ID로 사용
        SendPlayerID(clientSocket, playerID);


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
    const std::chrono::milliseconds updateInterval(16); // 초당 60fps

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
                    loop.Update(inputPacket.get()); // 입력이 있는 경우에 업데이트
                }

                // 상태 패킷 생성 및 전송
                auto objectPacket = std::make_unique<ObjectInfo_Packet>();
                const auto& objects = ObjectManager::GetInstance().GetObjects();

                int index = 0;
                for (auto* obj : objects) {
                    if (CPlayer* player = dynamic_cast<CPlayer*>(obj)) {
                        objectPacket->m_player[index] = player->GetPK();                    
                        index++;
                    }
                }
                CScene* currentScene = SceneManager::GetInstance().GetCurrentScene();
                objectPacket->changescene = currentScene->IsSceneChangeRequired();
                if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage1)
                {
                    Stage1* stage1 = dynamic_cast<Stage1*>(currentScene);
                    if (stage1->IsDoorOpen())
                        objectPacket->openthedoor = true;
                    else
                        objectPacket->openthedoor = false;
                }
                else if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage2) {
                    Stage2* stage2 = dynamic_cast<Stage2*>(currentScene);
                    if (stage2->IsDoorOpen())
                        objectPacket->openthedoor = true;
                    else
                        objectPacket->openthedoor = false;
                }
                else if (SceneManager::GetInstance().GetSceneType() == SceneType::Stage3) {
                    Stage3* stage3 = dynamic_cast<Stage3*>(currentScene);
                    if (stage3->IsDoorOpen())
                        objectPacket->openthedoor = true;
                    else
                        objectPacket->openthedoor = false;

                    if(stage3->IsSwitchPressed())
                        objectPacket->switchpress = true;
                    else 
                        objectPacket->switchpress = false;
                }
                EnterCriticalSection(&SendQueueCS);               
                SendQueue.push(std::move(objectPacket));                
                LeaveCriticalSection(&SendQueueCS);
            }
        }
    }

    // 종료 처리
    for (int i = 0; i < clientCount; ++i) {
        if (communicationThreads[i]) {
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


void SendPlayerID(SOCKET clientSocket, uint32_t playerID) {
    PlayerIDResponsePacket idResponse;
    idResponse.m_playerID = playerID;
    idResponse.isSuccess = true;

    // Player ID 패킷 전송
    int sendResult = send(clientSocket, reinterpret_cast<char*>(&idResponse), sizeof(PlayerIDResponsePacket), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "Failed to send Player ID packet: " << WSAGetLastError() << "\n";
    }
}