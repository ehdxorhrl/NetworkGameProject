
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
//	int stageCnt = 0;	//�������� ȭ�� ����ִ� ī��Ʈ
//	bool gravity = false;
//	int TimeCnt0 = 0, TimeCnt1 = 0, TimeCnt2 = 0, TimeCnt3 = 0;
//	bool gameStart = false, gameEnd = false;	//�ʱ�ȭ�� ����Ű ������ Ʈ��
//	bool nextStage = true;
//	int Time1[4], Time2[4], Time3[4];	//Ÿ�� �����ϴ� ����
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
    responsePacket.packetType = 1; // ���� ��Ŷ
    responsePacket.playerID = rand() % 1000; // ������ ID �Ҵ� (����)

    return SendPlayerIDResponse(clientSock, responsePacket);
}

bool SendPlayerIDRequest(SOCKET& sock) {
    PlayerIDRequestPacket requestPacket;
    requestPacket.packetType = 0; // ��û ��Ŷ

    int retval = send(sock, (char*)&requestPacket, sizeof(requestPacket), 0);
    if (retval == SOCKET_ERROR) {
        err_display("SendPlayerIDRequest Error");
        return false;
    }
    return true;
}