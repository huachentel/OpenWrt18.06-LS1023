/***************************************************************
* FILE:	socket.c

* DESCRIPTION:
		多线程socket本地并发服务器，接收用户字符串；
MODIFY HISTORY:
	2018.11.22           liuhuan   create
****************************************************************/


#include "socket.h"
#include "gpio_func.h"

extern int dev_fd;

void *thread_func(void* arg)
{
	int clientfd;
	int nbytes;
	char usrBuf[USR_BUF_MAX_SIZE];

	/*接收客户端fd*/
	clientfd = *(int *)arg;
	memset(&usrBuf, 0, sizeof(en1002_gpio_t));

	while(1)
	{
		nbytes = socket_recv(clientfd, usrBuf, USR_BUF_MAX_SIZE);
		if(nbytes < 0)
		{
	        printf("recv error\n");
	        break;
	    }
		else if (nbytes == 0)
		{
	        printf("peer exit\n");
	        break;
	    }
		else
		{
	        analysis_usrBuf(usrBuf, clientfd);
	    }
	}

    free (arg);
    
	pthread_exit(NULL);  
}

int socket_func(void)
{
	int serverfd, clientfd;
	socklen_t addrlen = sizeof(struct sockaddr);
    struct sockaddr_in servaddr, cliaddr;
	pthread_t tid;

	/*创建本地套接字接口*/
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd < 0)
	{
		printf("socket error\n");
		return -1;
	}  

	/*填充本地地址信息*/
	memset(&servaddr, 0, addrlen);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(10000);

	/*绑定本地套接字*/
	if(bind(serverfd, (struct sockaddr *)&servaddr, addrlen) < 0)
	{
		printf("Bind error");
		return -1;
	}

	/*监听套接字*/
	if(listen(serverfd, 5) < 0)
	{
		printf("Listen error\n");
		return -1;
	}

	while(1)
	{
		/*接收连接*/
		clientfd = accept(serverfd, (struct sockaddr *)&cliaddr, &addrlen);

		if(pthread_create(&tid, NULL, thread_func, (void*)&clientfd))
		{
			printf("Pthread_create error\n");  
			break;
		}
	}

	close(serverfd);
}

int socket_recv(int clientfd, char *readBuf, int len)
{
	ssize_t ret;

	/*接收客户端数据*/
	ret = recv(clientfd, readBuf, len, 0);
	readBuf[ret - 1] = '\0';

	return (int)ret;
}

int socket_send(int clientfd, char *sendBuf, int len)
{
	ssize_t ret;

	ret = send(clientfd, sendBuf, sizeof(sendBuf), 0);

	return (int)ret;
}

