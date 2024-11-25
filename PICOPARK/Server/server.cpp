#include <iostream>
#include <queue>
#include <cstdint>
#include <winsock2.h>
#include <windows.h>
#include <vector>
#include "Packet.h"

// Include necessary structs and enums here

CRITICAL_SECTION recvQueueCS;
CRITICAL_SECTION sendQueueCS;

std::queue<Input_Packet> RecvQueue;
std::queue<ObjectInfo_Packet> SendQueue;

void SendStartGame(SOCKET& sock, bool isGameStarted)
{

}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
    SOCKET clientSocket = (SOCKET)lpParam;
    char recvBuffer[1024];
    char sendBuffer[1024];
    int bytesReceived, bytesSent;

    while (true) {
        // Receive data
        {
            EnterCriticalSection(&recvQueueCS);
            bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            LeaveCriticalSection(&recvQueueCS);

            if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
                std::cerr << "recv() failed or connection closed: " << WSAGetLastError() << std::endl;
                break;
            }

            // Process received packet and push to RecvQueue
            Input_Packet packet;
            memcpy(&packet, recvBuffer, sizeof(Input_Packet));

            EnterCriticalSection(&recvQueueCS);
            RecvQueue.push(packet);
            LeaveCriticalSection(&recvQueueCS);
        }

        // Send data
        {
            EnterCriticalSection(&sendQueueCS);
            if (!SendQueue.empty()) {
                ObjectInfo_Packet packet = SendQueue.front();
                SendQueue.pop();
                LeaveCriticalSection(&sendQueueCS);

                memcpy(sendBuffer, &packet, sizeof(ObjectInfo_Packet));
                bytesSent = send(clientSocket, sendBuffer, sizeof(ObjectInfo_Packet), 0);
                if (bytesSent == SOCKET_ERROR) {
                    std::cerr << "send() failed: " << WSAGetLastError() << std::endl;
                    break;
                }
            }
            else {
                LeaveCriticalSection(&sendQueueCS);
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

    // Main update loop
    while (true) {
        std::queue<Input_Packet> TempQueue;

        // Move data from RecvQueue to TempQueue
        EnterCriticalSection(&recvQueueCS);
        while (!RecvQueue.empty()) {
            TempQueue.push(RecvQueue.front());
            RecvQueue.pop();
        }
        LeaveCriticalSection(&recvQueueCS);

        // Process TempQueue and create updates
        while (!TempQueue.empty()) {
            Input_Packet packet = TempQueue.front();
            TempQueue.pop();
            // Process packet (update logic to be implemented)
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
