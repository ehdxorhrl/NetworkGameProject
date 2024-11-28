#include <winsock2.h>
#include <iostream>
#include <cstdint>
#include <windows.h>
#include <vector>
#include <cstring>
#include "Packet.h"
#pragma comment(lib, "ws2_32.lib")

// �� �÷��̾ ���� ������ CriticalSection
CRITICAL_SECTION playerBufferCS[2];

// �÷��̾� ���� ũ�� �� �ʱ�ȭ
constexpr size_t BUFFER_SIZE = 1024;
char playerBuffers[2][BUFFER_SIZE]; // �÷��̾� 2���� ����
size_t bufferOffsets[2] = { 0, 0 };  // �� ������ ���� ���� ��ġ
int mProcessed[2]{};

size_t GetPacketSize(uint8_t packetType) {
    switch (packetType) {
    case 1: return sizeof(Input_Packet);
    case 11: return sizeof(GTime_Packet);
    case 12: return sizeof(PlayerIDResponsePacket);
    case 13: return sizeof(ObjectInfo_Packet);
    case 101: return sizeof(EndGame_Packet);
    default: return 0; // Unknown packet type
    }
}

template<typename Type>
Type ReadVal(int i)
{
    Type data = {};
    memcpy(&data, playerBuffers[i] + mProcessed[i], sizeof(Type));
    mProcessed[i] += sizeof(Type);
    return data;
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
    auto params = reinterpret_cast<std::pair<SOCKET, int>*>(lpParam);
    SOCKET clientSocket = params->first;
    int playerID = params->second;
    delete params; // ���� �Ҵ� ����

    int sizeReceived;
    int dataReceived;

    while (true) {
        // ������ ����
        recv(clientSocket, (char*)&sizeReceived, sizeof(int), 0);
        dataReceived = recv(clientSocket, playerBuffers[playerID], sizeReceived, 0);

        mProcessed[playerID] = 0;
        while (sizeReceived > mProcessed[playerID])
        {
            switch (ReadVal<short>(playerID))
            {
            case 1:
            {
                auto data = ReadVal<Input_Packet>(playerID);




               // std::cout << "Input = " << (int)data.inputState << ", " << (int)data.inputType << ", " << (int)data.m_playerID << ", " << (int)data.m_scene << ", " << std::endl;
                break;
            }
            case 11:
            {
                auto data = ReadVal<GTime_Packet>(playerID);
                //std::cout << "GTime = " << data.gameTime << std::endl;
                break;
            }
            case 12:
            {
                auto data = ReadVal<PlayerIDResponsePacket>(playerID);
                //std::cout << "PlayerID" << std::endl;
                break;
            }
            case 13:
            {
                ReadVal<ObjectInfo_Packet>(playerID);
                break;
            }
            case 101:
            {
                ReadVal<EndGame_Packet>(playerID);
                break;
            }
            }

        }
    }

    closesocket(clientSocket);
    return 0;
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    // Winsock �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // ���� ���� ����
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // ���� �ּ� ����
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // ���ε�
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // ���� ���
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    // �� �÷��̾ ���� CriticalSection �ʱ�ȭ
    InitializeCriticalSection(&playerBufferCS[0]);
    InitializeCriticalSection(&playerBufferCS[1]);

    int clientCount = 0;
    while (clientCount < 2) { // �� ���� Ŭ���̾�Ʈ�� ���
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "Player " << clientCount << " connected." << std::endl;
        auto* params = new std::pair<SOCKET, int>(clientSocket, clientCount);
        HANDLE hThread = CreateThread(NULL, 0, CommunicationThread, params, 0, NULL);
        if (hThread == NULL) {
            std::cerr << "CreateThread failed: " << GetLastError() << std::endl;
            closesocket(clientSocket);
            delete params;
        }
        clientCount++;
    }

    std::cout << "Game Start" << std::endl;

    while (true)
    {

    }

    // Cleanup
    DeleteCriticalSection(&playerBufferCS[0]);
    DeleteCriticalSection(&playerBufferCS[1]);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
