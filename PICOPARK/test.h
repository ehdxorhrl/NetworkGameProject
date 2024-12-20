
//#include "CGameloop.h"
//class CGameloop
//{
//public:
//	CGameloop() : times{ Time1, Time2, Time3 } {}
//	void Init(HWND hWnd);
//	void ResetStage(Cstage st);
//	void LoadImages();
//	void InitializeMaps();
//	void LoadAllMapsFromFile(const string& filePath, map<int, vector<vector<BLOCK>>>& maps);
//	Cstage getStage() const { return Stage; }
//private:
//	PLAYER player[2];
//	vector<BULLET> bullets{};
//	vector<BULLET> cannonBullets{};
//	map<int, vector<vector<BLOCK>>> maps;
//	CImage P1Image[4], P2Image[4], block[BLOCKCNT], map[5], number[10], stageTime[3], semicolon;
//	Cstage Stage;	//0 = stage[0], 1 = stage[1], 2 = stage[2], 3 = stage[3], 4=stage[4]
//	int mapSelection = 0;	//0=11, 1=12, 2=21, 3=22, 4=23, 5=31, 6=32, 7=33
//	int stageCnt = 0;	//스테이지 화면 띄워주는 카운트
//	bool gravity = false;
//	int TimeCnt0 = 0, TimeCnt1 = 0, TimeCnt2 = 0, TimeCnt3 = 0;
//	bool gameStart = false, gameEnd = false;	//초기화면 엔터키 누르면 트루
//	bool nextStage = true;
//	int Time1[4], Time2[4], Time3[4];	//타임 저장하는 변수
//	bool Next = false;
//	bool timeStart = false;
//	int CBcnt = 0, CBcntcnt = 0;
//	array<int*, 3> times;
//};
//
//
//enum class Cstage {
//	Stage0 = 0,
//	Stage1,
//	Stage2,
//	Stage3,
//	Stage4
//};
//
//enum CMap
//{
//	Map1_1 = 0,
//	Map1_2,
//	Map2_1,
//	Map2_2,
//	Map2_3,
//	Map3_1,
//	Map3_2
//};



bool HandlePlayerIDRequest(SOCKET& clientSock) {
    PlayerIDResponsePacket responsePacket;
    responsePacket.packetType = 1; // 응답 패킷
    responsePacket.playerID = rand() % 1000; // 임의의 ID 할당

    return SendPlayerIDResponse(clientSock, responsePacket);
}

bool SendPlayerIDRequest(SOCKET& sock) {
    PlayerIDRequestPacket requestPacket;
    requestPacket.packetType = 0; // 요청 패킷

    int retval = send(sock, (char*)&requestPacket, sizeof(requestPacket), 0);
    if (retval == SOCKET_ERROR) {
        err_display("SendPlayerIDRequest Error");
        return false;
    }
    return true;
}

bool HandlePlayerIDResponsePacket(const PlayerIDResponsePacket& packet) {
    if (packet.packetType != 1) {
        printf("Invalid packet type\\n");
        return false;
    }
    printf("Received Player ID: %d\\n", packet.playerID);
    return true;
}

bool SendPlayerIDRequest(SOCKET& sock) {
    PlayerIDRequestPacket requestPacket;
    requestPacket.packetType = 0; // 요청 패킷

    int retval = send(sock, (char*)&requestPacket, sizeof(requestPacket), 0);
    if (retval == SOCKET_ERROR) {
        err_display("SendPlayerIDRequest Error");
        return false;
    }
    return true;
}

bool SendPacket(SOCKET& sock, const void* packetData, int packetSize) {
    if (sock == INVALID_SOCKET || packetData == nullptr || packetSize <= 0) {
        cerr << "SendPacket: Invalid parameters" << endl;
        return false;
    }

    int totalBytesSent = 0;
    const char* dataPtr = static_cast<const char*>(packetData);

    while (totalBytesSent < packetSize) {
        int bytesSent = send(sock, dataPtr + totalBytesSent, packetSize - totalBytesSent, 0);
        if (bytesSent == SOCKET_ERROR) {
            cerr << "SendPacket: Error during send() - " << WSAGetLastError() << endl;
            return false;
        }
        totalBytesSent += bytesSent;
    }

    return true;
}

