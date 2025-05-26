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
    {"name": "platform_common dfd_my_type=0x4102", "delay": 0},
    {"name": "wb_io_dev", "delay": 0},
    {"name": "wb_io_dev_device", "delay": 0},
    {"name": "wb_fpga_pcie", "delay": 0},
    {"name": "wb_pcie_dev", "delay": 0},
    {"name": "wb_pcie_dev_device", "delay": 0},
    {"name": "wb_i2c_dev", "delay": 0},
    {"name": "wb_i2c_ocores", "delay": 0},
    {"name": "wb_i2c_ocores_device", "delay": 0},
    {"name": "wb_i2c_mux_pca9641", "delay": 0},
    {"name": "wb_i2c_mux_pca954x", "delay": 0},
    {"name": "wb_i2c_mux_pca954x_device", "delay": 0},
    {"name": "wb_i2c_dev_device", "delay": 0},
    {"name": "wb_optoe", "delay": 0},
    {"name": "wb_at24", "delay": 0},
    {"name": "firmware_driver_ispvme", "delay": 0},
    {"name": "firmware_driver_sysfs", "delay": 0},
    {"name": "wb_firmware_upgrade_device", "delay": 0},

    {"name": "s3ip_sysfs", "delay": 0},
    {"name": "wb_switch_driver", "delay": 0},
    {"name": "syseeprom_device_driver", "delay": 0},
    {"name": "transceiver_device_driver", "delay": 0},
]

DEVICE = [
    {"name": "wb_24c02", "bus": 0, "loc": 0x56},
]

OPTOE = [
    {"name": "wb_optoe3", "startbus": 25, "endbus": 64},
]

INIT_PARAM = []

INIT_COMMAND = [
    # set sysled gree
    "dfd_debug sysfs_data_wr /dev/cpld1 0x50 0x4",
    # mac led reset
    "dfd_debug sysfs_data_wr /dev/fpga0 0x40 0x98 0x00 0x00 0x00",
    "dfd_debug sysfs_data_wr /dev/fpga0 0x44 0x98 0x00 0x00 0x00",
    "dfd_debug sysfs_data_wr /dev/fpga0 0x48 0x98 0x00 0x00 0x00",
    "dfd_debug sysfs_data_wr /dev/fpga0 0x4c 0x98 0x00 0x00 0x00",
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
    "devtype": 0x4102,
    "e2_devtype": {"gettype":"sysfs", "loc":"/sys/module/platform_common/parameters/dfd_my_type", "int_decode": 10},
    "slot0": {
        "subtype": 0,
        "VME": {
            "chain1": {
                "name": "CPU CPLD",
                "is_support_warm_upg": 0,
            },
            "chain2": {
                "name": "BASE CPLD",
                "is_support_warm_upg": 0,
            },
            "chain3": {
                "name": "MAC CPLDA",
                "is_support_warm_upg": 0,
            },
            "chain4": {
                "name": "FCB CPLD",
                "is_support_warm_upg": 0,
            },
            "chain5": {
                "name": "MISC CPLD",
                "is_support_warm_upg": 0,
            },
        },

        "SPI-LOGIC-DEV": {
            "chain1": {
                "name": "FPGA",
                "is_support_warm_upg": 0,
            },
        },

        "MTD": {
            "chain2": {
                "name": "BIOS",
                "is_support_warm_upg": 0,
                "filesizecheck": 20480,  # bios check file size, Unit: K
                "init_cmd": [
                    {"cmd": "modprobe mtd", "gettype": "cmd"},
                    {"cmd": "modprobe spi_nor", "gettype": "cmd"},
                    {"cmd": "modprobe ofpart", "gettype": "cmd"},
                    {"cmd": "modprobe intel_spi writeable=1", "gettype": "cmd"},
                    {"cmd": "modprobe intel_spi_pci", "gettype": "cmd"},
                ],
                "finish_cmd": [
                    {"cmd": "rmmod intel_spi_pci", "gettype": "cmd"},
                    {"cmd": "rmmod intel_spi", "gettype": "cmd"},
                    {"cmd": "rmmod ofpart", "gettype": "cmd"},
                    {"cmd": "rmmod spi_nor", "gettype": "cmd"},
                    {"cmd": "rmmod mtd", "gettype": "cmd"},
                ],
            },
        },

        "TEST": {
            "fpga": [
                {"chain": 1, "file": "/etc/.upgrade_test/fpga_test_header.bin", "display_name": "FPGA"},
            ],
            "cpld": [
                {"chain": 1, "file": "/etc/.upgrade_test/cpu_cpld_test_header.vme", "display_name": "CPU_CPLD"},
                {"chain": 2, "file": "/etc/.upgrade_test/base_cpld_test_header.vme", "display_name": "BASE_CPLD"},
                {"chain": 3, "file": "/etc/.upgrade_test/mac_cplda_test_header.vme", "display_name": "MAC_CPLDA"},
                {"chain": 4, "file": "/etc/.upgrade_test/fcb_cpld_test_header.vme", "display_name": "FCB_CPLD"},
                {"chain": 5, "file": "/etc/.upgrade_test/misc_cpld_test_header.vme", "display_name": "MISC_CPLD"},
            ],
        },
    },
}
