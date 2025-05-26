/*
 * Copyright(C) 2021 SONIC. All rights reserved.
 */
/*
 * wb_firmware_upgrade.c
 * Original Author: sonic 2021-03-17
 *
 * ko for firmware device
 * History
 *  [Version]        [Author]                   [Date]            [Description]
 *    v1.0      sonic        2021-05-07          Initial version
 */
#include <linux/module.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <firmware_upgrade.h>
#include <linux/version.h>
#include <linux/gpio.h>

#define GPIO_D1700_NUM      (237)                               /* D1700 CPU GPIO number */
#define GPIO_D1700_OFFSET   (ARCH_NR_GPIOS - GPIO_D1700_NUM)    /* Formula of gpio base number in Kernel */

static int g_wb_firmware_upgrade_debug = 0;
static int g_wb_firmware_upgrade_error = 0;

module_param(g_wb_firmware_upgrade_debug, int, S_IRUGO | S_IWUSR);
module_param(g_wb_firmware_upgrade_error, int, S_IRUGO | S_IWUSR);

#define WB_FIRMWARE_UPGRADE_DEBUG_VERBOSE(fmt, args...) do {                                        \
    if (g_wb_firmware_upgrade_debug) { \
        printk(KERN_INFO "[WB_FIRMWARE_UPGRADE][VER][func:%s line:%d]\r\n"fmt, __func__, __LINE__, ## args); \
    } \
} while (0)

#define WB_FIRMWARE_UPGRADE_DEBUG_ERROR(fmt, args...) do {                                        \
    if (g_wb_firmware_upgrade_error) { \
        printk(KERN_ERR "[WB_FIRMWARE_UPGRADE][ERR][func:%s line:%d]\r\n"fmt, __func__, __LINE__, ## args); \
    } \
} while (0)

/* phy fpga */
static firmware_upgrade_device_t firmware_upgrade_device_data0 = {
    .type                = "SPI_LOGIC",
    .chain               = 5,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0xa00,
        .flash_base   = 0x2f0000,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 0,
};

/* phy fpga shaopian */
static firmware_upgrade_device_t firmware_upgrade_device_data1 = {
    .type                = "SPI_LOGIC",
    .chain               = 6,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0xa00,
        .flash_base   = 0x0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 0,
};

static firmware_upgrade_device_t firmware_upgrade_device_data_phy1 = {
    .type                = "SPI_LOGIC",
    .chain               = 7,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x4000,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xfffffffe,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x1,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy2 = {
    .type                = "SPI_LOGIC",
    .chain               = 8,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x4200,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xfffffffd,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x2,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy3 = {
    .type                = "SPI_LOGIC",
    .chain               = 9,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x4400,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xfffffffb,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x4,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy4 = {
    .type                = "SPI_LOGIC",
    .chain               = 10,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x4600,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xfffffff7,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x8,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy5 = {
    .type                = "SPI_LOGIC",
    .chain               = 11,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x4800,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xffffffef,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x10,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy6 = {
    .type                = "SPI_LOGIC",
    .chain               = 12,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x4A00,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xffffffdf,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x20,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy7 = {
    .type                = "SPI_LOGIC",
    .chain               = 13,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x4C00,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xffffffbf,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x40,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy8 = {
    .type                = "SPI_LOGIC",
    .chain               = 14,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x4E00,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xffffff7f,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x80,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy9 = {
    .type                = "SPI_LOGIC",
    .chain               = 15,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x5000,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xfffffeff,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x100,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy10 = {
    .type                = "SPI_LOGIC",
    .chain               = 16,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x5200,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xfffffdff,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x200,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy11 = {
    .type                = "SPI_LOGIC",
    .chain               = 17,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x5400,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xfffffbff,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x400,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy12 = {
    .type                = "SPI_LOGIC",
    .chain               = 18,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x5600,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xfffff7ff,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x800,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy13 = {
    .type                = "SPI_LOGIC",
    .chain               = 19,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x5800,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xffffefff,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x1000,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy14 = {
    .type                = "SPI_LOGIC",
    .chain               = 20,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x5A00,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xffffdfff,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x2000,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy15 = {
    .type                = "SPI_LOGIC",
    .chain               = 21,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x5C00,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xffffbfff,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x4000,
    .en_logic_width[0]   = 0x4,
};
static firmware_upgrade_device_t firmware_upgrade_device_data_phy16 = {
    .type                = "SPI_LOGIC",
    .chain               = 22,
    .chip_index          = 1,
    .upg_type.sysfs = {
        .dev_name     = "/dev/fpga0",
        .ctrl_base    = 0x5E00,
        .flash_base   = 0,
        .test_base    = 0x7F0000,
        .test_size    = 0x10000,
    },
    .en_gpio_num        = 0,
    .en_logic_num       = 1,

    .en_logic_dev[0]     = "/dev/fpga0",
    .en_logic_addr[0]    = 0xa0,
    .en_logic_mask[0]    = 0xffff7fff,
    .en_logic_en_val[0]  = 0x0,
    .en_logic_dis_val[0] = 0x8000,
    .en_logic_width[0]   = 0x4,
};

static void firmware_device_release(struct device *dev)
{
    return;
}

static struct platform_device firmware_upgrade_device[] = {
    {
        .name   = "firmware_sysfs",
        .id = 0,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data0,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 1,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data1,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 2,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy1,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 3,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy2,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 4,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy3,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 5,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy4,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 6,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy5,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 7,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy6,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 8,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy7,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 9,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy8,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 10,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy9,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 11,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy10,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 12,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy11,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 13,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy12,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 14,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy13,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 15,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy14,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 16,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy15,
            .release = firmware_device_release,
        },
    },
    {
        .name   = "firmware_sysfs",
        .id = 17,
        .dev    = {
            .platform_data  = &firmware_upgrade_device_data_phy16,
            .release = firmware_device_release,
        },
    },
};

static int __init firmware_upgrade_device_init(void)
{
    int i;
    int ret = 0;
    firmware_upgrade_device_t *firmware_upgrade_device_data;

    WB_FIRMWARE_UPGRADE_DEBUG_VERBOSE("enter!\n");
    for (i = 0; i < ARRAY_SIZE(firmware_upgrade_device); i++) {
        firmware_upgrade_device_data = firmware_upgrade_device[i].dev.platform_data;
        ret = platform_device_register(&firmware_upgrade_device[i]);
        if (ret < 0) {
            firmware_upgrade_device_data->device_flag = -1; /* device register failed, set flag -1 */
            printk(KERN_ERR "firmware_upgrade_device id%d register failed!\n", i + 1);
        } else {
            firmware_upgrade_device_data->device_flag = 0; /* device register suucess, set flag 0 */
        }
    }
    return 0;
}

static void __exit firmware_upgrade_device_exit(void)
{
    int i;
    firmware_upgrade_device_t *firmware_upgrade_device_data;

    WB_FIRMWARE_UPGRADE_DEBUG_VERBOSE("enter!\n");
    for (i = ARRAY_SIZE(firmware_upgrade_device) - 1; i >= 0; i--) {
        firmware_upgrade_device_data = firmware_upgrade_device[i].dev.platform_data;
        if (firmware_upgrade_device_data->device_flag == 0) { /* device register success, need unregister */
            platform_device_unregister(&firmware_upgrade_device[i]);
        }
    }
}

module_init(firmware_upgrade_device_init);
module_exit(firmware_upgrade_device_exit);
MODULE_DESCRIPTION("FIRMWARE UPGRADE Devices");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sonic");
