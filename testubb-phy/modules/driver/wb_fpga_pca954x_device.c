#include <linux/module.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <fpga_i2c.h>

static int g_wb_fpga_pca954x_device_debug = 0;
static int g_wb_fpga_pca954x_device_error = 0;

module_param(g_wb_fpga_pca954x_device_debug, int, S_IRUGO | S_IWUSR);
module_param(g_wb_fpga_pca954x_device_error, int, S_IRUGO | S_IWUSR);

#define WB_FPGA_PCA954X_DEVICE_DEBUG_VERBOSE(fmt, args...) do {                                        \
    if (g_wb_fpga_pca954x_device_debug) { \
        printk(KERN_INFO "[WB_FPGA_PCA954X_DEVICE][VER][func:%s line:%d]\r\n"fmt, __func__, __LINE__, ## args); \
    } \
} while (0)

#define WB_FPGA_PCA954X_DEVICE_DEBUG_ERROR(fmt, args...) do {                                        \
    if (g_wb_fpga_pca954x_device_error) { \
        printk(KERN_ERR "[WB_FPGA_PCA954X_DEVICE][ERR][func:%s line:%d]\r\n"fmt, __func__, __LINE__, ## args); \
    } \
} while (0)

/* CPLD-I2C-MASTER-1 */
static fpga_pca954x_device_t fpga_pca954x_device_data0 = {
    .i2c_bus = 2,
    .i2c_addr = 0x70,
    .pca9548_base_nr = 12,
    .fpga_9548_flag = 1,
    .fpga_9548_reset_flag = 0,
};

static fpga_pca954x_device_t fpga_pca954x_device_data1 = {
    .i2c_bus = 12,
    .i2c_addr = 0x76,
    .pca9548_base_nr = 28,
    .fpga_9548_flag = 2,
    .fpga_9548_reset_flag = 1,
};

static fpga_pca954x_device_t fpga_pca954x_device_data2 = {
    .i2c_bus = 13,
    .i2c_addr = 0x75,
    .pca9548_base_nr = 36,
    .fpga_9548_flag = 2,
    .fpga_9548_reset_flag = 1,
};

/* CPLD-I2C-MASTER-2 */
static fpga_pca954x_device_t fpga_pca954x_device_data3 = {
    .i2c_bus = 3,
    .i2c_addr = 0x70,
    .pca9548_base_nr = 20,
    .fpga_9548_flag = 1,
    .fpga_9548_reset_flag = 0,
};

static fpga_pca954x_device_t fpga_pca954x_device_data4 = {
    .i2c_bus = 20,
    .i2c_addr = 0x70,
    .pca9548_base_nr = 44,
    .fpga_9548_flag = 2,
    .fpga_9548_reset_flag = 1,
};

static fpga_pca954x_device_t fpga_pca954x_device_data5 = {
    .i2c_bus = 21,
    .i2c_addr = 0x70,
    .pca9548_base_nr = 52,
    .fpga_9548_flag = 2,
    .fpga_9548_reset_flag = 1,
};

/* CPLD-I2C-MASTER-3 */
static fpga_pca954x_device_t fpga_pca954x_device_data6 = {
    .i2c_bus = 22,
    .i2c_addr = 0x70,
    .pca9548_base_nr = 60,
    .fpga_9548_flag = 2,
    .fpga_9548_reset_flag = 1,
};

static fpga_pca954x_device_t fpga_pca954x_device_data7 = {
    .i2c_bus = 23,
    .i2c_addr = 0x70,
    .pca9548_base_nr = 68,
    .fpga_9548_flag = 2,
    .fpga_9548_reset_flag = 1,
};

struct i2c_board_info fpga_pca954x_device_info[] = {
    {
        .type = "wb_fpga_pca9548",
        .platform_data = &fpga_pca954x_device_data0,
    },
    {
        .type = "wb_fpga_pca9548",
        .platform_data = &fpga_pca954x_device_data1,
    },
    {
        .type = "wb_fpga_pca9548",
        .platform_data = &fpga_pca954x_device_data2,
    },
    {
        .type = "wb_fpga_pca9548",
        .platform_data = &fpga_pca954x_device_data3,
    },
    {
        .type = "wb_fpga_pca9548",
        .platform_data = &fpga_pca954x_device_data4,
    },
    {
        .type = "wb_fpga_pca9548",
        .platform_data = &fpga_pca954x_device_data5,
    },
    {
        .type = "wb_fpga_pca9548",
        .platform_data = &fpga_pca954x_device_data6,
    },
    {
        .type = "wb_fpga_pca9548",
        .platform_data = &fpga_pca954x_device_data7,
    },
};

static int __init wb_fpga_pca954x_device_init(void)
{
    int i;
    struct i2c_adapter *adap;
    struct i2c_client *client;
    fpga_pca954x_device_t *fpga_pca954x_device_data;

    WB_FPGA_PCA954X_DEVICE_DEBUG_VERBOSE("enter!\n");
    for (i = 0; i < ARRAY_SIZE(fpga_pca954x_device_info); i++) {
        fpga_pca954x_device_data = fpga_pca954x_device_info[i].platform_data;
        fpga_pca954x_device_info[i].addr = fpga_pca954x_device_data->i2c_addr;
        adap = i2c_get_adapter(fpga_pca954x_device_data->i2c_bus);
        if (adap == NULL) {
            fpga_pca954x_device_data->client = NULL;
            printk(KERN_ERR "get i2c bus %d adapter fail.\n", fpga_pca954x_device_data->i2c_bus);
            continue;
        }
        client = i2c_new_client_device(adap, &fpga_pca954x_device_info[i]);
        if (!client) {
            fpga_pca954x_device_data->client = NULL;
            printk(KERN_ERR "Failed to register fpga pca954x device %d at bus %d!\n",
                fpga_pca954x_device_data->i2c_addr, fpga_pca954x_device_data->i2c_bus);
        } else {
            fpga_pca954x_device_data->client = client;
        }
        i2c_put_adapter(adap);
    }
    return 0;
}

static void __exit wb_fpga_pca954x_device_exit(void)
{
    int i;
    fpga_pca954x_device_t *fpga_pca954x_device_data;

    WB_FPGA_PCA954X_DEVICE_DEBUG_VERBOSE("enter!\n");
    for (i = ARRAY_SIZE(fpga_pca954x_device_info) - 1; i >= 0; i--) {
        fpga_pca954x_device_data = fpga_pca954x_device_info[i].platform_data;
        if (fpga_pca954x_device_data->client) {
            i2c_unregister_device(fpga_pca954x_device_data->client);
            fpga_pca954x_device_data->client = NULL;
        }
    }
}

module_init(wb_fpga_pca954x_device_init);
module_exit(wb_fpga_pca954x_device_exit);
MODULE_DESCRIPTION("FPGA PCA954X Devices");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("support");
