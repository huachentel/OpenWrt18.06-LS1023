/***************************************************************
* FILE:	gpio_func.c

* DESCRIPTION:
		解析用户字符串，操作相应的GPIO;
MODIFY HISTORY:
	2018.11.22           liuhuan   create
****************************************************************/

#include "gpio_func.h"

int dev_fd;     /*GPIO设备文件描述符*/

static en1002_gpio_info_t gpio_info_tb[] = {
    {"CLOUD_LED", CLOUD_LED, WRITE, LOW},
    {"LTE_LED0", LTE_LED_0, WRITE, LOW},
    {"LTE_LED1", LTE_LED_1, WRITE, LOW},
    {"LTE_LED2", LTE_LED_2, WRITE, LOW},
    {"CPU_LED", CPU_LED, WRITE, 0},
    {"IGSFP0_DISABLE", IGSFP0_DISABLE, WRITE, LOW},
    {"IGSFP1_DISABLE", IGSFP1_DISABLE, WRITE, LOW},
    {"WIFI_PWR", WIFI_PWR_EN, WRITE, HIGH},
    {"LTE_PWR", LTE_PWR_EN, WRITE, HIGH},
    {"IGSFP0_NEXIST", IGSFP0_NEXIST, READ, LOW},
    {"IGSFP1_NEXIST", IGSFP1_NEXIST, READ, LOW},
    {"GPIO_RST", GPIO_RST, READ, LOW},
    {}
};

#define GPIO_INFO_TAB_SIZE      sizeof(gpio_info_tb) / sizeof(en1002_gpio_info_t)


/*
	function: 分割字符串
	argument: cmd_str:用户字符串指针
			  tokens: 存放参数的指针数组
	note: cmd_str 将被损坏 返回参数的个数
*/
static int line2tokens(char * cmd_str, char ** tokens)
{
	char *token;
	unsigned char i = 0;

	if ((cmd_str == NULL) || (strlen(cmd_str) < 1))
		return -1;

	/* parse the line */
	i = 0;

	token = strtok(cmd_str, CMD_SEP_CHARS);

	while(token != NULL)
	{
		tokens[i++] = token;
		token = strtok(NULL, CMD_SEP_CHARS);
	}
	tokens[i] = NULL;

	return i;
}

int dev_fd_get()
{
	dev_fd = open(GPIO_DEV, O_RDWR);
	if(dev_fd < 0)
	{
		printf("open gpio device faided\n");
		exit(-1);
	}

	return 0;
}

int dev_fd_close()
{
    if(close(dev_fd) < 0)
    {
		printf("open gpio device faided\n");
		return -1;
    }

    return 0;
}

/*
	function: 操作相应的GPIO,并将结果返回给用户
	argment: usrInfoTmp:要设置GPIO的结构体
			 op: WRITE or READ  
			 tabID: 操作的GPIO在gpio_info_tb中的位置
			 clientfd: READ操作用到
*/
static int en1002_gpio_op(en1002_gpio_t *usrInfoTmp, int op, int tabID, int clientfd)
{
    char sendBuf[SEND_BUF_SIZE];

    if(op == READ)    /*输入方向       读取*/
    {	
    	/*判断是不是可读*/
		if(gpio_info_tb[tabID].direction != READ)
		{
			printf("read only\n");
			return -1;
		}
    
        read(dev_fd, usrInfoTmp, sizeof(en1002_gpio_t));
        printf("%s = %d\n", gpio_info_tb[tabID].name, usrInfoTmp->value);
        sprintf(sendBuf, "%d", usrInfoTmp->value);
        socket_send(clientfd, sendBuf, sizeof(sendBuf));
    }
    else if(op == WRITE)      /*输出方向       写入*/
    {
    	/*判断是不是可写*/
		if(gpio_info_tb[tabID].direction != WRITE)
		{
			printf("write only\n");
			return -1;
		}
    
        write(dev_fd, usrInfoTmp, sizeof(en1002_gpio_t));
        printf("set success %s = %d\n", gpio_info_tb[tabID].name, usrInfoTmp->value);
    }

    return 1;
}

/*
	cpu灯闪烁线程
*/
void *cpu_led_init(void* arg)
{	
	en1002_gpio_t cpuLed;

	cpuLed.gpio_id = CPU_LED;
	
    while(1)
    {
    	cpuLed.value = HIGH;
    	write(dev_fd, &cpuLed, sizeof(en1002_gpio_t));
    	sleep(1);
    	cpuLed.value = LOW;
    	write(dev_fd, &cpuLed, sizeof(en1002_gpio_t));
    	sleep(1);
    }

    pthread_exit(NULL);  
}


int gpio_init(void)
{
	en1002_gpio_t gpioInit;
	int i;
	pthread_t tid;

	for(i = 0; i < GPIO_INFO_TAB_SIZE; i++)
	{
		gpioInit.gpio_id = gpio_info_tb[i].gpio_id;
		gpioInit.value = gpio_info_tb[i].initStatus;
		en1002_gpio_op(&gpioInit, WRITE, i, 0);
	}

	if(pthread_create(&tid, NULL, cpu_led_init, (void*)NULL))
	{
		printf("Pthread_create error\n");  
		return -1;
	}
}

/*
	function: 处理用户字符串
	argument：cmd：用户层字符串
			  clientfd: 客户端fd
*/
int analysis_usrBuf(char *cmd, int clientfd)
{
	char cmdTmp[USR_BUF_MAX_SIZE];
	static char *argv[CMD_MAX_ARGCS];       /*存放没个参数的指针*/
	int argc, i;
	en1002_gpio_t gpioCtl;

	strcpy(cmdTmp, cmd);
	argc = line2tokens(cmdTmp, argv); /*分割字符串*/
	if(argc != 2 && argc != 3)
	{
		printf("Parameter error\n");
		return -1;
	}

	for(i = 0; i < GPIO_INFO_TAB_SIZE; i++)
	{
		/*如果找到对应name的gpio终止循环，继续向下运行*/
		if(!strcmp(argv[1], gpio_info_tb[i].name))
		{
			gpioCtl.gpio_id = gpio_info_tb[i].gpio_id;
			break;
		}
	}
        
	/*判断对应GPIO所要执行的操作*/
	if(i < GPIO_INFO_TAB_SIZE)
	{
		if((!strcmp(argv[0], "GET")) && (argc == 2))
		{
			en1002_gpio_op(&gpioCtl, READ, i, clientfd);
		}
		else if((!strcmp(argv[0], "SET")) && (argc == 3))
		{
			if(!strncmp(argv[2], "LOW", 2))
			{
				gpioCtl.value = LOW;
			}
			else if(!strncmp(argv[2], "HIGH", 3))
			{
				gpioCtl.value = HIGH;
			}
			else
			{
				printf(" error argument 3 'HIGH' or 'LOW'\n");
				return -1;
			}
			en1002_gpio_op(&gpioCtl, WRITE, i, clientfd);
		}
		else
		{
			printf("error input opration\n");
			return -1;
		}
	}
	else
	{
		printf("error gpio name\n");
		return -1;
	}
}


