#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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

#define IN				1
#define OUT				0

#define NAME_MAX_SIZE	 20
#define USR_BUF_MAX_SIZE 50

#define CMD_SEP_CHARS	" "
#define CMD_MAX_ARGCS	3



typedef struct en1002_gpio_s{
	int gpio_id;
	int value;
}en1002_gpio_t;

typedef struct en1002_gpio_info{
	char name[NAME_MAX_SIZE];
	int gpio_id;
	int direction;
}en1002_gpio_info_t;

typedef struct thread_arg{
    int clientfd;  
    struct sockaddr_in client;  
}thread_arg_t;  

static en1002_gpio_info_t gpio_info_tb[] = {
	{"CLOUD_LED", CLOUD_LED, OUT},
	{"WIFI_PWR", WIFI_PWR_EN, OUT},
	{"LTE_PWR", LTE_PWR_EN, OUT},
	{"LTE_LED0", LTE_LED_0, OUT},
	{"LTE_LED1", LTE_LED_1, OUT},
	{"LTE_LED2", LTE_LED_2, OUT},
	{"CPU_LED", CPU_LED, OUT},
	{"IGSFP0_DISABLE", IGSFP0_DISABLE, OUT},
	{"IGSFP1_DISABLE", IGSFP1_DISABLE, OUT},
	{"IGSFP0_NEXIST", IGSFP0_NEXIST, IN},
	{"IGSFP1_NEXIST", IGSFP1_NEXIST, IN},
	{"GPIO_RST", GPIO_RST, IN},
	{}
};

#define GPIO_INFO_TAB_SIZE	sizeof(gpio_info_tb) / sizeof(en1002_gpio_info_t)

/*cmd_line 将被损坏*/
/*返回token的个数*/
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

static int en1002_gpio_op(en1002_gpio_t *usrInfoTmp, int op, int clientfd)
{
	int fd;
	char buf[2];

	fd = open("/dev/en1002_gpio_op", O_RDWR);
	if(fd < 0)
	{
		printf("open /dev/en1002_gpio_op faided\n");
		return -1;
	}
	if(op == IN)
	{
		read(fd, usrInfoTmp, sizeof(en1002_gpio_t));
		printf("value = %d\n", usrInfoTmp->value);
		sprintf(buf, "%d", usrInfoTmp->value);
		send(clientfd, buf, sizeof(buf), 0);
	}
	else if(op == OUT)
	{
		write(fd, usrInfoTmp, sizeof(en1002_gpio_t));
		printf("set success\n");
	}

	close(fd);
	
	return 1;
}

static int analysis_usrBuf(char *cmd, int clientfd)
{
	char cmdTmp[USR_BUF_MAX_SIZE];
	static char *argv[CMD_MAX_ARGCS];
	int argc, i;
	en1002_gpio_t usrInfo;

	strcpy(cmdTmp, cmd);
	
	argc = line2tokens(cmdTmp, argv);
	if(argc != 2 && argc != 3)
	{
		printf("Parameter error\n");
		return -1;
	}	

	for(i = 0; i < GPIO_INFO_TAB_SIZE; i++)
	{
		/*如果找到对应name的gpio*/
		if(!strcmp(argv[1], gpio_info_tb[i].name))
		{
			usrInfo.gpio_id = gpio_info_tb[i].gpio_id;
			break;
		}
	}
	
	if(i < GPIO_INFO_TAB_SIZE)
	{
		if((!strcmp(argv[0], "GET")) || (argc == 2))
		{
			en1002_gpio_op(&usrInfo, IN, clientfd);
		}
		else if((!strcmp(argv[0], "SET")) || (argc == 3))
		{
			if(!strncmp(argv[2], "ON", 2))
			{
				usrInfo.value = 0;
			}
			else if(!strncmp(argv[2], "OFF", 3))
			{
				usrInfo.value = 1;
			}
			
			en1002_gpio_op(&usrInfo, OUT, clientfd);
		}
		else
		{
			printf("error input status\n");
			return -1;
		}
	}
	else
	{
		printf("error gpio name\n");
		return -1;
	}
}

void *thread_func(void* arg)
{
	thread_arg_t *info;
	char usrBuf[USR_BUF_MAX_SIZE];
	ssize_t nbytes;

	info = (thread_arg_t *)arg;
	memset(&usrBuf, 0, sizeof(en1002_gpio_t));

	while(1)
	{
		/*接收客户端数据*/
		nbytes = recv(info->clientfd, usrBuf, USR_BUF_MAX_SIZE, 0);
		usrBuf[nbytes - 1] = '\0';
		
		if(nbytes < 0)
		{
	        perror("recv error");
	        break;
	    }
		else if (nbytes == 0)
		{
	        printf("peer exit\n");
	        break;
	    }
		else
		{
	        analysis_usrBuf(usrBuf, info->clientfd);
	    }
	}

    free (arg);
    
	pthread_exit(NULL);  
}


int main(int argc, char *argv[])
{
    int serverfd, clientfd;
    socklen_t addrlen = sizeof(struct sockaddr);
    struct sockaddr_in servaddr, cliaddr;
    thread_arg_t *arg;
	pthread_t tid;


	/*创建本地套接字接口*/
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd < 0)
    {
        perror("socket error");
        return -1;
    }  

	/*填充本地地址信息*/
	memset(&servaddr, 0, addrlen);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

	/*绑定本地套接字*/
    if(bind(serverfd, (struct sockaddr *)&servaddr, addrlen) < 0)
	{
        perror("bind error");
        exit(-1);
    }

	/*监听套接字*/
	if(listen(serverfd, 5) < 0)
	{
        perror("listen error");
        exit(-1);
    }

	while(1)
    {
		/*接收连接*/
    	clientfd = accept(serverfd, (struct sockaddr *)&cliaddr, &addrlen);
    	arg = (thread_arg_t *)malloc(sizeof(thread_arg_t));
    	arg->clientfd = clientfd;
    	memcpy((void*)&arg->client, &cliaddr, sizeof(cliaddr));  

		if(pthread_create(&tid, NULL, thread_func, (void*)arg))
		{
			perror("Pthread_create error");  
			break;
		}
    }

	close(serverfd);
}

