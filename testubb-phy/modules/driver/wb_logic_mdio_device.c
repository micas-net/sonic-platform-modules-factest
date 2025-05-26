#include <linux/module.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <wb_logic_mdio.h>

static int g_wb_logic_mdio_device_debug = 0;
static int g_wb_logic_mdio_device_error = 0;

module_param(g_wb_logic_mdio_device_debug, int, S_IRUGO | S_IWUSR);
module_param(g_wb_logic_mdio_device_error, int, S_IRUGO | S_IWUSR);

#define WB_LOGIC_MDIO_DEVICE_DEBUG_VERBOSE(fmt, args...) do {                                        \
    if (g_wb_logic_mdio_device_debug) { \
        printk(KERN_INFO "[WB_LOGIC_MDIO_DEVICE][VER][func:%s line:%d]\r\n"fmt, __func__, __LINE__, ## args); \
    } \
} while (0)

#define WB_LOGIC_MDIO_DEVICE_DEBUG_ERROR(fmt, args...) do {                                        \
    if (g_wb_logic_mdio_device_error) { \
        printk(KERN_ERR "[WB_LOGIC_MDIO_DEVICE][ERR][func:%s line:%d]\r\n"fmt, __func__, __LINE__, ## args); \
    } \
} while (0)

static logic_mdio_device_t logic_mdio_device_data0 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2000,
};

static logic_mdio_device_t logic_mdio_device_data1 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2010,
};

static logic_mdio_device_t logic_mdio_device_data2 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2020,
};

static logic_mdio_device_t logic_mdio_device_data3 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2030,
};

static logic_mdio_device_t logic_mdio_device_data4 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2040,
};

static logic_mdio_device_t logic_mdio_device_data5 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2050,
};

static logic_mdio_device_t logic_mdio_device_data6 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2060,
};

static logic_mdio_device_t logic_mdio_device_data7 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2070,
};

static logic_mdio_device_t logic_mdio_device_data8 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2080,
};

static logic_mdio_device_t logic_mdio_device_data9 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x2090,
};

static logic_mdio_device_t logic_mdio_device_data10 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x20a0,
};

static logic_mdio_device_t logic_mdio_device_data11 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x20b0,
};

static logic_mdio_device_t logic_mdio_device_data12 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x20c0,
};

static logic_mdio_device_t logic_mdio_device_data13 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x20d0,
};

static logic_mdio_device_t logic_mdio_device_data14 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x20e0,
};

static logic_mdio_device_t logic_mdio_device_data15 = {
    .dev_name = "/dev/fpga0",
    .big_endian = 0,
    .reg_access_mode = 3,   /* pcie */
    .reg_width = 4,
    .reg_offset = 0x20f0,
};

static void wb_logic_mdio_device_release(struct device *dev)
{
    return;
}

static struct platform_device logic_mdio_device[] = {
    {
        .name   = "wb-logic-mdio",
        .id = 1,
        .dev    = {
            .platform_data  = &logic_mdio_device_data0,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 2,
        .dev    = {
            .platform_data  = &logic_mdio_device_data1,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 3,
        .dev    = {
            .platform_data  = &logic_mdio_device_data2,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 4,
        .dev    = {
            .platform_data  = &logic_mdio_device_data3,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 5,
        .dev    = {
            .platform_data  = &logic_mdio_device_data4,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 6,
        .dev    = {
            .platform_data  = &logic_mdio_device_data5,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 7,
        .dev    = {
            .platform_data  = &logic_mdio_device_data6,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 8,
        .dev    = {
            .platform_data  = &logic_mdio_device_data7,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 9,
        .dev    = {
            .platform_data  = &logic_mdio_device_data8,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 10,
        .dev    = {
            .platform_data  = &logic_mdio_device_data9,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 11,
        .dev    = {
            .platform_data  = &logic_mdio_device_data10,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 12,
        .dev    = {
            .platform_data  = &logic_mdio_device_data11,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 13,
        .dev    = {
            .platform_data  = &logic_mdio_device_data12,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 14,
        .dev    = {
            .platform_data  = &logic_mdio_device_data13,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 15,
        .dev    = {
            .platform_data  = &logic_mdio_device_data14,
            .release = wb_logic_mdio_device_release,
        },
    },
    {
        .name   = "wb-logic-mdio",
        .id = 16,
        .dev    = {
            .platform_data  = &logic_mdio_device_data15,
            .release = wb_logic_mdio_device_release,
        },
    },
};

static int __init wb_logic_mdio_device_init(void)
{
    int i;
    int ret = 0;
    logic_mdio_device_t *logic_mdio_device_data;

    WB_LOGIC_MDIO_DEVICE_DEBUG_VERBOSE("enter!\n");
    for (i = 0; i < ARRAY_SIZE(logic_mdio_device); i++) {
        logic_mdio_device_data = logic_mdio_device[i].dev.platform_data;
        ret = platform_device_register(&logic_mdio_device[i]);
        if (ret < 0) {
            logic_mdio_device_data->device_flag = -1; /* device register failed, set flag -1 */
            printk(KERN_ERR "wb-logic-mdio.%d register failed!\n", i + 1);
        } else {
            logic_mdio_device_data->device_flag = 0; /* device register suucess, set flag 0 */
        }
    }
    return 0;
}

static void __exit wb_logic_mdio_device_exit(void)
{
    int i;
    logic_mdio_device_t *logic_mdio_device_data;

    WB_LOGIC_MDIO_DEVICE_DEBUG_VERBOSE("enter!\n");
    for (i = ARRAY_SIZE(logic_mdio_device) - 1; i >= 0; i--) {
        logic_mdio_device_data = logic_mdio_device[i].dev.platform_data;
        if (logic_mdio_device_data->device_flag == 0) { /* device register success, need unregister */
            platform_device_unregister(&logic_mdio_device[i]);
        }
    }
}

module_init(wb_logic_mdio_device_init);
module_exit(wb_logic_mdio_device_exit);
MODULE_DESCRIPTION("LOGIC MDIO Devices");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("support");
