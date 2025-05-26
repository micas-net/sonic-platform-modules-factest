#include <linux/module.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <fpga_i2c.h>

static int g_wb_fpga_i2c_debug = 0;
static int g_wb_fpga_i2c_error = 0;

module_param(g_wb_fpga_i2c_debug, int, S_IRUGO | S_IWUSR);
module_param(g_wb_fpga_i2c_error, int, S_IRUGO | S_IWUSR);

#define WB_FPGA_I2C_DEBUG_VERBOSE(fmt, args...) do {                                        \
    if (g_wb_fpga_i2c_debug) { \
        printk(KERN_INFO "[WB_FPGA_I2C][VER][func:%s line:%d]\r\n"fmt, __func__, __LINE__, ## args); \
    } \
} while (0)

#define WB_FPGA_I2C_DEBUG_ERROR(fmt, args...) do {                                        \
    if (g_wb_fpga_i2c_error) { \
        printk(KERN_ERR "[WB_FPGA_I2C][ERR][func:%s line:%d]\r\n"fmt, __func__, __LINE__, ## args); \
    } \
} while (0)

/* CPLD-I2C-MASTER-1 */
static fpga_i2c_bus_device_t fpga0_i2c_bus_device_data0 = {
    .adap_nr                 = 2,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x200,
    .i2c_filter              = 0x204,
    .i2c_stretch             = 0x208,
    .i2c_ext_9548_exits_flag = 0x20c,
    .i2c_ext_9548_addr       = 0x210,
    .i2c_ext_9548_chan       = 0x214,
    .i2c_in_9548_chan        = 0x218,
    .i2c_slave               = 0x21c,
    .i2c_reg                 = 0x220,
    .i2c_reg_len             = 0x230,
    .i2c_data_len            = 0x234,
    .i2c_ctrl                = 0x238,
    .i2c_status              = 0x23c,
    .i2c_err_vec             = 0x248,
    .i2c_data_buf            = 0x300,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/cpld2",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 6,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x27c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

/* CPLD-I2C-MASTER-2 */
static fpga_i2c_bus_device_t fpga0_i2c_bus_device_data1 = {
    .adap_nr                 = 3,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x400,
    .i2c_filter              = 0x404,
    .i2c_stretch             = 0x408,
    .i2c_ext_9548_exits_flag = 0x40c,
    .i2c_ext_9548_addr       = 0x410,
    .i2c_ext_9548_chan       = 0x414,
    .i2c_in_9548_chan        = 0x418,
    .i2c_slave               = 0x41c,
    .i2c_reg                 = 0x420,
    .i2c_reg_len             = 0x430,
    .i2c_data_len            = 0x434,
    .i2c_ctrl                = 0x438,
    .i2c_status              = 0x43c,
    .i2c_err_vec             = 0x448,
    .i2c_data_buf            = 0x500,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/cpld2",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 6,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x47c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static void wb_fpga_i2c_bus_device_release(struct device *dev)
{
    return;
}

static struct platform_device fpga_i2c_bus_device[] = {
    {
        .name   = "wb-fpga-i2c",
        .id = 1,
        .dev    = {
            .platform_data  = &fpga0_i2c_bus_device_data0,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 2,
        .dev    = {
            .platform_data  = &fpga0_i2c_bus_device_data1,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
};

static int __init wb_fpga_i2c_bus_device_init(void)
{
    int i;
    int ret = 0;
    fpga_i2c_bus_device_t *fpga_i2c_bus_device_data;

    WB_FPGA_I2C_DEBUG_VERBOSE("enter!\n");
    for (i = 0; i < ARRAY_SIZE(fpga_i2c_bus_device); i++) {
        fpga_i2c_bus_device_data = fpga_i2c_bus_device[i].dev.platform_data;
        ret = platform_device_register(&fpga_i2c_bus_device[i]);
        if (ret < 0) {
            fpga_i2c_bus_device_data->device_flag = -1; /* device register failed, set flag -1 */
            printk(KERN_ERR "wb-fpga-i2c.%d register failed!\n", i + 1);
        } else {
            fpga_i2c_bus_device_data->device_flag = 0; /* device register suucess, set flag 0 */
        }
    }
    return 0;
}

static void __exit wb_fpga_i2c_bus_device_exit(void)
{
    int i;
    fpga_i2c_bus_device_t *fpga_i2c_bus_device_data;

    WB_FPGA_I2C_DEBUG_VERBOSE("enter!\n");
    for (i = ARRAY_SIZE(fpga_i2c_bus_device) - 1; i >= 0; i--) {
        fpga_i2c_bus_device_data = fpga_i2c_bus_device[i].dev.platform_data;
        if (fpga_i2c_bus_device_data->device_flag == 0) { /* device register success, need unregister */
            platform_device_unregister(&fpga_i2c_bus_device[i]);
        }
    }
}

module_init(wb_fpga_i2c_bus_device_init);
module_exit(wb_fpga_i2c_bus_device_exit);
MODULE_DESCRIPTION("FPGA I2C Devices");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("support");
