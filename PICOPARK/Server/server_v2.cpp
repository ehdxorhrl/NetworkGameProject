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
HANDLE startGameEvent;

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

bool AddToBuffer(int playerID, const char* data, size_t dataSize) {
    EnterCriticalSection(&playerBufferCS[playerID]);
    if (bufferOffsets[playerID] + dataSize > BUFFER_SIZE) {
        LeaveCriticalSection(&playerBufferCS[playerID]);
        std::cerr << "Buffer overflow for player " << playerID << std::endl;
        return false;
    }

    memcpy(&playerBuffers[playerID][bufferOffsets[playerID]], data, dataSize);
    bufferOffsets[playerID] += dataSize;
    LeaveCriticalSection(&playerBufferCS[playerID]);
    return true;
}

void ProcessBuffer(int playerID) {
    EnterCriticalSection(&playerBufferCS[playerID]);
    size_t offset = 0;

    while (offset < bufferOffsets[playerID]) {
        uint8_t opCode = playerBuffers[playerID][offset];
        size_t packetSize = GetPacketSize(opCode);

        if (packetSize == 0 || offset + packetSize > bufferOffsets[playerID]) {
            std::cerr << "Invalid or incomplete packet in buffer for player " << playerID << std::endl;
            break;
        }

        // ó�� ���� (��: ��Ŷ �б� �� ó��)
        const char* packetData = &playerBuffers[playerID][offset];
        std::cout << "Processing packet (opCode: " << (int)opCode << ", size: " << packetSize << ") for player " << playerID << std::endl;

        // ó�� �Ϸ� �� ������ �̵�
        offset += packetSize;
    }

    // ó���� ������ ����
    if (offset < bufferOffsets[playerID]) {
        memmove(playerBuffers[playerID], &playerBuffers[playerID][offset], bufferOffsets[playerID] - offset);
    }
    bufferOffsets[playerID] -= offset;
    LeaveCriticalSection(&playerBufferCS[playerID]);
}

template<typename Type>
Type ReadVal(int i)
{
    Type data = {};
    memcpy(&data, playerBuffers[i] + mProcessed[i], sizeof(Type));
    mProcessed[i] += sizeof(Type);
    return data;
}

void SendPackets(SOCKET clientSocket, uint8_t opCode) {
    char sendBuffer[1024] = { 0 };
    size_t offset = 0;

    // ��Ŷ ��� �ۼ� (opCode)
    memcpy(sendBuffer, &opCode, sizeof(opCode));
    offset += sizeof(opCode);

    // opCode�� ���� �߰� ������ �ۼ�
    if (opCode == 11) { // GTime_Packet ����
        GTime_Packet gTimePacket = { 1234 }; // ��: ���� �ð� 1234ms
        memcpy(sendBuffer + offset, &gTimePacket, sizeof(gTimePacket));
        offset += sizeof(gTimePacket);
    }
    else if (opCode == 13) { // ObjectInfo_Packet ����
        ObjectInfo_Packet objectInfo = {
            
        };
        memcpy(sendBuffer + offset, &objectInfo, sizeof(objectInfo));
        offset += sizeof(objectInfo);
    }

    // ��Ŷ �۽�
    if (send(clientSocket, sendBuffer, offset, 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send packet: " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "Packet sent to client (opCode: " << (int)opCode << ")" << std::endl;
    }
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
    auto params = reinterpret_cast<std::pair<SOCKET, int>*>(lpParam);
    SOCKET clientSocket = params->first;
    int playerID = params->second;
    delete params; // ���� �Ҵ� ����

    //char recvBuffer[1024];
    //int bytesReceived;
    int sizeReceived;
    int dataReceived;

    WaitForSingleObject(startGameEvent, INFINITE);
    std::cout << "Game start" << std::endl;

    while (true) {
        // ������ ����
       /* bytesReceived = recv(clientSocket, recvBuffer, sizeof(short) + sizeof(Input_Packet), 0);*/
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
                std::cout << "Input = " << (int)data.inputState << ", " << (int)data.inputType << ", " << (int)data.m_playerID << ", " << (int)data.m_scene << ", " << std::endl;
                break;
            }
            case 11:
            {
                auto data = ReadVal<GTime_Packet>(playerID);
                std::cout << "GTime = " << data.gameTime << std::endl;
                break;
            }
            case 12:
            {
                auto data = ReadVal<PlayerIDResponsePacket>(playerID);
                std::cout << "PlayerID" << std::endl;
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
    
        SendPackets(clientSocket, 11);
        /*if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "recv() failed or connection closed for player " << playerID << ": " << WSAGetLastError() << std::endl;
            break;
        }*/
        //else
        //{
        //    //std::cout << "Success" << std::endl;
        //}

       /* short opcode;
        Input_Packet input_packet;
        memcpy(&opcode, recvBuffer, sizeof(short));
        memcpy(&input_packet, recvBuffer + sizeof(short), sizeof(Input_Packet));

        std::cout << "opcode = " << opcode << std::endl;
        std::cout << "Input Packet = " << (int)input_packet.inputState << ", " << (int)input_packet.inputType << ", " << (int)input_packet.m_playerID << ", " << (int)input_packet.m_scene << std::endl;*/

        //// ���ۿ� ������ �߰�
        //if (!AddToBuffer(playerID, recvBuffer, bytesReceived)) {
        //    break;
        //}

        // ��� ���� ó��
        //ProcessBuffer(playerID);
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

    int clientCount = 0;

    // �� �÷��̾ ���� CriticalSection �ʱ�ȭ
    InitializeCriticalSection(&playerBufferCS[0]);
    InitializeCriticalSection(&playerBufferCS[1]);

    // �̺�Ʈ ��ü ���� (�ʱ� ����: ���ȣ ����)
    startGameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (startGameEvent == NULL) {
        std::cerr << "CreateEvent failed: " << GetLastError() << std::endl;
        return 1;
    }
    
    // Ŭ���̾�Ʈ ��� ����
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        EnterCriticalSection(&playerBufferCS[0]);
        EnterCriticalSection(&playerBufferCS[1]);
        if (clientCount < 2) {
            std::cout << "Player " << clientCount << " connected." << std::endl;

            // ������ ����
            auto* params = new std::pair<SOCKET, int>(clientSocket, clientCount);
            HANDLE hThread = CreateThread(NULL, 0, CommunicationThread, params, 0, NULL);
            if (hThread == NULL) {
                std::cerr << "CreateThread failed: " << GetLastError() << std::endl;
                closesocket(clientSocket);
                delete params;
            }
            //else {
            //    CloseHandle(hThread);
            //}

            clientCount++;
        }
        LeaveCriticalSection(&playerBufferCS[0]);
        LeaveCriticalSection(&playerBufferCS[1]);

        // Ŭ���̾�Ʈ 2�� ���� �Ǹ� ��� ����
        if (clientCount == 2) {
            std::cout << "Both Game Start" << std::endl;
            SetEvent(startGameEvent); // ��� ��� ���� ������ ����
            break;
        }
    }
    

    while (true)
    {

    }

    // Cleanup
    WaitForSingleObject(startGameEvent, INFINITE);
    CloseHandle(startGameEvent);
    DeleteCriticalSection(&playerBufferCS[0]);
    DeleteCriticalSection(&playerBufferCS[1]);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