bool ReceivePacket(SOCKET& sock, void* buffer, int bufferSize)
{
    if (buffer == nullptr || bufferSize <= 0)
    {
        std::cerr << "Invalid buffer or buffer size." << std::endl;
        return false;
    }

    int totalBytesReceived = 0; // 총 수신된 바이트 수
    int bytesReceived = 0;

    while (totalBytesReceived < bufferSize)
    {
        // 수신 호출
        bytesReceived = recv(sock, static_cast<char*>(buffer) + totalBytesReceived, bufferSize - totalBytesReceived, 0);

        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Error receiving data. Error Code: " << WSAGetLastError() << std::endl;
            return false; // 오류 발생
        }

        if (bytesReceived == 0)
        {
            std::cerr << "Connection closed by peer." << std::endl;
            return false; // 연결 종료
        }

        totalBytesReceived += bytesReceived;
    }

    return true; // 성공적으로 패킷 수신
}

bool SendGTimePacket(SOCKET& sock, const GTime_Packet& gTimePacket) {
    // 패킷 데이터를 버퍼로 복사
    char buffer[sizeof(GTime_Packet)];
    memcpy(buffer, &gTimePacket, sizeof(GTime_Packet));

    // 데이터 전송
    int bytesSent = send(sock, buffer, sizeof(GTime_Packet), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "send() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    // 전송 성공
    return true;
}

bool SetSocketTimeout(SOCKET& sock, int timeoutInSeconds) {
    // 타임아웃 값을 밀리초 단위로 변환
    int timeoutInMillis = timeoutInSeconds * 1000;

    // recv() 타임아웃 설정
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis)) == SOCKET_ERROR) {
        std::cerr << "Failed to set recv timeout: " << WSAGetLastError() << std::endl;
        return false;
    }

    // send() 타임아웃 설정
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeoutInMillis, sizeof(timeoutInMillis)) == SOCKET_ERROR) {
        std::cerr << "Failed to set send timeout: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

bool HandleNetworkError(int errorCode) {
    std::string errorMessage;

    // 오류 코드에 따른 메시지 처리
    switch (errorCode) {
    case WSAEWOULDBLOCK:
        errorMessage = "Non-blocking operation could not complete immediately.";
        break;
    case WSAECONNRESET:
        errorMessage = "Connection reset by peer.";
        break;
    case WSAETIMEDOUT:
        errorMessage = "Connection timed out.";
        break;
    case WSAENETDOWN:
        errorMessage = "Network is down.";
        break;
    case WSAENOTCONN:
        errorMessage = "Socket is not connected.";
        break;
    case WSAECONNABORTED:
        errorMessage = "Software caused connection abort.";
        break;
    case WSAEADDRINUSE:
        errorMessage = "Address already in use.";
        break;
    default:
        errorMessage = "Unknown network error: " + std::to_string(errorCode);
        break;
    }

    // 오류 메시지 출력
    std::cerr << "Network Error [" << errorCode << "]: " << errorMessage << std::endl;

    // 오류 처리 결과 반환
    // true: 복구 가능한 오류, false: 복구 불가능한 오류
    switch (errorCode) {
    case WSAEWOULDBLOCK: // 비차단 상태에서 발생하는 정상적인 상황
        return true;
    case WSAECONNRESET:  // 연결이 리셋된 경우
    case WSAETIMEDOUT:   // 타임아웃 발생
    case WSAENETDOWN:    // 네트워크 다운
    case WSAECONNABORTED:// 연결 중단
    case WSAENOTCONN:    // 소켓이 연결되지 않음
    case WSAEADDRINUSE:  // 주소 사용 중
        return false;    // 복구 불가능한 오류
    default:
        return false;    // 알 수 없는 오류는 복구 불가로 간주
    }
}

//// 서버로부터 데이터 수신
        //int bytesReceived = recv(g_serverSocket, buffer, sizeof(buffer) - 1, 0);
        //if (bytesReceived > 0) {
        //    buffer[bytesReceived] = '\0';  // 수신된 데이터에 null-terminator 추가
        //
        //    if (strcmp(buffer, "START") == 0) {
        //        auto* currentScene = dynamic_cast<LoadingScene*>(SceneManager::GetInstance().GetCurrentScene());
        //        if (currentScene) {
        //            currentScene->SetServerSignalReceived(true); // 서버 신호 전달
        //        }
        //    }
        //    //loop.UpdateGameState(buffer);  // 수신된 데이터를 바로 게임 상태에 반영
        //}
        //else if (bytesReceived == 0 || bytesReceived == SOCKET_ERROR) {
        //    break;  // 서버 연결 끊김 또는 오류 발생 시 종료
        //}
        //
        //// SendQueue 없이 바로 데이터를 서버로 전송
        ////std::string dataToSend = loop.GetDataToSend();  // 전송할 데이터를 게임 로직에서 가져옴
        ////if (!dataToSend.empty()) {
        ////    send(g_serverSocket, dataToSend.c_str(), dataToSend.size(), 0);
        ////}