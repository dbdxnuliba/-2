#include "7dof_socket.h"
#include "../errmsg.h"

#include <stdio.h>
#include <Winsock2.h>
#include <time.h>

#pragma comment(lib,"ws2_32.lib")

static SOCKET sockClient = INVALID_SOCKET;

// extern struct rsi_media sm_media;

// #define mid sm_media

SOCKET socket_open(const char *ip, const int *port)
{
	SOCKET sockC = INVALID_SOCKET;
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR){
		fprintf(stderr, "WSAStartup function failed with error: %d\n", iResult);
		return -1;
	}

	// Create a SOCKET for connecting to server
	if ((sockC = socket(AF_INET, SOCK_STREAM, 0)) == LMEM_INVALID_HANDLE) {
		fprintf(stderr, "socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons((unsigned short)(*port));

	iResult = connect(sockC, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	if (iResult == SOCKET_ERROR) {
		fprintf(stderr, "connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(sockC);
		if (iResult == SOCKET_ERROR)
			fprintf(stderr, "closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	return sockC;
}

int rob_socket_open(const char *ip, const int *port)
{
	sockClient = socket_open(ip, port);
	if (sockClient != -1)
		return 1;
	return 0;
}

int socket_close(SOCKET sockC)
{
	int iResult = closesocket(sockC);
	
	if(iResult == SOCKET_ERROR){
		fprintf(stderr, "closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	WSACleanup();
	return 0;
}

void rob_socket_close()
{
	socket_close(sockClient);
	sockClient = INVALID_SOCKET;
}

int socket_send(SOCKET s, const char  *buf, int len, int flags, int timeout)
{
	const int ClOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
	int iResult, sendsize = 0;
	clock_t t1, t2;

	t1 = clock() / ClOCKS_PER_MS;

	while (1){
		if ((iResult = send(s, buf, len, flags)) == SOCKET_ERROR){
			fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
			closesocket(sockClient);
			WSACleanup();
			return 0;
		} else if(iResult == len){
			return 1;
		}

		t2 = clock() / ClOCKS_PER_MS;
		if ((t2 - t1) < timeout)
			continue;
		else 
			return 0;

	}
	return 0;
}

int rob_socket_send_string(const char  *buf, int len)
{
#define TIMEOUT 3000
	return socket_send(sockClient, buf, len, 0, TIMEOUT);
}

//
// desire_len: 期望收到的数据数量
// buflen: buf的实际长度
// 在实际调用中保证desire_len的长度小于等于buflen
//
int socket_recv(SOCKET s, char  *buf, int buflen, int desire_len, int flags)
{
#define TIMEOUT 3000
	const int ClOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
	clock_t t1, t2;
	int iResult, readc = 0;

	t1 = clock() / ClOCKS_PER_MS;

    DWORD timeout = 100; //millisec
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	while (1){
		iResult = recv(s, buf, buflen, 0);

        if (iResult < 0) ;//return readc;
        else{
			readc += iResult;
			buf += iResult;
            buflen -= iResult;
        }
		
		if (readc >= desire_len)  return readc;

		t2 = clock() / ClOCKS_PER_MS;
		if ((t2 - t1) < TIMEOUT)
			continue;
		else
			return readc;
	}
	
	return readc;
}

int rob_socket_read_string(char *buf, int buflen, int desire_len)
{
	if (desire_len > buflen) return 0;
	return socket_recv(sockClient, buf, buflen, desire_len, 0);
}

int socket_get_var(const char *name)
{
	char buf[16];
	//socket_send(sockClient, name, strlen(name) + 1, 0);
	//socket_recv(sockClient, buf, 16, 0);

	return atoi(buf);
}
