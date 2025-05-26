#include <linux/module.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <wb_uio_irq.h>
#include <wb_bsp_kernel_debug.h>

static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);

/* MAC CPLD */
static uio_irq_device_t uio_irq_device_data0 = {
    .irq_info_mode = 1,     /* 0: pirq_line mode; 1: gpio mode*/
    .gpio = 27,
    .irq_type = 0x00000008,
};

static void wb_uio_irq_device_release(struct device *dev)
{
    return;
}

static struct platform_device platform_uio_irq_device[] = {
    {
        .name   = "uio-irq",
        .id = 1,
        .dev    = {
            .platform_data  = &uio_irq_device_data0,
            .release = wb_uio_irq_device_release,
        },
    },
};

static int __init wb_uio_irq_device_init(void)
{
    int i;
    int ret = 0;
    uio_irq_device_t *uio_irq_device_data;

    DEBUG_VERBOSE("enter!\n");
    for (i = 0; i < ARRAY_SIZE(platform_uio_irq_device); i++) {
        uio_irq_device_data = platform_uio_irq_device[i].dev.platform_data;
        ret = platform_device_register(&platform_uio_irq_device[i]);
        if (ret < 0) {
            uio_irq_device_data->device_flag = -1; /* device register failed, set flag -1 */
            printk(KERN_ERR "uio-irq.%d register failed!\n", i + 1);
        } else {
            uio_irq_device_data->device_flag = 0; /* device register suucess, set flag 0 */
        }
    }
    return 0;
}

static void __exit wb_uio_irq_device_exit(void)
{
    int i;
    uio_irq_device_t *uio_irq_device_data;

    DEBUG_VERBOSE("enter!\n");
    for (i = ARRAY_SIZE(platform_uio_irq_device) - 1; i >= 0; i--) {
        uio_irq_device_data = platform_uio_irq_device[i].dev.platform_data;
        if (uio_irq_device_data->device_flag == 0) { /* device register success, need unregister */
            platform_device_unregister(&platform_uio_irq_device[i]);
        }
    }
}

module_init(wb_uio_irq_device_init);
module_exit(wb_uio_irq_device_exit);
MODULE_DESCRIPTION("RG uio irq Devices");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sonic_rd@ruijie.com.cn");
