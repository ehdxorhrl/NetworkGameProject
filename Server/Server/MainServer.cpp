#include "framework.h"
#include "Packet.h"

#define SERVERPORT 9000
#define BUFSIZE    512

CRITICAL_SECTION cs; // ������ ����ȭ�� ���� ���� �� ��ü ������ ��ȣ�ϱ� ���� ���

DWORD WINAPI UpdateThreadFunc(LPVOID lpParam)
{
	EnterCriticalSection(&cs);

	while (true)
	{

	}

	LeaveCriticalSection(&cs);
}

int main(int argc, char* argv[])
{
	int retval;;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread, hUpdateThread;

	// CS �ʱ�ȭ
	InitializeCriticalSection(&cs);

	// UpdateThread
	hUpdateThread = CreateThread(NULL, 0, UpdateThreadFunc, NULL, 0, NULL);
	if (hUpdateThread == NULL) {
		err_display("CreateThread() for UpdateThreadFunc");
		return 1;
	}

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		/*
		// ������ ����
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}
		*/

	}

	WaitForSingleObject(hUpdateThread, INFINITE);

	// CS ����
	DeleteCriticalSection(&cs);

	// ���� �ݱ�
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}
