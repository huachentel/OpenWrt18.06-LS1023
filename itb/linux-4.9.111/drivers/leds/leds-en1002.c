
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

#include <linux/gpio.h>
#include <linux/leds.h>

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
#define WIFI_PWR_EN		GPIO_EN1002(GPIO1_NUM, 22)
#define WIFI_PWR_USB_EN		GPIO_EN1002(GPIO1_NUM, 18)

#define GPIO_RST		GPIO_EN1002(GPIO2_NUM, 27)

#define LED_WIFI_5G		GPIO_EN1002(GPIO4_NUM, 10)
#define CLOUD_LED	GPIO_EN1002(GPIO4_NUM, 11)

static struct gpio_led gpio_leds[] = {
    {
        .name = "sys",
		.default_trigger = "timer",
        .gpio = CPU_LED, 
        .default_state = LEDS_GPIO_DEFSTATE_ON, // 默认LED亮
        .active_low = 1, // 低电平亮       
    },
    {
        .name = "cloud",
		.default_trigger = "timer",
        .gpio = CLOUD_LED,
        .default_state = LEDS_GPIO_DEFSTATE_ON, 
        .active_low = 1,          
    },
    {
        .name = "lte_0",		
        .gpio = LTE_LED_0,
        .default_state = LEDS_GPIO_DEFSTATE_OFF, 
        .active_low = 1,          
    },
    {
        .name = "lte_1",		
        .gpio = LTE_LED_2,
        .default_state = LEDS_GPIO_DEFSTATE_OFF, 
        .active_low = 1,          
    },
    {
        .name = "lte_2",		
        .gpio = LTE_LED_1,
        .default_state = LEDS_GPIO_DEFSTATE_OFF, 
        .active_low = 1,          
    },
    {
        .name = "lte_pwr",		
        .gpio = LTE_PWR_EN,
        .default_state = LEDS_GPIO_DEFSTATE_ON, 
        .active_low = 0,          
    },
    
/*
    {
        .name = "wifi_pwr_5g",		
        .gpio = WIFI_PWR_EN,
        .default_state = LEDS_GPIO_DEFSTATE_ON, 
        .active_low = 0,          
    },
    {
        .name = "wifi_pwr_2.4g",		
        .gpio = WIFI_PWR_USB_EN,
        .default_state = LEDS_GPIO_DEFSTATE_ON, 
        .active_low = 0,          
    },
*/
    {
        .name = "wifi_5g",		
        .gpio = LED_WIFI_5G,
        .default_state = LEDS_GPIO_DEFSTATE_ON, 
        .active_low = 0,          
    },
/*
   	{
        .name = "wifi_2.4g",		
        .gpio = LED_WIFI_24G,
        .default_state = LEDS_GPIO_DEFSTATE_ON, 
        .active_low = 0,          
    },
*/ 
};

 
static struct gpio_led_platform_data gpio_led_info = {
     .leds          = gpio_leds,
     .num_leds     = ARRAY_SIZE(gpio_leds),
};

static struct platform_device leds_gpio = {
    .name = "leds-gpio",
    .id = -1,
    .dev = {
        .platform_data = &gpio_led_info,      
    },
};

static int __init en1002_led_drv_init(void)
{	
	platform_device_register(&leds_gpio);

	return 0;
}

static int __init en1002_led_drv_exit(void)
{	
	platform_device_unregister(&leds_gpio);

	return 0;
}

module_init(en1002_led_drv_init);
//module_exit(en1002_led_drv_exit);

MODULE_AUTHOR("liji");
MODULE_DESCRIPTION("en1002 led ctrl");
MODULE_LICENSE("GPL");

