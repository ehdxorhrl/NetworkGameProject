#include "framework.h"
#include "Packet.h"

#define SERVERPORT 9000
#define BUFSIZE    512

CRITICAL_SECTION cs; // 스레드 동기화를 위한 변수 및 객체 접근을 보호하기 위해 사용

bool SendPlayerIDResponse(SOCKET& sock, const PlayerIDResponsePacket& responsePacket)
{
	int retval;
	
	retval = send(sock, (char*)&responsePacket, sizeof(responsePacket), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send error");
	}
}

DWORD WINAPI UpdateThreadFunc(LPVOID lpParam)
{
	EnterCriticalSection(&cs);

	while (true)
	{

	}

	LeaveCriticalSection(&cs);
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {

}

int main(int argc, char* argv[])
{
	int retval;;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
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

	// CS 초기화
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

		
		// 스레드 생성
		hThread = CreateThread(NULL, 0, CommunicationThread, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}
		

	}

	WaitForSingleObject(hUpdateThread, INFINITE);

	// CS 삭제
	DeleteCriticalSection(&cs);

	// 소켓 닫기
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
