#include "stdafx.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	int iResult; 
	WSADATA wsaData;

	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService;

	int recvbuflen = 8192;
	char *sendbuf = "{\"id\": 0, \"method\": \"mining.subscribe\", \"params\": [\"cgminer/3.7.3\"]}\n";
	char recvbuf[8192] = "";

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStatup failed with error: %d\n", iResult);
		return 1;
	}

	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		wprintf(L"1socket failed with error: %ld\n", WSAGetLastError());
		return 1;
	}

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("185.18.148.41");	
	clientService.sin_port = htons(23399);

	iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"2socket failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"3socket failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %d\n", iResult);

	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			wprintf(L"Bytes received: %d\n", iResult);
		else if (iResult == 0)
			wprintf(L"Connection closed\n");
		else
			wprintf(L"recv failed with error: %d\n", WSAGetLastError());

		if (iResult > 400) {
			sendbuf = "{\"id\": 2, \"method\": \"mining.authorize\", \"params\": [\"philiar.1\", \"x\"]}\n";
			iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		}
	} while (iResult > 0);

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR){
		wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"close failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	WSACleanup();
	return 0;
}

