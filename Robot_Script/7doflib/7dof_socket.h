#ifndef DOF_SOCKET_H
#define DOF_SOCKET_H

#include<WinSock2.h>
//
// ����ֵ�� 0/1
//
int rob_socket_open(const char *ip, const int *port);
void rob_socket_close();

//
// buf: ����Ҫ���͵�����
// len: buf�ĳ���
// ����ֵ�� �����Ƿ��ͳɹ� 0/1
// description: timeout = 3000ms
//
int rob_socket_send_string(const char  *buf, int len);

//
// buf���յ����ݵĻ���ռ�
// buflen: buf�ĳ���
// desire_len: �����յ����ݵĳ���
// ����ֵ��ʵ���յ����ݵĳ��ȣ��������û���յ�����0
// description: timeout = 3000ms
//
int rob_socket_read_string(char *buf, int buflen, int desire_len);


//
//
//
int socket_send(SOCKET s, const char  *buf, int len, int flags, int timeout);
int socket_close(SOCKET sockC);
SOCKET socket_open(const char *ip, const int *port);

#endif