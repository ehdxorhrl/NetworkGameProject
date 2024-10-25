// 안쓰는 코드
#include "stdafx.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Program gimal project";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL collision(float a_x1, float a_y1, float a_x2, float a_y2, float b_x1, float b_y1, float b_x2, float b_y2);
BOOL Collision(int x1, int y1, int x2, int y2, int X1, int Y1, int X2, int Y2);
BOOL Collision(int playerX1, int playerY1, int playerX2, int playerY2, int blockX1, int blockY1, int blockX2, int blockY2);

void checkKey();

// 내가 수정한 코드 함수
void UpdateByPlayer();
void UpdatePlayerAnimation(PLAYER player[], int playerCount);
void LoadImages();
void LoadAllMapsFromFile(const string& filePath, map<int, vector<vector<BLOCK>>>& maps);
void InitializeMaps();
void ResetStage(int stageNum);
void resetTimers();
void updatePlayersPosition(int p0_x, int p0_y, int p1_x, int p1_y);
void updateTimers(int timeArray[]);
void drawBlock(int mapType, int x, int y, int type, HDC mdc);
void drawSpecialBlocks(int mapSelection, HDC mdc);
void processMapSelection(int mapSelection, int i, int j, HDC mdc);
void handleCollisionWithBlocks(BULLET& bullet, vector<vector<BLOCK>>& map);
void handlePlayerCollision(BULLET& bullet, PLAYER& player);
void fireCannon(int& CBcntcnt, int& CBcnt, std::vector<BULLET>& cannonBullets, std::vector<std::vector<BLOCK>>& map);
void handlePlayerShooting(PLAYER& player, std::vector<BULLET>& bullets, int& bulletCnt);
void updateShooting(PLAYER& player);
void handleDoorOpen(PLAYER& player, BLOCK& doorBlock);
void handleSwitchPress(PLAYER& player, BLOCK& switchBlock, bool increaseSize);
void handleKeyPickup(PLAYER& player, BLOCK& keyBlock, vector<BLOCK> blocksToActivate);
void checkPlayerGround(PLAYER& player, const std::vector<std::vector<BLOCK>>& map);
void triggerGimmick(int P_ID);
void processMapCollision(PLAYER* players, const std::vector<std::vector<BLOCK>>& map);
void handleCollision(PLAYER& player, const BLOCK& block);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	CGameloop loop;
	g_hInst = hInstance;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_HAND);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_QUESTION);
	RegisterClassEx(&WndClass);
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		0, 0, MAPSIZE, MAPSIZE, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//메시지 루프
	while(Message.message!=WM_QUIT){
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else {
			loop.Update();
			loop.Render();
		}
	}
	return Message.wParam;
}

PLAYER player[2];

vector<BULLET> bullets(333);//
vector<BULLET> cannonBullets(333);//
map<int, vector<vector<BLOCK>>> maps;

BOOL keyStates[256];

