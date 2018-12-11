#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
//#include <mach/gpio.h>

#define GPIO1_NUM		480
#define GPIO2_NUM		448
#define GPIO4_NUM		384

#define GPIO_EN1002(a, b)	(a + b)
#define GPIO_RST		GPIO_EN1002(GPIO2_NUM, 27)


static struct gpio_keys_button btns[] = {
     {
          .desc          = "reset",
          .type          = EV_KEY,
          .code          = KEY_RESTART,
          .debounce_interval = 100,
          .gpio          = GPIO_RST,
          .active_low     = 1,
     },
};

struct gpio_keys_platform_data pdata = {
    .buttons = btns,
    .nbuttons = ARRAY_SIZE(btns),
    .rep = 1,
    .name = "reset",
};

struct platform_device key_gpio = {
    .name = "gpio-keys", 
    .id = -1,
    .dev = {
        .platform_data = &pdata,
    },
};
		
static int __init en1002_key_drv_init(void)
{	
	platform_device_register(&key_gpio);

	return 0;
}

static int __init en1002_key_drv_exit(void)
{	
	platform_device_unregister(&key_gpio);

	return 0;
}

module_init(en1002_key_drv_init);
//module_exit(en1002_led_drv_exit);

MODULE_AUTHOR("liji");
MODULE_DESCRIPTION("en1002 key ctrl");
MODULE_LICENSE("GPL");


