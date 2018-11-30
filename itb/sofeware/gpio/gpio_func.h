#ifndef __GPIO_FUNC_H__
#define __GPIO_FUNC_H__

#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>


#define GPIO1_NUM		480
#define GPIO2_NUM		448
#define GPIO4_NUM		384

#define GPIO_EN1002(a, b)	(a + b)  

#define IGSFP0_NEXIST	GPIO_EN1002(GPIO1_NUM, 23) //GPIO1_23
#define IGSFP0_DISABLE	GPIO_EN1002(GPIO1_NUM, 24)
#define IGSFP1_DISABLE	GPIO_EN1002(GPIO1_NUM, 27)
#define IGSFP1_NEXIST	GPIO_EN1002(GPIO1_NUM, 29)
#define CPU_LED			GPIO_EN1002(GPIO4_NUM, 12)
#define LTE_LED_0		GPIO_EN1002(GPIO4_NUM, 13)
#define LTE_LED_1		GPIO_EN1002(GPIO1_NUM, 20)
#define LTE_LED_2		GPIO_EN1002(GPIO1_NUM, 21)
#define LTE_PWR_EN		GPIO_EN1002(GPIO1_NUM, 19)
#define WIFI_PWR_EN		GPIO_EN1002(GPIO1_NUM, 18)
#define CLOUD_LED		GPIO_EN1002(GPIO1_NUM, 16)
#define GPIO_RST		GPIO_EN1002(GPIO2_NUM, 27)

#define GPIO_DEV			"/dev/en1002_gpio_op"
#define SEND_BUF_SIZE		2
#define CMD_SEP_CHARS		" "
#define NAME_MAX_SIZE		30
#define CMD_MAX_ARGCS		10

#define READ				0
#define WRITE				1

#define HIGH				1
#define LOW					0

extern int dev_fd;	

typedef struct en1002_gpio_s{
	int gpio_id;
	int value;
}en1002_gpio_t;

typedef struct en1002_gpio_info{
	char name[NAME_MAX_SIZE];
	int gpio_id;
	int direction;
	int initStatus;
}en1002_gpio_info_t;

int analysis_usrBuf(char *cmd, int clientfd);
int dev_fd_get(void);
int dev_fd_close(void);
int gpio_init(void);

#endif
