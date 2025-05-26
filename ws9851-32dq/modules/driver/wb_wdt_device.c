#include <linux/module.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <wb_wdt.h>
#include <wb_logic_dev_common.h>
#include <wb_bsp_kernel_debug.h>

static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);

static wb_wdt_device_t wb_wdt_device_data_0 = {
    .feed_wdt_type = 2,
    .hw_margin = 180000,
    .feed_time = 30000,
    .config_dev_name = "/dev/cpld3",
    .config_mode = LOGIC_FEED_WDT_MODE,
    .priv_func_mode = SYMBOL_I2C_DEV_MODE,
    .enable_reg = 0xf2,
    .enable_val = 0x1,
    .disable_val = 0x0,
    .enable_mask = 0x1,
    .timeout_cfg_reg = 0xf3,
    .hw_algo = "toggle",
    .wdt_config_mode.logic_wdt = {
        .feed_dev_name = "/dev/cpld3",
        .feed_reg = 0xf2,
        .active_val = 0x1,
        .logic_func_mode = SYMBOL_I2C_DEV_MODE,
    },
    .timer_accuracy = 6000,          /* 6s */
};

static wb_wdt_device_t wb_wdt_device_data_1 = {
    .feed_wdt_type = 1,
    .hw_margin = 90000,
    .feed_time = 9000,
    .config_dev_name = "/dev/cpld1",
    .config_mode = LOGIC_FEED_WDT_MODE,
    .priv_func_mode = SYMBOL_IO_DEV_MODE,
    .enable_reg = 0x4f,
    .enable_val = 0x1,
    .disable_val = 0x0,
    .enable_mask = 0x1,
    .timeout_cfg_reg = 0x50,
    .hw_algo = "level",
    .wdt_config_mode.logic_wdt = {
        .feed_dev_name = "/dev/cpld1",
        .feed_reg = 0xac,
        .active_val = 0x0,
        .logic_func_mode = SYMBOL_IO_DEV_MODE,
    },
    .timer_accuracy = 1600,          /* 1.6s */
};

static void wb_wdt_device_release(struct device *dev)
{
    return;
}

static struct platform_device wb_wdt_device[] = {
    {
        .name   = "wb_wdt",
        .id = 0,
        .dev    = {
            .platform_data  = &wb_wdt_device_data_0,
            .release = wb_wdt_device_release,
        },
    },
    {
        .name   = "wb_wdt",
        .id = 1,
        .dev    = {
            .platform_data  = &wb_wdt_device_data_1,
            .release = wb_wdt_device_release,
        },
    }
};

static int __init wb_wdt_device_init(void)
{
    int i;
    int ret = 0;
    wb_wdt_device_t *wdt_device_data;

    DEBUG_VERBOSE("enter!\n");
    for (i = 0; i < ARRAY_SIZE(wb_wdt_device); i++) {
        wdt_device_data = wb_wdt_device[i].dev.platform_data;
        ret = platform_device_register(&wb_wdt_device[i]);
        if (ret < 0) {
            wdt_device_data->device_flag = -1; /* device register failed, set flag -1 */
            printk(KERN_ERR "rg-wdt.%d register failed!\n", i + 1);
        } else {
            wdt_device_data->device_flag = 0; /* device register suucess, set flag 0 */
        }
    }
    return 0;
}

static void __exit wb_wdt_device_exit(void)
{
    int i;
    wb_wdt_device_t *wdt_device_data;

    DEBUG_VERBOSE("enter!\n");
    for (i = ARRAY_SIZE(wb_wdt_device) - 1; i >= 0; i--) {
        wdt_device_data = wb_wdt_device[i].dev.platform_data;
        if (wdt_device_data->device_flag == 0) { /* device register success, need unregister */
            platform_device_unregister(&wb_wdt_device[i]);
        }
    }
}

module_init(wb_wdt_device_init);
module_exit(wb_wdt_device_exit);
MODULE_DESCRIPTION("WB WDT Devices");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("support");
