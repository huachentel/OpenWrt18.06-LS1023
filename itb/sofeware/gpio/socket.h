#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define USR_BUF_MAX_SIZE	50

int socket_func(void);
int socket_recv(int clientfd, char *readBuf, int len);
int socket_send(int clientfd, char *sendBuf, int len);

#endif

