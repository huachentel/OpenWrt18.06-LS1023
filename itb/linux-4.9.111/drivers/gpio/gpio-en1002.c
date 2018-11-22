/*
 * LEDs driver for HuaChen GPIOs
 *
 * Copyright (C) 2007 8D Technologies inc.
 * Raphael Assenat <raph@8d.com>
 * Copyright (C) 2008 Freescale Semiconductor, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Contect liji6630@sina.com for other support
 */
#include<linux/module.h>
#include<linux/init.h>

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>

#include <linux/kernel.h>      
#include <linux/platform_device.h>
#include <linux/cdev.h>         
#include <linux/types.h>       
#include <linux/errno.h>       
#include <linux/string.h>
#include <linux/ctype.h>


#include <linux/io.h>
#include <linux/gpio.h>

#define GPIO1_NUM		480
#define GPIO2_NUM		448
#define GPIO4_NUM		384

#define GPIO_EN1002(a, b)	(a + b)  

#define IGSFP0_NEXIST	GPIO_EN1002(GPIO1_NUM, 23) //GPIO1_23
//#define IGSFP0_SCL		GPIO_EN1002(GPIO1_NUM, 26)
//#define IGSFP0_SDA		GPIO_EN1002(GPIO1_NUM, 25)
#define IGSFP0_DISABLE	GPIO_EN1002(GPIO1_NUM, 24)
#define IGSFP1_DISABLE	GPIO_EN1002(GPIO1_NUM, 27)
//#define IGSFP1_SDA		GPIO_EN1002(GPIO1_NUM, 28)
#define IGSFP1_NEXIST	GPIO_EN1002(GPIO1_NUM, 29)
#define CPU_LED			GPIO_EN1002(GPIO4_NUM, 12)

#define LTE_LED_0		GPIO_EN1002(GPIO4_NUM, 13)
#define LTE_LED_1		GPIO_EN1002(GPIO1_NUM, 21)
#define LTE_LED_2		GPIO_EN1002(GPIO1_NUM, 20)

#define LTE_PWR_EN		GPIO_EN1002(GPIO1_NUM, 19)
#define WIFI_PWR_EN		GPIO_EN1002(GPIO1_NUM, 18)
#define CLOUD_LED		GPIO_EN1002(GPIO1_NUM, 16)

#define GPIO_RST		GPIO_EN1002(GPIO2_NUM, 27)

#define IN	1
#define OUT	0

int en1002_gpio_major;

struct class *en1002_gpio_drv_class;
struct device *en1002_gpio_drv_class_dev;

typedef struct en1002_gpio_s{
	int gpio_id;
	int value;
}en1002_gpio_t;

/*声明一下*/
static void gpio_direction_set(int gpio_id, int direction);
static void gpio_data_set(int gpio_id, int data);
static void gpio_data_get(int gpio_id, int *pdata);
static int en1002_gpio_drv_open(struct inode *inode, struct file *file);
static ssize_t en1002_gpio_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos);
static ssize_t en1002_gpio_drv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);



/*设置gpio方向，如果是输出，0为输出*/
static void gpio_direction_set(int gpio_id, int direction)
{
	switch(direction){
		case 0://out
			gpio_direction_output(gpio_id, 0);
			break;
		case 1:
			gpio_direction_input(gpio_id);
	}
}

/*设置gpio输出状态*/
static void gpio_data_set(int gpio_id, int data)
{
	gpio_set_value(gpio_id, data);
}

/*获取gpio输入状态*/
static void gpio_data_get(int gpio_id, int *pdata)
{
	*pdata = gpio_get_value(gpio_id);
}

static int en1002_gpio_drv_open(struct inode *inode, struct file *file)
{
	/*set init flag for each gpio function ...*/
	gpio_direction_set(IGSFP0_NEXIST, IN);
	gpio_direction_set(IGSFP0_DISABLE, OUT);
	gpio_direction_set(IGSFP1_DISABLE, OUT);
	gpio_direction_set(IGSFP1_NEXIST, IN);
	gpio_direction_set(CPU_LED, OUT);
	gpio_direction_set(LTE_LED_0, OUT);
	gpio_direction_set(LTE_LED_1, OUT);
	gpio_direction_set(LTE_LED_2, OUT);
	gpio_direction_set(LTE_PWR_EN, OUT);
	gpio_direction_set(WIFI_PWR_EN, OUT);
	gpio_direction_set(CLOUD_LED, OUT);
	gpio_direction_set(GPIO_RST, IN);
	
	return 0;
}

static ssize_t en1002_gpio_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	en1002_gpio_t en1002_gpio;
	
	memset((char *)&en1002_gpio, 0, sizeof(en1002_gpio));
	copy_from_user(&en1002_gpio, buf, count);
	
	/*设置输出*/
	gpio_data_set(en1002_gpio.gpio_id, en1002_gpio.value);
	
	return 0;
}

static ssize_t en1002_gpio_drv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	int pdata = 0;
	en1002_gpio_t en1002_gpio;
	
	memset((char *)&en1002_gpio, 0, sizeof(en1002_gpio));
	copy_from_user(&en1002_gpio, buf, count);

	/*读状态并保存*/
	gpio_data_get(en1002_gpio.gpio_id, &pdata);
	en1002_gpio.value = pdata;

	/*传给用户*/	
	copy_to_user(buf, &en1002_gpio, sizeof(en1002_gpio));
	return 0;
}

static struct file_operations en1002_gpio_drv_fops = {
	.owner = THIS_MODULE,
	.open  = en1002_gpio_drv_open,
	.read  = en1002_gpio_drv_read,
	.write = en1002_gpio_drv_write,
};


static int __init en1002_gpio_drv_init(void)
{	
	/*注册,告诉内核*/
	en1002_gpio_major = register_chrdev(0, "en1002_gpio_drv", &en1002_gpio_drv_fops);
	/*创建一个类*/
	en1002_gpio_drv_class = class_create(THIS_MODULE, "en1002_gpiodrv");    
	en1002_gpio_drv_class_dev = device_create(en1002_gpio_drv_class, NULL, MKDEV(en1002_gpio_major, 0), NULL, "en1002_gpio_op"); 

	return 0;
}
static void __exit en1002_gpio_drv_exit(void)
{
	device_unregister(en1002_gpio_drv_class_dev); 
	class_destroy(en1002_gpio_drv_class);
}
 
module_init(en1002_gpio_drv_init);
module_exit(en1002_gpio_drv_exit);




MODULE_AUTHOR("liji");
MODULE_DESCRIPTION("en1002 gpio ctrl");
MODULE_LICENSE("GPL");


