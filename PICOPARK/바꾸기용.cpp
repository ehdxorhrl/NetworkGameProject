// 이것도 안쓰는 코드

#include "stdafx.h"

void CGameloop::Init(HWND hWnd)
{
	LoadImages();
	InitializeMaps();

	for (int i = 0; i < NUM_PLAYERS; i++)
		player[i].Init();

	//플레이어 초기값 - 들어온 순서로 맞추기 가능
	for (int i = 0; i < NUM_PLAYERS; i++)
	{
		if (player[i].type == PICO)
		{
			player[i].x = 200;
			player[i].y = 675;
		}
		else
		{
			player[i].x = 600;
			player[i].y = 675;
		}
	}

	//배경음악
	PlaySound(L"배경음악.wav", NULL, SND_ASYNC | SND_LOOP);

	//타이머 설정
	SetTimer(hWnd, 1, 100, NULL);
	SetTimer(hWnd, 2, 10, NULL);
	SetTimer(hWnd, 3, 10, NULL);
	SetTimer(hWnd, 4, 1000, NULL);
}

void CGameloop::ResetStage(Cstage st) {
	if (maps.find(st) == maps.end()) {
		return; // Invalid stage number
	}

	player[0].x = (st == Cstage::Stage2) ? 200 : (st == Cstage::Stage3) ? 75 : 150;
	player[0].y = (st == Cstage::Stage2) ? 675 : (st == Cstage::Stage3) ? 675 : 575;
	player[1].x = (st == Cstage::Stage2) ? 600 : (st == Cstage::Stage3) ? 150 : 250;
	player[1].y = (st == Cstage::Stage2) ? 675 : (st == Cstage::Stage3) ? 675 : 575;
	mapSelection = (st == Cstage::Stage2) ? 0 : (st == Cstage::Stage3) ? 2 : 5;

	// Reset player properties
	for (int i = 0; i < NUM_PLAYERS; i++)
		player[i].Init();

	// Reset map blocks from the loaded maps
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			maps[mapSelection][i][j].x = i * 100;
			maps[mapSelection][i][j].y = j * 100;
			maps[mapSelection][i][j].type = maps[st][i][j].type;
			maps[mapSelection][i][j].select = maps[st][i][j].select;
		}
	}
}

void CGameloop::LoadImages()
{
	// Load player images
	P1Image[0].Load(L"차렷1.jpg");
	P1Image[1].Load(L"이동1.jpg");
	P1Image[2].Load(L"점프1.jpg");
	P1Image[3].Load(L"죽음1.png");

	P2Image[0].Load(L"차렷2.jpg");
	P2Image[1].Load(L"이동2.jpg");
	P2Image[2].Load(L"점프2.jpg");
	P2Image[3].Load(L"죽음2.png");

	// Load block images
	block[0].Load(L"블럭1.png");
	block[1].Load(L"블럭2.png");
	block[2].Load(L"블럭3.png");
	block[3].Load(L"블럭4.png");
	block[4].Load(L"닫은문.png");
	block[5].Load(L"열린문.png");
	block[6].Load(L"빨간스위치O.png");
	block[7].Load(L"빨간스위치X.png");
	block[8].Load(L"초록스위치O.png");
	block[9].Load(L"초록스위치X.png");
	block[10].Load(L"파란스위치O.png");
	block[11].Load(L"파란스위치X.png");
	block[12].Load(L"열쇠.png");
	block[13].Load(L"대포.png");
	block[14].Load(L"총알2.png");
	block[15].Load(L"총알1.png");

	// Load stage images
	stage[0].Load(L"STAGE0.png");
	stage[1].Load(L"STAGE1.png");
	stage[2].Load(L"STAGE2.png");
	stage[3].Load(L"STAGE3.png");
	stage[4].Load(L"STAGE4.png");

	// Load number images
	number[0].Load(L"0.png");
	number[1].Load(L"1.png");
	number[2].Load(L"2.png");
	number[3].Load(L"3.png");
	number[4].Load(L"4.png");
	number[5].Load(L"5.png");
	number[6].Load(L"6.png");
	number[7].Load(L"7.png");
	number[8].Load(L"8.png");
	number[9].Load(L"9.png");
	semicolon.Load(L"세미콜론.png");

	// Load stage time images
	stageTime[0].Load(L"시간1.png");
	stageTime[1].Load(L"시간2.png");
	stageTime[2].Load(L"시간3.png");
}

void CGameloop::InitializeMaps()
{
	LoadAllMapsFromFile("maps.txt", maps);
}

void CGameloop::LoadAllMapsFromFile(const string& filePath, map<int, vector<vector<BLOCK>>>& maps) {
	ifstream file(filePath);
	if (file.is_open()) {
		string line;
		int currentMap = -1;

		while (getline(file, line)) {
			// 구분자(# mapXX)를 찾아서 현재 맵을 설정
			if (line.find("# map") == 0) {
				currentMap = stoi(line.substr(5));
				maps[currentMap] = vector<vector<BLOCK>>(8, vector<BLOCK>(8));
				continue;
			}

			if (currentMap == -1) {
				continue; // 유효한 맵이 설정되지 않았다면 스킵
			}

			// 현재 맵에 데이터 추가
			int x, y, type;
			std::string selectStr;
			bool select;

			istringstream iss(line);
			if (iss >> x >> y >> type >> selectStr) {
				select = (selectStr == "true");
				int i = x / 100;
				int j = y / 100;
				maps[currentMap][i][j].x = x;
				maps[currentMap][i][j].y = y;
				maps[currentMap][i][j].type = type;
				maps[currentMap][i][j].select = select;
			}
		}
		file.close();
	}
}