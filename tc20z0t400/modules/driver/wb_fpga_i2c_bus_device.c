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

/* fpga i2c master */
static fpga_i2c_bus_device_t fpga0_i2c_bus_device_data0 = {
    .adap_nr                 = 2,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x2000,
    .i2c_filter              = 0x2004,
    .i2c_stretch             = 0x2008,
    .i2c_ext_9548_exits_flag = 0x200c,
    .i2c_ext_9548_addr       = 0x2010,
    .i2c_ext_9548_chan       = 0x2014,
    .i2c_in_9548_chan        = 0x2018,
    .i2c_slave               = 0x201c,
    .i2c_reg                 = 0x2020,
    .i2c_reg_len             = 0x2030,
    .i2c_data_len            = 0x2034,
    .i2c_ctrl                = 0x2038,
    .i2c_status              = 0x203c,
    .i2c_err_vec             = 0x2048,
    .i2c_data_buf            = 0x2100,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x207c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_i2c_bus_device_data1 = {
    .adap_nr                 = 3,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x2200,
    .i2c_filter              = 0x2204,
    .i2c_stretch             = 0x2208,
    .i2c_ext_9548_exits_flag = 0x220c,
    .i2c_ext_9548_addr       = 0x2210,
    .i2c_ext_9548_chan       = 0x2214,
    .i2c_in_9548_chan        = 0x2218,
    .i2c_slave               = 0x221c,
    .i2c_reg                 = 0x2220,
    .i2c_reg_len             = 0x2230,
    .i2c_data_len            = 0x2234,
    .i2c_ctrl                = 0x2238,
    .i2c_status              = 0x223c,
    .i2c_err_vec             = 0x2248,
    .i2c_data_buf            = 0x2300,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x227c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_i2c_bus_device_data2 = {
    .adap_nr                 = 4,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x2400,
    .i2c_filter              = 0x2404,
    .i2c_stretch             = 0x2408,
    .i2c_ext_9548_exits_flag = 0x240c,
    .i2c_ext_9548_addr       = 0x2410,
    .i2c_ext_9548_chan       = 0x2414,
    .i2c_in_9548_chan        = 0x2418,
    .i2c_slave               = 0x241c,
    .i2c_reg                 = 0x2420,
    .i2c_reg_len             = 0x2430,
    .i2c_data_len            = 0x2434,
    .i2c_ctrl                = 0x2438,
    .i2c_status              = 0x243c,
    .i2c_err_vec             = 0x2448,
    .i2c_data_buf            = 0x2500,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x247c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_i2c_bus_device_data3 = {
    .adap_nr                 = 5,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x2600,
    .i2c_filter              = 0x2604,
    .i2c_stretch             = 0x2608,
    .i2c_ext_9548_exits_flag = 0x260c,
    .i2c_ext_9548_addr       = 0x2610,
    .i2c_ext_9548_chan       = 0x2614,
    .i2c_in_9548_chan        = 0x2618,
    .i2c_slave               = 0x261c,
    .i2c_reg                 = 0x2620,
    .i2c_reg_len             = 0x2630,
    .i2c_data_len            = 0x2634,
    .i2c_ctrl                = 0x2638,
    .i2c_status              = 0x263c,
    .i2c_err_vec             = 0x2648,
    .i2c_data_buf            = 0x2700,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x267c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_i2c_bus_device_data4 = {
    .adap_nr                 = 6,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x2800,
    .i2c_filter              = 0x2804,
    .i2c_stretch             = 0x2808,
    .i2c_ext_9548_exits_flag = 0x280c,
    .i2c_ext_9548_addr       = 0x2810,
    .i2c_ext_9548_chan       = 0x2814,
    .i2c_in_9548_chan        = 0x2818,
    .i2c_slave               = 0x281c,
    .i2c_reg                 = 0x2820,
    .i2c_reg_len             = 0x2830,
    .i2c_data_len            = 0x2834,
    .i2c_ctrl                = 0x2838,
    .i2c_status              = 0x283c,
    .i2c_err_vec             = 0x2848,
    .i2c_data_buf            = 0x2900,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x287c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_i2c_bus_device_data5 = {
    .adap_nr                 = 7,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x2a00,
    .i2c_filter              = 0x2a04,
    .i2c_stretch             = 0x2a08,
    .i2c_ext_9548_exits_flag = 0x2a0c,
    .i2c_ext_9548_addr       = 0x2a10,
    .i2c_ext_9548_chan       = 0x2a14,
    .i2c_in_9548_chan        = 0x2a18,
    .i2c_slave               = 0x2a1c,
    .i2c_reg                 = 0x2a20,
    .i2c_reg_len             = 0x2a30,
    .i2c_data_len            = 0x2a34,
    .i2c_ctrl                = 0x2a38,
    .i2c_status              = 0x2a3c,
    .i2c_err_vec             = 0x2a48,
    .i2c_data_buf            = 0x2b00,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x2a7c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_dom_i2c_bus_device_data0 = {
    .adap_nr                 = 8,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x3000,
    .i2c_filter              = 0x3004,
    .i2c_stretch             = 0x3008,
    .i2c_ext_9548_exits_flag = 0x300c,
    .i2c_ext_9548_addr       = 0x3010,
    .i2c_ext_9548_chan       = 0x3014,
    .i2c_in_9548_chan        = 0x3018,
    .i2c_slave               = 0x301c,
    .i2c_reg                 = 0x3020,
    .i2c_reg_len             = 0x3030,
    .i2c_data_len            = 0x3034,
    .i2c_ctrl                = 0x3038,
    .i2c_status              = 0x303c,
    .i2c_err_vec             = 0x3048,
    .i2c_data_buf            = 0x3100,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x307c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_dom_i2c_bus_device_data1 = {
    .adap_nr                 = 9,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x3200,
    .i2c_filter              = 0x3204,
    .i2c_stretch             = 0x3208,
    .i2c_ext_9548_exits_flag = 0x320c,
    .i2c_ext_9548_addr       = 0x3210,
    .i2c_ext_9548_chan       = 0x3214,
    .i2c_in_9548_chan        = 0x3218,
    .i2c_slave               = 0x321c,
    .i2c_reg                 = 0x3220,
    .i2c_reg_len             = 0x3230,
    .i2c_data_len            = 0x3234,
    .i2c_ctrl                = 0x3238,
    .i2c_status              = 0x323c,
    .i2c_err_vec             = 0x3248,
    .i2c_data_buf            = 0x3300,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x327c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_dom_i2c_bus_device_data2 = {
    .adap_nr                 = 10,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x3400,
    .i2c_filter              = 0x3404,
    .i2c_stretch             = 0x3408,
    .i2c_ext_9548_exits_flag = 0x340c,
    .i2c_ext_9548_addr       = 0x3410,
    .i2c_ext_9548_chan       = 0x3414,
    .i2c_in_9548_chan        = 0x3418,
    .i2c_slave               = 0x341c,
    .i2c_reg                 = 0x3420,
    .i2c_reg_len             = 0x3430,
    .i2c_data_len            = 0x3434,
    .i2c_ctrl                = 0x3438,
    .i2c_status              = 0x343c,
    .i2c_err_vec             = 0x3448,
    .i2c_data_buf            = 0x3500,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x347c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_dom_i2c_bus_device_data3 = {
    .adap_nr                 = 11,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x3600,
    .i2c_filter              = 0x3604,
    .i2c_stretch             = 0x3608,
    .i2c_ext_9548_exits_flag = 0x360c,
    .i2c_ext_9548_addr       = 0x3610,
    .i2c_ext_9548_chan       = 0x3614,
    .i2c_in_9548_chan        = 0x3618,
    .i2c_slave               = 0x361c,
    .i2c_reg                 = 0x3620,
    .i2c_reg_len             = 0x3630,
    .i2c_data_len            = 0x3634,
    .i2c_ctrl                = 0x3638,
    .i2c_status              = 0x363c,
    .i2c_err_vec             = 0x3648,
    .i2c_data_buf            = 0x3700,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x367c,
    .i2c_reset_on            = 0x00000001,
    .i2c_reset_off           = 0x00000000,
    .i2c_rst_delay_b         = 0, /* delay time before reset(us) */
    .i2c_rst_delay           = 1, /* reset time(us) */
    .i2c_rst_delay_a         = 1, /* delay time after reset(us) */
};

static fpga_i2c_bus_device_t fpga0_dom_i2c_bus_device_data4 = {
    .adap_nr                 = 12,
    .i2c_timeout             = 3000,
    .i2c_scale               = 0x3800,
    .i2c_filter              = 0x3804,
    .i2c_stretch             = 0x3808,
    .i2c_ext_9548_exits_flag = 0x380c,
    .i2c_ext_9548_addr       = 0x3810,
    .i2c_ext_9548_chan       = 0x3814,
    .i2c_in_9548_chan        = 0x3818,
    .i2c_slave               = 0x381c,
    .i2c_reg                 = 0x3820,
    .i2c_reg_len             = 0x3830,
    .i2c_data_len            = 0x3834,
    .i2c_ctrl                = 0x3838,
    .i2c_status              = 0x383c,
    .i2c_err_vec             = 0x3848,
    .i2c_data_buf            = 0x3900,
    .i2c_data_buf_len        = 256,
    .dev_name                = "/dev/fpga0",
    .i2c_scale_value         = 0x4e,
    .i2c_filter_value        = 0x7c,
    .i2c_stretch_value       = 0x7c,
    .i2c_func_mode           = 2,
    .i2c_adap_reset_flag     = 1,
    .i2c_reset_addr          = 0x387c,
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
    {
        .name   = "wb-fpga-i2c",
        .id = 3,
        .dev    = {
            .platform_data  = &fpga0_i2c_bus_device_data2,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 4,
        .dev    = {
            .platform_data  = &fpga0_i2c_bus_device_data3,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 5,
        .dev    = {
            .platform_data  = &fpga0_i2c_bus_device_data4,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 6,
        .dev    = {
            .platform_data  = &fpga0_i2c_bus_device_data5,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 7,
        .dev    = {
            .platform_data  = &fpga0_dom_i2c_bus_device_data0,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 8,
        .dev    = {
            .platform_data  = &fpga0_dom_i2c_bus_device_data1,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 9,
        .dev    = {
            .platform_data  = &fpga0_dom_i2c_bus_device_data2,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 10,
        .dev    = {
            .platform_data  = &fpga0_dom_i2c_bus_device_data3,
            .release = wb_fpga_i2c_bus_device_release,
        },
    },
    {
        .name   = "wb-fpga-i2c",
        .id = 11,
        .dev    = {
            .platform_data  = &fpga0_dom_i2c_bus_device_data4,
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