//---------------------------------
CGameloop gameloop{};
CImage P1Image[4], P2Image[4], block[BLOCKCNT], stage[5], number[10], stageTime[3], semicolon;
int bulletCnt = 0;
int mapSelection = 0;	//0=11, 1=12, 2=21, 3=22, 4=23, 5=31, 6=32, 7=33
bool gravity = false;
int TimeCnt0 = 0, TimeCnt1 = 0, TimeCnt2 = 0, TimeCnt3 = 0;
bool gameStart = false, gameEnd = false;	//초기화면 엔터키 누르면 트루
int stageNum = 0;	//0 = stage[0], 1 = stage[1], 2 = stage[2], 3 = stage[3], 4=stage[4]
int stageCnt = 0;	//스테이지 화면 띄워주는 카운트
bool nextStage = true;
int Time1[4], Time2[4], Time3[4];	//타임 저장하는 변수
bool Next = false;
bool timeStart = false;
int CBcnt = 0,CBcntcnt = 0;
int* times[] = { Time1, Time2, Time3 };
//---------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN hPen, oldPen;
	HBRUSH hBrush, oldBrush;

	HDC mdc;
	HBITMAP hBitmap;
	RECT rt;
	//------------------------	

	switch (iMsg) {
	case WM_CREATE: {
		gameloop.Init(hWnd);
		break;
	}
	case WM_KEYDOWN:
		keyStates[wParam] = TRUE;
		switch (LOWORD(wParam)) 
		{
			case VK_RETURN: {
				if (stageNum == 0) gameStart = true;
				break;
			}
			case VK_TAB: {
				if (stageNum != 0) nextStage = true;
				break;
			}
			case VK_CONTROL: {
				gameloop.ResetStage();
				break;
			}
		}
		break;
	case WM_KEYUP:
		keyStates[wParam] = FALSE;
		break;
	case WM_COMMAND:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER:
		switch (wParam){
			case 1: {
				//이미지 세기
				UpdatePlayerAnimation(player, NUM_PLAYERS);
				break; }
			case 2: {
				if (!nextStage && gameStart)
				{
					//키 받기
					checkKey();
					UpdateByPlayer();

					//스테이지 넘어가기
					if (player[0].exitAll && player[1].exitAll)
					{
						player[0].exitAll = false;
						player[1].exitAll = false;
						player[0].size = 25;
						player[1].size = 25;
						nextStage = true;
					}


					//중력

					for (int i = 0; i < NUM_PLAYERS; i++) 
					{
						player[i].isOnGround = false;
						checkPlayerGround(player[i], maps[mapSelection]);
					}

					//날고있는지 체크
					/*player[0].isFlying = false;
					player[1].isFlying = false;
					if (!player[0].isOnGround && (player[0].isjump || player[0].isjumpDown))
					{
						player[0].isFlying = true;
					}
					if (!player[1].isOnGround && (player[1].isjump || player[1].isjumpDown))
					{
						player[1].isFlying = true;
					}*/

					//점프 중일때 isOnGround를 true로
					for (int i = 0; i < NUM_PLAYERS; i++)
					{
						// 점프 상태에 따라 isOnGround 설정
						if (player[i].isjump || player[i].isjumpDown)
							player[i].isOnGround = true;

						// 맵을 벗어났을 때 isOnGround 설정
						if (player[i].x > 800 || player[i].x < 0)
							player[i].isOnGround = true;
					}

					//플레이어끼리 점프 충돌체크
					/*if (player[0].isjump && Collision(player[0].x1, player[0].y1 - 5, player[0].x2, player[0].y2 - 5, player[1].x1, player[1].y1, player[1].x2, player[1].y2))
					{
						player[0].isjumpCnt = 30;
					}
					if (player[0].isjumpDown && Collision(player[0].x1, player[0].y1 + 5, player[0].x2, player[0].y2 + 5, player[1].x1, player[1].y1, player[1].x2, player[1].y2))
					{
						player[0].isjumpDownCnt = 30;
						player[0].isOnGround = true;
					}
					if (player[1].isjump && Collision(player[1].x1, player[1].y1 - 5, player[1].x2, player[1].y2 - 5, player[0].x1, player[0].y1, player[0].x2, player[0].y2))
					{
						player[1].isjumpCnt = 30;
					}
					if (player[1].isjumpDown && Collision(player[1].x1, player[1].y1 + 5, player[1].x2, player[1].y2 + 5, player[0].x1, player[0].y1, player[0].x2, player[0].y2))
					{
						player[1].isjumpDownCnt = 30;
						player[1].isOnGround = true;
					}*/
					//플레이어 겹침

					for (int i = 0; i < NUM_PLAYERS; i++) {
						int otherPlayer = (i == 0) ? 1 : 0;  // 두 번째 플레이어 인덱스 설정

						// 수직으로 겹치는지 확인
						if (player[otherPlayer].y1 <= player[i].y2 && player[i].y2 < player[otherPlayer].y2) {

							// 수평으로 겹치는지 확인 (x값만 사용)
							if (player[i].x1 < player[otherPlayer].x2 && player[i].x2 > player[otherPlayer].x1) {

								// y2 좌표가 겹칠 때 위치 조정
								if (player[i].y2 > player[otherPlayer].y1) {
									player[i].y = player[otherPlayer].y - player[otherPlayer].size - player[i].size;
								}

								// 점프 상태에 따른 처리
								if (player[i].y2 == player[otherPlayer].y1 && (player[i].isOnGround || player[i].isjumpDown)) {
									player[i].y -= 5;
								}
							}
						}
					}
						
					//중력 적용
					if (!player[0].isOnGround)
					{
						player[0].y += 5;
					}
					if (!player[1].isOnGround)
					{
						player[1].y += 5;
					}

					//블럭 + 블럭 양옆 충돌체크
					processMapCollision(player, maps[mapSelection]);

					//화면 넘기기(스테이지)
					//0=11, 1=12, 2=21, 3=22, 4=23, 5=31, 6=32, 7=33
					//stageNum + 1
				
					if (player[0].x > 800 && player[1].x > 800)
					{
						if (stageNum == 2)
						{
							if (mapSelection < 1)
							{
								mapSelection++;
								player[0].x = 60;
								player[1].x = 0;
							}
							else
							{
								player[0].x = 800;
								player[1].x = 740;
							}
						}
						if (stageNum == 3)
						{
							if (mapSelection < 4)
							{
								mapSelection++;
								player[0].x = 60;
								player[1].x = 0;
							}
							else
							{
								player[0].x = 800;
								player[1].x = 740;
							}
						}
						if (stageNum == 4)
						{
							if (mapSelection < 7)
							{
								mapSelection++;
								player[0].x = 60;
								player[1].x = 0;
							}
							else
							{
								player[0].x = 800;
								player[1].x = 740;
							}
						}
					}
					if (player[0].x < 0 && player[1].x < 0)
					{
						if (stageNum == 2)
						{
							if (mapSelection >0)
							{
								mapSelection--;
								player[0].x = 800;
								player[1].x = 740;
							}
							else
							{
								player[0].x = 60;
								player[1].x = 0;
							}
						}
						if (stageNum == 3)
						{
							if (mapSelection > 2)
							{
								mapSelection--;
								player[0].x = 800;
								player[1].x = 740;
							}
							else
							{
								player[0].x = 60;
								player[1].x = 0;
							}
						}
						if (stageNum == 4)
						{
							if (mapSelection > 5)
							{
								mapSelection--;
								player[0].x = 800;
								player[1].x = 740;
							}
							else
							{
								player[0].x = 60;
								player[1].x = 0;
							}
						}
					}

				

					//플레이어 점프
					for (int i = 0; i < 2; i++)
					{
						if (player[i].isjump)
						{
							player[i].y -= 5;
							player[i].isjumpCnt++;
							if (player[i].isjumpCnt >= 30)
							{
								player[i].isjumpCnt = 0;
								player[i].isjump = false;
								player[i].isjumpDown = true;
							}
						}
						if (player[i].isjumpDown)
						{
							player[i].y += 5;
							player[i].isjumpDownCnt++;
							if (player[i].isjumpDownCnt >= 30)
							{
								player[i].isjumpDownCnt = 0;
								player[i].isjumpDown = false;
							}
						}
					}
				}
				break;}
			case 3: {
				if (mapSelection == 5) {
					fireCannon(CBcntcnt, CBcnt, cannonBullets, maps[5]);
				}
				else {
					for (auto& cannon : cannonBullets) {
						cannon.select = false;  // 대포알 비활성화
					}
				}

				// 플레이어 총알 발사 처리
				for (int i = 0; i < 2; i++) {
					handlePlayerShooting(player[i], bullets, bulletCnt);
				}

				// 총알 이동 처리
				for (auto& b : bullets) {
					if (b.select) {
						b.x += (b.direction == 1 ? -5 : 5);  // 방향에 따라 이동
						if (b.x < 0 || b.x > 800) {
							b.select = false;  // 화면 밖으로 나가면 비활성화
						}
						b.x1 = b.x - b.size;
						b.y1 = b.y - b.size;
						b.x2 = b.x + b.size;
						b.y2 = b.y + b.size;
					}
				}

				// 플레이어 발사 상태 업데이트
				for (int i = 0; i < 2; i++) {
					updateShooting(player[i]);
				}

				// 충돌 처리 (블록과 충돌)
				if (mapSelection == 5) {
					for (auto& b : cannonBullets) {
						handleCollisionWithBlocks(b, maps[5]);  // mapSelection이 5일 때, map[5]에서 충돌 처리
					}
					for (auto& b : bullets) {
						handleCollisionWithBlocks(b, maps[5]);  // 플레이어 총알도 같은 맵에서 충돌 처리
					}
				}
				else if (mapSelection == 6) {
					for (auto& b : bullets) {
						handleCollisionWithBlocks(b, maps[6]);  // 다른 맵 (예: map[6])에서 충돌 처리
					}
				}

				// 플레이어와 총알 충돌 처리
				for (auto& b : cannonBullets) {
					for (int i = 0; i < 2; i++) {
						handlePlayerCollision(b, player[i]);  // 대포알과 플레이어 충돌 처리
					}
				}
			}
			case 4: {
				//스테이지 변경
				if (nextStage) {
					if (gameStart && stageNum == 0) stageNum++;
					if (stageNum == 4) {
						gameEnd = true;
						Next = true;
					}
					else if (stageNum != 0) {
						timeStart = false;
						if (++stageCnt == 3) {
							player[0].size = player[1].size = 25;
							stageCnt = 0;
							if (stageNum < 4) {
								stageNum++;
								Next = true;
								timeStart = true;
							}
							nextStage = false;
						}
					}
				}
				//카운트 초기화, 맵 설정, 좌표 설정
				if (Next) {
					resetTimers();
					if (stageNum == 2) {
						mapSelection = 0;
					}
					else if (stageNum == 3) {
						updatePlayersPosition(75, 675, 150, 675);
						mapSelection = 2;
					}
					else if (stageNum == 4 && !gameEnd) {
						updatePlayersPosition(150, 575, 250, 575);
						mapSelection = 5;
					}
					Next = false;
				}

				//카운트
				if (timeStart) {
					if (stageNum == 2) {
						updateTimers(Time1);
					}
					else if (stageNum == 3) {
						updateTimers(Time2);
					}
					else if (stageNum == 4 && !gameEnd) {
						updateTimers(Time3);
					}
				}
				break; }
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		GetClientRect(hWnd, &rt);
		hdc = BeginPaint(hWnd, &ps);
		mdc = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
		SelectObject(mdc, (HBITMAP)hBitmap);

		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		hBrush = CreateSolidBrush(RGB(255, 255, 255));

		oldPen = (HPEN)SelectObject(mdc, hPen);
		oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
		Rectangle(mdc, 0, 0, rt.right, rt.bottom);
		//----------------------------------------------------------------------
		
		//맵1-1
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				processMapSelection(mapSelection, i, j, mdc);
			}
		}
		for (int i = 0; i < 2; i++) {
			if (!player[i].isdeath) {
				int imageIndex = player[i].isjump ? 2 : player[i].imageCnt;
				(i == 0 ? P1Image : P2Image)[imageIndex].Draw(mdc, player[i].x1, player[i].y1, player[i].size * 2, player[i].size * 2, 0, 0, 100, 100);
			}
			else {
				(i == 0 ? P1Image : P2Image)[3].Draw(mdc, player[i].x1, player[i].y1, player[i].size * 2, player[i].size * 2, 0, 0, 100, 100);
			}
		}

		//총알 출력 //대포 총알 출력
		for(auto& bullet : bullets)
			if (bullet.select)
				block[15].Draw(mdc, bullet.x1, bullet.y1, 20, 20, 0, 0, 100, 100);
		for (auto& cannonBullet : cannonBullets)
			if (cannonBullet.select)
				block[14].Draw(mdc, cannonBullet.x1, cannonBullet.y1, 50, 50, 0, 0, 100, 100);

		//열쇠잡기 출력
		for (int i = 0; i < 2; i++) {
			if (player[i].haveKey)
				block[12].Draw(mdc, player[i].x1 - 25, player[i].y1 - 25, 25, 25, 0, 0, 100, 100);
		}

		//시간 출력
		if (stageNum >= 2 && stageNum <= 4) {
			int* currentTime = times[stageNum - 2];
			number[currentTime[0]].Draw(mdc, 275, 0, 50, 50, 0, 0, 100, 100);
			number[currentTime[1]].Draw(mdc, 325, 0, 50, 50, 0, 0, 100, 100);
			semicolon.Draw(mdc, 375, 0, 50, 50, 0, 0, 100, 100);
			number[currentTime[2]].Draw(mdc, 425, 0, 50, 50, 0, 0, 100, 100);
			number[currentTime[3]].Draw(mdc, 475, 0, 50, 50, 0, 0, 100, 100);
		}

		//스테이지화면 출력
		if (nextStage)
			stage[stageNum].Draw(mdc, 0, 0, MAPSIZE, MAPSIZE, 0, 0,800, 800);

		//엔딩 타임 출력
		if (gameEnd) {
			int yPos[] = { 425, 525, 625 };

			for (int i = 0; i < 3; i++) {
				// Draw stage time background
				stageTime[i].Draw(mdc, 125, 400 + (i * 100), 200, 100, 0, 0, 200, 100);

				// Draw time numbers
				number[times[i][0]].Draw(mdc, 325, yPos[i], 50, 50, 0, 0, 100, 100);
				number[times[i][1]].Draw(mdc, 375, yPos[i], 50, 50, 0, 0, 100, 100);
				semicolon.Draw(mdc, 425, yPos[i], 50, 50, 0, 0, 100, 100);
				number[times[i][2]].Draw(mdc, 475, yPos[i], 50, 50, 0, 0, 100, 100);
				number[times[i][3]].Draw(mdc, 525, yPos[i], 50, 50, 0, 0, 100, 100);
			}
		}
		//----------------------------------------------------------------------

		BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY);
		DeleteDC(mdc);
		DeleteObject(hBitmap);
		DeleteObject(hPen);
		DeleteObject(hBrush);
		DeleteObject(oldPen);
		DeleteObject(oldBrush);
		EndPaint(hWnd, &ps);
		break;
	/*case WM_LBUTTONDOWN:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);

		InvalidateRect(hWnd, NULL, FALSE);
		break;

	
	case WM_RBUTTONDOWN:
	
		break;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hWnd, iMsg, wParam, lParam));
}

BOOL collision(float a_x1, float a_y1, float a_x2, float a_y2, float b_x1, float b_y1, float b_x2, float b_y2)
{
	if (((b_x1 < a_x1 && a_x1 < b_x2) || (b_x1 < a_x2 && a_x2 < b_x2)) &&
		((b_y1 < a_y2 && a_y2 < b_y2) || (b_y1 < a_y1 && a_y1 < b_y2)))
	{
		return true;
	}
	return false;
}

BOOL Collision(int playerX1, int playerY1, int playerX2, int playerY2, int blockX1, int blockY1, int blockX2, int blockY2)
{
	return (playerX1 < blockX2 && playerX2 > blockX1 && playerY1 < blockY2 && playerY2 > blockY1);
}

void checkKey()
{
	if (keyStates['a']||keyStates['A'])
	{
		if (!player[0].isdeath)
		{
			player[0].ismoving = true;
			if (Collision(player[0].x1 - 5, player[0].y1, player[0].x2 - 5, player[0].y2, player[1].x1, player[1].y1, player[1].x2, player[1].y2))
			{

			}
			else
				player[0].x -= 5;

			player[0].direction = 1;
		}
		
	}
	if (keyStates['d']||keyStates['D'])
	{
		if (!player[0].isdeath)
		{
			player[0].ismoving = true;
			if (Collision(player[0].x1 + 5, player[0].y1, player[0].x2 + 5, player[0].y2, player[1].x1, player[1].y1, player[1].x2, player[1].y2))
			{
				//player[0].x -= 5;
			}
			else
				player[0].x += 5;
			player[0].direction = 2;
		}
		
	}
	if (keyStates['w']||keyStates['W'])
	{
		if (!player[0].isjump && !player[0].isjumpDown && !player[0].isdeath)
		{
			if (Collision(player[0].x1, player[0].y1, player[0].x2, player[0].y2, player[1].x1, player[1].y1, player[1].x2, player[1].y2))
			{

			}
			else
				player[0].isjump = true;
		}
	}
	if (keyStates['s'] || keyStates['S'])
	{
		if (player[0].exit)
		{
			//스테이지1
			if ((mapSelection == 0) && (collision(player[0].x1, player[0].y1, player[0].x2, player[0].y2, maps[mapSelection][7][0].x, maps[mapSelection][7][0].y, maps[mapSelection][7][0].x + 100, maps[mapSelection][7][0].y + 100)))
			{
				player[0].exitAll = true;
				player[0].exit = false;
				player[0].x = -200;
			}
			//스테이지2
			if ((mapSelection == 4) && (collision(player[0].x1, player[0].y1, player[0].x2, player[0].y2, maps[mapSelection][7][2].x, maps[mapSelection][7][2].y, maps[mapSelection][7][2].x + 100, maps[mapSelection][7][2].y + 100)))
			{
				player[0].exitAll = true;
				player[0].exit = false;
				player[0].x = -200;
			}
			//스테이지3
			if ((mapSelection == 7) && (collision(player[0].x1, player[0].y1, player[0].x2, player[0].y2, maps[mapSelection][7][3].x, maps[mapSelection][7][3].y, maps[mapSelection][7][3].x + 100, maps[mapSelection][7][3].y + 100)))
			{
				player[0].exitAll = true;
				player[0].exit = false;
				player[0].x = -200;
			}
		}
	}
	if (keyStates['f']||keyStates['F'])
	{
		if (!player[0].isshooting && !player[0].isdeath)
		{
			player[0].shooting = true;
		}
	}
	if (keyStates[VK_LEFT])
	{
		if (!player[1].isdeath)
		{
			player[1].ismoving = true;
			if (Collision(player[1].x1 - 5, player[1].y1, player[1].x2 - 5, player[1].y2, player[0].x1, player[0].y1, player[0].x2, player[0].y2))
			{
				//player[1].x += 6;
			}
			else
				player[1].x -= 5;
			player[1].direction = 1;
		}
		
	}
	if (keyStates[VK_RIGHT])
	{
		if (!player[1].isdeath)
		{
			player[1].ismoving = true;
			if (Collision(player[1].x1 + 5, player[1].y1, player[1].x2 + 5, player[1].y2, player[0].x1, player[0].y1, player[0].x2, player[0].y2))
			{
				//player[1].x -= 6;
			}
			else
				player[1].x += 5;
			player[1].direction = 2;
		}
		
	}
	if (keyStates[VK_UP])
	{
		if (!player[1].isjump && !player[1].isjumpDown && !player[1].isdeath)
		{
			if (Collision(player[1].x1, player[1].y1, player[1].x2, player[1].y2, player[0].x1, player[0].y1, player[0].x2, player[0].y2))
			{

			}
			else
				player[1].isjump = true;
		}
	}
	if (keyStates[VK_DOWN])
	{
		if (player[1].exit)
		{
			//스테이지1
			if ((mapSelection == 0) && (collision(player[1].x1, player[1].y1, player[1].x2, player[1].y2, maps[mapSelection][7][0].x, maps[mapSelection][7][0].y, maps[mapSelection][7][0].x + 100, maps[mapSelection][7][0].y + 100)))
			{
				player[1].exitAll = true;
				player[1].exit = false;
				player[1].x = -100;
			}
			//스테이지2
			if ((mapSelection == 4) && (collision(player[1].x1, player[1].y1, player[1].x2, player[1].y2, maps[mapSelection][7][2].x, maps[mapSelection][7][2].y, maps[mapSelection][7][2].x + 100, maps[mapSelection][7][2].y + 100)))
			{
				player[1].exitAll = true;
				player[1].exit = false;
				player[1].x = -100;
			}
			//스테이지3
			if ((mapSelection == 7) && (collision(player[1].x1, player[1].y1, player[1].x2, player[1].y2, maps[mapSelection][7][3].x, maps[mapSelection][7][3].y, maps[mapSelection][7][3].x + 100, maps[mapSelection][7][3].y + 100)))
			{
				player[1].exitAll = true;
				player[1].exit = false;
				player[1].x = -100;
			}
		}
	}
	if (keyStates[VK_SPACE])
	{
		if (!player[1].isshooting && !player[1].isdeath)
		{
			player[1].shooting = true;
		}
	}
}

void UpdateByPlayer()
{
	for (int i = 0; i < NUM_PLAYERS; i++)
	{
		//플레이어 좌표
		player[i].RenewPos();
		//기믹 작동
		triggerGimmick(i);
	}
}

void UpdatePlayerAnimation(PLAYER player[], int playerCount) {
	for (int i = 0; i < playerCount; i++) {
		player[i].UpdateMovement();
	}
}

void LoadImages() {
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

void LoadAllMapsFromFile(const string& filePath, map<int, vector<vector<BLOCK>>>& maps) {
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

void InitializeMaps() {
	LoadAllMapsFromFile("maps.txt", maps);
}

void ResetStage(int stageNum) {
	if (maps.find(stageNum) == maps.end()) {
		return; // Invalid stage number
	}

	player[0].x = (stageNum == 2) ? 200 : (stageNum == 3) ? 75 : 150;
	player[0].y = (stageNum == 2) ? 675 : (stageNum == 3) ? 675 : 575;
	player[1].x = (stageNum == 2) ? 600 : (stageNum == 3) ? 150 : 250;
	player[1].y = (stageNum == 2) ? 675 : (stageNum == 3) ? 675 : 575;
	mapSelection = (stageNum == 2) ? 0 : (stageNum == 3) ? 2 : 5;

	// Reset player properties
	player[0].isdeath = false;
	player[1].isdeath = false;
	player[0].haveKey = false;
	player[1].haveKey = false;
	player[0].size = 25;
	player[1].size = 25;

	// Reset map blocks from the loaded maps
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			maps[mapSelection][i][j].x = i * 100;
			maps[mapSelection][i][j].y = j * 100;
			maps[mapSelection][i][j].type = maps[stageNum][i][j].type;
			maps[mapSelection][i][j].select = maps[stageNum][i][j].select;
		}
	}
}

void resetTimers() {
	TimeCnt3 = TimeCnt2 = TimeCnt1 = TimeCnt0 = 0;
}

void updatePlayersPosition(int p0_x, int p0_y, int p1_x, int p1_y) {
	player[0].x = p0_x;
	player[0].y = p0_y;
	player[1].x = p1_x;
	player[1].y = p1_y;
}

void updateTimers(int timeArray[]) {
	TimeCnt3++;
	if (TimeCnt3 == 10) {
		TimeCnt2++;
		TimeCnt3 = 0;
	}
	if (TimeCnt2 == 6) {
		TimeCnt1++;
		TimeCnt2 = 0;
	}
	if (TimeCnt1 == 10) {
		TimeCnt0++;
		TimeCnt1 = 0;
	}
	timeArray[0] = TimeCnt0;
	timeArray[1] = TimeCnt1;
	timeArray[2] = TimeCnt2;
	timeArray[3] = TimeCnt3;
}

void drawBlock(int mapType, int x, int y, int type, HDC mdc) {
	if (type != -1) {
		block[type].Draw(mdc, x, y, 100, 100, 0, 0, 100, 100);
	}
}

void drawSpecialBlocks(int mapSelection, HDC mdc) {
	switch (mapSelection) {
	case 0:
		drawBlock(mapSelection, maps[mapSelection][0][3].x, maps[mapSelection][0][3].y, maps[mapSelection][0][3].type, mdc); // Key
		drawBlock(mapSelection, maps[mapSelection][7][0].x, maps[mapSelection][7][0].y, maps[mapSelection][7][0].type, mdc); // Door
		break;
	case 2:
		drawBlock(mapSelection, maps[mapSelection][2][6].x, maps[mapSelection][2][6].y, maps[mapSelection][2][6].type, mdc); // +Button
		break;
	case 3:
		drawBlock(mapSelection, maps[mapSelection][3][6].x, maps[mapSelection][3][6].y, maps[mapSelection][3][6].type, mdc); // -Button
		drawBlock(mapSelection, maps[mapSelection][4][1].x, maps[mapSelection][4][1].y, maps[mapSelection][4][1].type, mdc); // Key
		break;
	case 4:
		drawBlock(mapSelection, maps[mapSelection][7][2].x, maps[mapSelection][7][2].y, maps[mapSelection][7][2].type, mdc); // Door
		break;
	case 5:
		drawBlock(mapSelection, maps[mapSelection][0][6].x, maps[mapSelection][0][6].y, maps[mapSelection][0][6].type, mdc); // Key
		break;
	case 7:
		drawBlock(mapSelection, maps[mapSelection][3][6].x, maps[mapSelection][3][6].y, maps[mapSelection][3][6].type, mdc); // Switch
		drawBlock(mapSelection, maps[mapSelection][7][3].x, maps[mapSelection][7][3].y, maps[mapSelection][7][3].type, mdc); // Door
		break;
	}
}

void processMapSelection(int mapSelection, int i, int j, HDC mdc) {
	if (maps[mapSelection][i][j].select) {
		drawBlock(mapSelection, maps[mapSelection][i][j].x, maps[mapSelection][i][j].y, maps[mapSelection][i][j].type, mdc);
	}

	// Draw any special blocks (Key, Door, Button, etc.)
	if (i == 0 && j == 0) { // Call only once per iteration
		drawSpecialBlocks(mapSelection, mdc);
	}
}

// 충돌 처리 함수 정의
void handleCollisionWithBlocks(BULLET& bullet, vector<vector<BLOCK>>& map) {
	for (auto& row : map) {
		for (auto& block : row) {
			if (bullet.select && block.select && block.type == 3) {
				if (collision(bullet.x1, bullet.y1, bullet.x2, bullet.y2, block.x, block.y, block.x + 100, block.y + 100)) {
					bullet.select = false;
					block.select = false;
				}
			}
		}
	}
}
// 플레이어 충돌 처리 함수
void handlePlayerCollision(BULLET& bullet, PLAYER& player) {
	if (bullet.select && collision(player.x1, player.y1, player.x2, player.y2, bullet.x1, bullet.y1, bullet.x2, bullet.y2)) {
		bullet.select = false;
		player.isdeath = true;
	}
}
void fireCannon(int& CBcntcnt, int& CBcnt, std::vector<BULLET>& cannonBullets, std::vector<std::vector<BLOCK>>& map) {
	CBcntcnt++;
	if (CBcntcnt == 150) {
		CBcntcnt = 0;
		CBcnt++;
		cannonBullets[CBcnt].select = true;
		cannonBullets[CBcnt].x = map[7][6].x;  // 대포 위치
		cannonBullets[CBcnt].y = map[7][6].y + 50;
		if (CBcnt == 333) {
			CBcnt = 0;
		}
	}
	// 대포알 이동 처리
	for (auto& cannonBullet : cannonBullets) {
		if (cannonBullet.select) {
			cannonBullet.x -= 5;  // 이동
			if (cannonBullet.x < 0 || cannonBullet.x > 800) {
				cannonBullet.select = false;  // 화면 밖으로 나가면 비활성화
			}
			cannonBullet.x1 = cannonBullet.x - cannonBullet.size;
			cannonBullet.y1 = cannonBullet.y - cannonBullet.size;
			cannonBullet.x2 = cannonBullet.x + cannonBullet.size;
			cannonBullet.y2 = cannonBullet.y + cannonBullet.size;
		}
	}
}
void handlePlayerShooting(PLAYER& player, std::vector<BULLET>& bullets, int& bulletCnt) {
	if (player.shooting) {
		bullets[bulletCnt].select = true;
		bullets[bulletCnt].direction = player.direction;
		bullets[bulletCnt].x = player.x;
		bullets[bulletCnt].y = player.y;
		bulletCnt = (bulletCnt + 1) % 333;  // 순환 카운터
		player.shooting = false;
		player.isshooting = true;
	}
}
void updateShooting(PLAYER& player) {
	if (player.isshooting) {
		player.shootingCnt++;
		if (player.shootingCnt == 30) {
			player.shootingCnt = 0;
			player.isshooting = false;
		}
	}
}

// 열쇠 먹기 처리 함수
void handleKeyPickup(PLAYER& player, BLOCK& keyBlock, vector<BLOCK> blocksToActivate) {
	if (collision(player.x1, player.y1, player.x2, player.y2, keyBlock.x, keyBlock.y, keyBlock.x + 100, keyBlock.y + 100)) {
		player.haveKey = true;
		keyBlock.type = -1; // 열쇠를 없앰

		// 열쇠 먹으면 활성화되는 블럭
		for (auto& block : blocksToActivate) {
			block.type = 1;
			block.select = true;
		}
	}
}
// 문 열기 처리 함수
void handleDoorOpen(PLAYER& player, BLOCK& doorBlock) {
	if (player.haveKey && collision(player.x1, player.y1, player.x2, player.y2, doorBlock.x, doorBlock.y, doorBlock.x + 100, doorBlock.y + 100)) {
		doorBlock.type = 5; // 문이 열림
		player.haveKey = false;
		player.exit = true;
	}
}
// 스위치 처리 함수 (+ 스위치)
void handleSwitchPress(PLAYER& player, BLOCK& switchBlock, bool increaseSize) {
	if (collision(player.x1, player.y1, player.x2, player.y2, switchBlock.x, switchBlock.y, switchBlock.x + 100, switchBlock.y + 100)) {
		if (increaseSize && player.size < 200) {
			player.y -= 5;
			player.size += 5;
		}
		else if (!increaseSize && player.size > 10) {
			player.y += 5;
			player.size -= 5;
		}
		switchBlock.type = increaseSize ? 6 : 10; // 스위치 눌린 상태
	}
}

void checkPlayerGround(PLAYER& player, const std::vector<std::vector<BLOCK>>& map) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (map[i][j].select && Collision(player.x1, player.y1, player.x2, player.y2, map[i][j].x, map[i][j].y, map[i][j].x + 100, map[i][j].y + 100)) {
				player.isOnGround = true;
				return;  // 충돌 확인되면 더 이상 체크할 필요 없음
			}
		}
	}
}

void triggerGimmick(int P_ID)
{
	switch (mapSelection)
	{
	case 0:
		// 나중에 객체 관련해서 따로 관리할 컨테이너 만들기 // 스테이지 1 열쇠 먹기
		handleKeyPickup(player[P_ID], maps[mapSelection][0][3], vector<BLOCK>{ maps[mapSelection][2][3], maps[mapSelection][3][2], maps[mapSelection][4][1], maps[mapSelection][5][1], maps[mapSelection][6][1], maps[mapSelection][7][1] });
		// 스테이지 1 문 열기
		handleDoorOpen(player[P_ID], maps[mapSelection][7][0]);
		break;
	case 2:
		handleSwitchPress(player[P_ID], maps[mapSelection][2][6], true);
		if (maps[mapSelection][2][6].type == 6)
			maps[mapSelection][2][6].type = 7;
		break;
	case 3:
		// 스테이지 2 열쇠 먹기
		handleKeyPickup(player[P_ID], maps[mapSelection][4][1], vector<BLOCK>{ maps[mapSelection][5][4], maps[mapSelection][4][5], maps[mapSelection][3][6] });
		// 스테이지 2 -스위치 누르기
		handleSwitchPress(player[P_ID], maps[mapSelection][3][6], false);
		// 스위치 상태가 눌린 후 풀리기
		if (maps[mapSelection][3][6].type == 10)
			maps[mapSelection][3][6].type = 11;
		break;
	case 4:
		handleDoorOpen(player[P_ID], maps[mapSelection][7][2]);
		break;
	case 5:
		handleKeyPickup(player[P_ID], maps[mapSelection][0][6], {});
		break;
	case 7:
		handleSwitchPress(player[P_ID], maps[mapSelection][3][6], true);
		handleDoorOpen(player[P_ID], maps[mapSelection][7][3]);
		break;
	}
}

// 플레이어와 블록의 모든 충돌(점프, 좌우)을 처리하는 함수
void handleCollision(PLAYER& player, const BLOCK& block) {
	// 오른쪽 충돌 체크
	if (Collision(player.x1 + 5, player.y1 - 5, player.x2 + 5, player.y2 - 5, block.x, block.y, block.x + 100, block.y + 100)) {
		player.x -= 5;
	}
	// 왼쪽 충돌 체크
	if (Collision(player.x1 - 5, player.y1 - 5, player.x2 - 5, player.y2 - 5, block.x, block.y, block.x + 100, block.y + 100)) {
		player.x += 5;
	}

	// 점프 충돌 체크
	if (player.isjump && Collision(player.x1, player.y1 - 5, player.x2, player.y2 - 5, block.x, block.y, block.x + 100, block.y + 100)) {
		player.isjumpCnt = 30;
	}
	// 내려가기(점프 다운) 충돌 체크
	if (player.isjumpDown && Collision(player.x1, player.y1 + 5, player.x2, player.y2 + 5, block.x, block.y, block.x + 100, block.y + 100)) {
		player.isjumpDownCnt = 30;
	}
}

void processMapCollision(PLAYER* players, const std::vector<std::vector<BLOCK>>& map) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (map[i][j].select) {
				// 두 플레이어에 대해 충돌 체크
				for (int p = 0; p < NUM_PLAYERS; p++) {
					handleCollision(players[p], map[i][j]);
				}
			}
		}
	}
}
