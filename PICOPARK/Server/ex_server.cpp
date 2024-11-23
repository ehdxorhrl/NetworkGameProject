#include <winsock2.h>
#include <iostream>
#include <queue>
#include <cstdint>
#include <windows.h>
#include <vector>
#include "Packet.h"
#pragma comment(lib, "ws2_32.lib")

CRITICAL_SECTION recvQueueCS;
CRITICAL_SECTION sendQueueCS;

std::queue<void*> RecvQueue;
std::queue<ObjectInfo_Packet> SendQueue;

size_t GetPacketSize(uint8_t packetType) {
    switch (packetType) {
    case 1: return sizeof(Input_Packet);
   /* case 2: return sizeof(PlayerIDRequest_Packet);*/
    case 11: return sizeof(GTime_Packet);
    case 12: return sizeof(PlayerIDResponsePacket);
    case 13: return sizeof(ObjectInfo_Packet);
    case 101: return sizeof(EndGame_Packet);
    default: return 0; // Unknown packet type
    }
}

void* ParsePacket(const char* buffer, uint8_t packetType) {
    void* packet = nullptr;
    switch (packetType) {
    case 1: {
        packet = new Input_Packet;
        memcpy(packet, buffer, sizeof(Input_Packet));
        break;
    }
  /*  case 2: {
        packet = new PlayerIDRequest_Packet;
        memcpy(packet, buffer, sizeof(PlayerIDRequest_Packet));
        break;
    }*/
    case 11: {
        packet = new GTime_Packet;
        memcpy(packet, buffer, sizeof(GTime_Packet));
        break;
    }
    case 12: {
        packet = new PlayerIDResponsePacket;
        memcpy(packet, buffer, sizeof(PlayerIDResponsePacket));
        break;
    }
    case 13: {
        packet = new ObjectInfo_Packet;
        memcpy(packet, buffer, sizeof(ObjectInfo_Packet));
        break;
    }
    case 101: {
        packet = new EndGame_Packet;
        memcpy(packet, buffer, sizeof(EndGame_Packet));
        break;
    }
    default: {
        std::cerr << "Unknown packet type: " << (int)packetType << std::endl;
        break;
    }
    }
    return packet;
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
    SOCKET clientSocket = (SOCKET)lpParam;
    char recvBuffer[1024];
    int bytesReceived;

    while (true) {
        // Receive data
        bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "recv() failed or connection closed: " << WSAGetLastError() << std::endl;
            break;
        }

        // Determine packet type
        uint8_t packetType = recvBuffer[0];
        size_t packetSize = GetPacketSize(packetType);
        if (packetSize == 0 || bytesReceived < packetSize) {
            std::cerr << "Invalid or incomplete packet received." << std::endl;
            continue;
        }

        // Parse the packet
        void* packet = ParsePacket(recvBuffer, packetType);
        if (packet) {
            EnterCriticalSection(&recvQueueCS);
            RecvQueue.push(packet);
            LeaveCriticalSection(&recvQueueCS);
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

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    // Initialize critical sections
    InitializeCriticalSection(&recvQueueCS);
    InitializeCriticalSection(&sendQueueCS);

    // Accept client connections and create communication threads
    int clientCount = 0;
    while (clientCount < 2) { // Limit to 2 clients
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "Client connected." << std::endl;
        HANDLE hThread = CreateThread(NULL, 0, CommunicationThread, (LPVOID)clientSocket, 0, NULL);
        if (hThread == NULL) {
            std::cerr << "CreateThread failed: " << GetLastError() << std::endl;
            closesocket(clientSocket);
        }
        else {
            CloseHandle(hThread);
        }
        clientCount++;
    }

    // 플레이어 ID Send()


    // Main update loop
    while (true) {
        std::queue<void*> TempQueue;

        EnterCriticalSection(&recvQueueCS);
        while (!RecvQueue.empty()) {
            TempQueue.push(RecvQueue.front());
            RecvQueue.pop();
        }
        LeaveCriticalSection(&recvQueueCS);

        // Process TempQueue and create updates
        while (!TempQueue.empty()) {
            void* packet = TempQueue.front();
            TempQueue.pop();
            // Process packet (update logic to be implemented)
            delete packet; // Free allocated memory after processing
        }

        // Add processed data to SendQueue if needed
        EnterCriticalSection(&sendQueueCS);
        // Populate SendQueue with response packets (update logic to be implemented)
        LeaveCriticalSection(&sendQueueCS);

        // Simulate a small delay to avoid busy-waiting
        Sleep(50);
    }

    // Cleanup
    DeleteCriticalSection(&recvQueueCS);
    DeleteCriticalSection(&sendQueueCS);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
