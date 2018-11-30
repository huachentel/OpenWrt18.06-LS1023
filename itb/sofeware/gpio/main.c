/***************************************************************
* FILE:	main.c

* DESCRIPTION:
		GPIO应用程序
MODIFY HISTORY:
	2018.11.22           liuhuan   create
****************************************************************/


#include "gpio_func.h"
#include "socket.h"

int main(int argc, char *argv[])
{
	dev_fd_get();
	gpio_init();
	socket_func();
	dev_fd_close();
}



