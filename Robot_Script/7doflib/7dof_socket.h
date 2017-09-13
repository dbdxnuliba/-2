#ifndef DOF_SOCKET_H
#define DOF_SOCKET_H

#include<WinSock2.h>
//
// 返回值， 0/1
//
int rob_socket_open(const char *ip, const int *port);
void rob_socket_close();

//
// buf: 保存要发送的数据
// len: buf的长度
// 返回值， 返回是否发送成功 0/1
// description: timeout = 3000ms
//
int rob_socket_send_string(const char  *buf, int len);

//
// buf：收到数据的缓冲空间
// buflen: buf的长度
// desire_len: 期望收到数据的长度
// 返回值，实际收到数据的长度，出错或者没有收到返回0
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