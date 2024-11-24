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
    mdc = CreateCompatibleDC(hdc); // �޸� DC ����
    hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom); // ��Ʈ�� ����
    SelectObject(mdc, hBitmap); // ��Ʈ���� �޸� DC�� ����

    // ����� ��� �簢������ ä��
    Rectangle(mdc, 0, 0, rt.right, rt.bottom);

    // �� �Ŵ��� ������
    if (&SceneManager) {
        SceneManager.Render(mdc);
        OutputDebugString(L"SceneManager::Render called\n");
    }

    // �� ������ ������ ȭ�鿡 ����
    BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY);

    // GDI ��ü ���󺹱� �� �ڿ� ����
    DeleteObject(hBitmap); // ��Ʈ�� ����
    DeleteDC(mdc); // �޸� DC ����
    ReleaseDC(hwnd, hdc);
}