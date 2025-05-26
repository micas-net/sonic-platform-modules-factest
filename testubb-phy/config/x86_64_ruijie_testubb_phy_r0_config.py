#!/usr/bin/python
# -*- coding: UTF-8 -*-
from platform_common import *

# start system modules
STARTMODULE  = {}

DRIVERLISTS = [
    {"name": "r8169", "delay": 0, "removable": 0},
    {"name": "ice", "delay": 0, "removable": 0},
    {"name": "wb_i2c_i801", "delay": 0},
    {"name": "i2c_dev", "delay": 0},
    {"name": "i2c_mux", "delay": 0},
    {"name": "wb_i2c_gpio_device gpio_sda=181 gpio_scl=180 gpio_chip_name=INTC3001:00 bus_num=1", "delay": 0},
    {"name": "platform_common dfd_my_type=0x4112", "delay": 0},
    {"name": "wb_logic_dev_common", "delay":0},
    {"name": "wb_io_dev", "delay": 0},
    {"name": "wb_io_dev_device", "delay": 0},
    {"name": "wb_indirect_dev", "delay": 0},
    {"name": "wb_indirect_dev_device", "delay": 0},
    {"name": "wb_fpga_pcie", "delay": 0},
    {"name": "wb_pcie_dev", "delay": 0},
    {"name": "wb_pcie_dev_device", "delay": 0},
    {"name": "wb_fpga_i2c_bus_drv", "delay": 0},
    {"name": "wb_fpga_i2c_bus_device", "delay": 0},
    {"name": "wb_i2c_mux_pca9641", "delay": 0},
    {"name": "wb_i2c_mux_pca954x", "delay": 0},
    {"name": "wb_i2c_mux_pca954x_device", "delay": 0},
    {"name": "wb_fpga_pca954x_drv", "delay": 0},
    {"name": "wb_fpga_pca954x_device", "delay": 0},
    {"name": "wb_logic_mdio", "delay": 0},
    {"name": "wb_logic_mdio_device", "delay": 0},
    {"name": "wb_i2c_dev", "delay": 0},
    {"name": "wb_i2c_dev_device", "delay": 0},

    {"name": "wb_optoe", "delay": 0},
    {"name": "wb_at24", "delay": 0},

    {"name": "firmware_driver_cpld", "delay": 0},
    {"name": "firmware_driver_ispvme", "delay": 0},
    {"name": "firmware_driver_sysfs", "delay": 0},
    {"name": "wb_firmware_upgrade_device", "delay": 0},
    {"name": "hw_test", "delay": 0},

    {"name": "s3ip_sysfs", "delay": 0},
    {"name": "wb_switch_driver", "delay": 0},
    {"name": "transceiver_device_driver", "delay": 0},
    {"name": "system_device_driver", "delay": 0},
]

DEVICE = [
    {"name": "wb_24c02", "bus": 0, "loc": 0x56},
]

OPTOE = [
    {"name": "wb_optoe3", "startbus": 44, "endbus": 75},
]

INIT_PARAM = []

INIT_COMMAND = [
    # enable root port PCIe AER
    "setpci -s 00:10.0 0x5c.b=0x1f",
    "setpci -s 00:12.0 0x5c.b=0x1f",
    "setpci -s 00:14.0 0x5c.b=0x1f",
    "setpci -s 14:02.0 0x5c.b=0x1f",
    "setpci -s 14:03.0 0x5c.b=0x1f",
    "setpci -s 14:04.0 0x5c.b=0x1f",
    "setpci -s 14:05.0 0x5c.b=0x1f"
]

UPGRADE_SUMMARY = {
    "devtype": 0x4112,
    "e2_devtype": {"gettype":"sysfs", "loc":"/sys/module/platform_common/parameters/dfd_my_type", "int_decode": 10},
    "slot0": {
        "subtype": 0,

        "SPI-LOGIC-DEV": {
            "chain5": {
                "name": "PHY_FPGA",
                "is_support_warm_upg": 0,
            },
            "chain7": {
                "name": "PHY1",
                "is_support_warm_upg": 0,
            },
            "chain8": {
                "name": "PHY2",
                "is_support_warm_upg": 0,
            },
            "chain9": {
                "name": "PHY3",
                "is_support_warm_upg": 0,
            },
            "chain10": {
                "name": "PHY4",
                "is_support_warm_upg": 0,
            },
            "chain11": {
                "name": "PHY5",
                "is_support_warm_upg": 0,
            },
            "chain12": {
                "name": "PHY6",
                "is_support_warm_upg": 0,
            },
            "chain13": {
                "name": "PHY7",
                "is_support_warm_upg": 0,
            },
            "chain14": {
                "name": "PHY8",
                "is_support_warm_upg": 0,
            },
            "chain15": {
                "name": "PHY9",
                "is_support_warm_upg": 0,
            },
            "chain16": {
                "name": "PHY10",
                "is_support_warm_upg": 0,
            },
            "chain17": {
                "name": "PHY11",
                "is_support_warm_upg": 0,
            },
            "chain18": {
                "name": "PHY12",
                "is_support_warm_upg": 0,
            },
            "chain19": {
                "name": "PHY13",
                "is_support_warm_upg": 0,
            },
            "chain20": {
                "name": "PHY14",
                "is_support_warm_upg": 0,
            },
            "chain21": {
                "name": "PHY15",
                "is_support_warm_upg": 0,
            },
            "chain22": {
                "name": "PHY16",
                "is_support_warm_upg": 0,
            },
        },

        "TEST": {
            "fpga": [
                {"chain": 5, "file": "/etc/.upgrade_test/fpga_test_header.bin", "display_name": "PHY_FPGA"},
            ],
        },
    },
}
