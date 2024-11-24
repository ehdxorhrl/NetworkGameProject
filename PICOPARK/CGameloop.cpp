#include "stdafx.h"
#include "CGameloop.h"

CGameloop::CGameloop()
{
}

CGameloop::~CGameloop()
{

}

void CGameloop::Init(HWND hWnd)
{
	hwnd = hWnd;
	timeManager.Init();
	inputManager.Init(hwnd);
	SceneManager.Init();
}

void CGameloop::Update()
{
	timeManager.Update();
	inputManager.Update();
	SceneManager.Update();
}

void CGameloop::Render()
{
    HDC mdc;
    HBITMAP hBitmap;
    RECT rt;

    GetClientRect(hwnd, &rt);
    hdc = GetDC(hwnd);
    mdc = CreateCompatibleDC(hdc); // 메모리 DC 생성
    hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom); // 비트맵 생성
    SelectObject(mdc, hBitmap); // 비트맵을 메모리 DC에 선택

    // 배경을 흰색 사각형으로 채움
    Rectangle(mdc, 0, 0, rt.right, rt.bottom);

    // 씬 매니저 렌더링
    if (&SceneManager) {
        SceneManager.Render(mdc);
        OutputDebugString(L"SceneManager::Render called\n");
    }

    // 백 버퍼의 내용을 화면에 복사
    BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY);

    // GDI 객체 원상복귀 및 자원 해제
    DeleteObject(hBitmap); // 비트맵 해제
    DeleteDC(mdc); // 메모리 DC 해제
    ReleaseDC(hwnd, hdc);
}