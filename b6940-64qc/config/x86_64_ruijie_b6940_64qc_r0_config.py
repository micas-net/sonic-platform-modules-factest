#!/usr/bin/python
# -*- coding: UTF-8 -*-
from platform_common import *

STARTMODULE = {
    "hal_fanctrl": 0,
    "hal_ledctrl":0,
    "avscontrol": 0,
    "dev_monitor": 0,
    "reboot_cause": 0,
    "pmon_syslog": 0,
    "sff_temp_polling": 0,
}

MANUINFO_CONF = {
    "bios": {
        "key": "BIOS",
        "head": True,
        "next": "onie"
    },
    "bios_vendor": {
        "parent": "bios",
        "key": "Vendor",
        "cmd": "dmidecode -t 0 |grep Vendor",
        "pattern": r".*Vendor",
        "separator": ":",
        "arrt_index": 1,
    },
    "bios_version": {
        "parent": "bios",
        "key": "Version",
        "cmd": "dmidecode -t 0 |grep Version",
        "pattern": r".*Version",
        "separator": ":",
        "arrt_index": 2,
    },
    "bios_date": {
        "parent": "bios",
        "key": "Release Date",
        "cmd": "dmidecode -t 0 |grep Release",
        "pattern": r".*Release Date",
        "separator": ":",
        "arrt_index": 3,
    },
    "onie": {
        "key": "ONIE",
        "next": "cpu"
    },
    "onie_date": {
        "parent": "onie",
        "key": "Build Date",
        "file": "/host/machine.conf",
        "pattern": r"^onie_build_date",
        "separator": "=",
        "arrt_index": 1,
    },
    "onie_version": {
        "parent": "onie",
        "key": "Version",
        "file": "/host/machine.conf",
        "pattern": r"^onie_version",
        "separator": "=",
        "arrt_index": 2,
    },

    "cpu": {
        "key": "CPU",
        "next": "ssd"
    },
    "cpu_vendor": {
        "parent": "cpu",
        "key": "Vendor",
        "cmd": "dmidecode --type processor |grep Manufacturer",
        "pattern": r".*Manufacturer",
        "separator": ":",
        "arrt_index": 1,
    },
    "cpu_model": {
        "parent": "cpu",
        "key": "Device Model",
        "cmd": "dmidecode --type processor | grep Version",
        "pattern": r".*Version",
        "separator": ":",
        "arrt_index": 2,
    },
    "cpu_core": {
        "parent": "cpu",
        "key": "Core Count",
        "cmd": "dmidecode --type processor | grep \"Core Count\"",
        "pattern": r".*Core Count",
        "separator": ":",
        "arrt_index": 3,
    },
    "cpu_thread": {
        "parent": "cpu",
        "key": "Thread Count",
        "cmd": "dmidecode --type processor | grep \"Thread Count\"",
        "pattern": r".*Thread Count",
        "separator": ":",
        "arrt_index": 4,
    },
    "ssd": {
        "key": "SSD",
        "next": "cpld"
    },
    "ssd_model": {
        "parent": "ssd",
        "key": "Device Model",
        "cmd": "smartctl -i /dev/sda |grep \"Device Model\"",
        "pattern": r".*Device Model",
        "separator": ":",
        "arrt_index": 1,
    },
    "ssd_fw": {
        "parent": "ssd",
        "key": "Firmware Version",
        "cmd": "smartctl -i /dev/sda |grep \"Firmware Version\"",
        "pattern": r".*Firmware Version",
        "separator": ":",
        "arrt_index": 2,
    },
    "ssd_user_cap": {
        "parent": "ssd",
        "key": "User Capacity",
        "cmd": "smartctl -i /dev/sda |grep \"User Capacity\"",
        "pattern": r".*User Capacity",
        "separator": ":",
        "arrt_index": 3,
    },

    "cpld": {
        "key": "CPLD",
        "next": "fpga"
    },

    "cpld1": {
        "key": "CPLD1",
        "parent": "cpld",
        "arrt_index": 1,
    },
    "cpld1_model": {
        "key": "Device Model",
        "parent": "cpld1",
        "config": "LCMXO3LF-2100C-5BG256C",
        "arrt_index": 1,
    },
    "cpld1_vender": {
        "key": "Vendor",
        "parent": "cpld1",
        "config": "LATTICE",
        "arrt_index": 2,
    },
    "cpld1_desc": {
        "key": "Description",
        "parent": "cpld1",
        "config": "CPU CPLD",
        "arrt_index": 3,
    },
    "cpld1_version": {
        "key": "Firmware Version",
        "parent": "cpld1",
        "reg": {
            "loc": "/dev/port",
            "offset": 0xa00,
            "size": 4
        },
        "callback": "cpld_format",
        "arrt_index": 4,
    },

    "cpld2": {
        "key": "CPLD2",
        "parent": "cpld",
        "arrt_index": 2,
    },
    "cpld2_model": {
        "key": "Device Model",
        "parent": "cpld2",
        "config": "LCMXO3LF-4300C-6BG324I",
        "arrt_index": 1,
    },
    "cpld2_vender": {
        "key": "Vendor",
        "parent": "cpld2",
        "config": "LATTICE",
        "arrt_index": 2,
    },
    "cpld2_desc": {
        "key": "Description",
        "parent": "cpld2",
        "config": "BASE CPLD",
        "arrt_index": 3,
    },
    "cpld2_version": {
        "key": "Firmware Version",
        "parent": "cpld2",
        "reg": {
            "loc": "/dev/port",
            "offset": 0x900,
            "size": 4
        },
        "callback": "cpld_format",
        "arrt_index": 4,
    },

    "cpld3": {
        "key": "CPLD3",
        "parent": "cpld",
        "arrt_index": 3,
    },
    "cpld3_model": {
        "key": "Device Model",
        "parent": "cpld3",
        "config": "LCMXO3LF-4300C-6BG324I",
        "arrt_index": 1,
    },
    "cpld3_vender": {
        "key": "Vendor",
        "parent": "cpld3",
        "config": "LATTICE",
        "arrt_index": 2,
    },
    "cpld3_desc": {
        "key": "Description",
        "parent": "cpld3",
        "config": "MAC CPLDA",
        "arrt_index": 3,
    },
    "cpld3_version": {
        "key": "Firmware Version",
        "parent": "cpld3",
        "i2c": {
            "bus": "18",
            "loc": "0x30",
            "offset": 0,
            "size": 4
        },
        "callback": "cpld_format",
        "arrt_index": 4,
    },

    "cpld4": {
        "key": "CPLD5",
        "parent": "cpld",
        "arrt_index": 4,
    },
    "cpld4_model": {
        "key": "Device Model",
        "parent": "cpld4",
        "config": "LCMXO3LF-4300C-6BG324I",
        "arrt_index": 1,
    },
    "cpld4_vender": {
        "key": "Vendor",
        "parent": "cpld4",
        "config": "LATTICE",
        "arrt_index": 2,
    },
    "cpld4_desc": {
        "key": "Description",
        "parent": "cpld4",
        "config": "MAC CPLDB",
        "arrt_index": 3,
    },
    "cpld4_version": {
        "key": "Firmware Version",
        "parent": "cpld4",
        "i2c": {
            "bus": "19",
            "loc": "0x30",
            "offset": 0,
            "size": 4
        },
        "callback": "cpld_format",
        "arrt_index": 4,
    },

    "i210": {
        "key": "NIC",
        "next": "fpga"
    },
    "i210_model": {
        "parent": "i210",
        "config": "NA",
        "key": "Device Model",
        "arrt_index": 1,
    },
    "i210_vendor": {
        "parent": "i210",
        "config": "INTEL",
        "key": "Vendor",
        "arrt_index": 2,
    },
    "i210_version": {
        "parent": "i210",
        "cmd": "ethtool -i eth0",
        "pattern": r"firmware-version",
        "separator": ":",
        "key": "Firmware Version",
        "arrt_index": 3,
    },

    "fpga": {
        "key": "FPGA",
    },
    "fpga_model": {
        "parent": "fpga",
        "config": "XC7A50T-2FGG484I",
        "key": "Device Model",
        "arrt_index": 1,
    },
    "fpga_vendor": {
        "parent": "fpga",
        "config": "XILINX",
        "key": "Vendor",
        "arrt_index": 2,
    },
    "fpga_desc": {
        "parent": "fpga",
        "config": "NA",
        "key": "Description",
        "arrt_index": 3,
    },
    "fpga_hw_version": {
        "parent": "fpga",
        "config": "NA",
        "key": "Hardware Version",
        "arrt_index": 4,
    },
    "fpga_fw_version": {
        "parent": "fpga",
        "pci": {
            "bus": 3,
            "slot": 0,
            "fn": 0,
            "bar": 0,
            "offset": 0
        },
        "key": "Firmware Version",
        "arrt_index": 5,
    },
    "fpga_date": {
        "parent": "fpga",
        "pci": {
            "bus": 3,
            "slot": 0,
            "fn": 0,
            "bar": 0,
            "offset": 4
        },
        "key": "Build Date",
        "arrt_index": 6,
    },
}

DRIVERLISTS = [
    {"name": "r8168", "delay": 0, "removable": 0},
    # {"name": "ice", "delay": 0, "removable": 0},
    {"name": "wb_i2c_i801", "delay": 0},
    {"name": "i2c_dev", "delay": 0},
    {"name": "i2c_mux", "delay": 0},
    {"name": "platform_common dfd_my_type=0x40e8", "delay": 0},
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
]

DEVICE = [
    {"name": "wb_24c02", "bus": 0, "loc": 0x56},
]

OPTOE = [
    {"name": "wb_optoe3", "startbus": 25, "endbus": 88},
]

REBOOT_CTRL_PARAM = {}

DEV_MONITOR_PARAM = {}

INIT_PARAM_PRE = []

INIT_COMMAND_PRE = []

INIT_PARAM = []

INIT_COMMAND = [
    # set sysled
    "dfd_debug io_wr 0x950 0x04",
    # mac led reset
    "dfd_debug sysfs_data_wr /dev/fpga0 0x40 0x98 0x00 0x00 0x00",
    "dfd_debug sysfs_data_wr /dev/fpga0 0x44 0x98 0x00 0x00 0x00",
    "dfd_debug sysfs_data_wr /dev/fpga0 0x48 0x98 0x00 0x00 0x00",
    "dfd_debug sysfs_data_wr /dev/fpga0 0x4c 0x98 0x00 0x00 0x00",
    # enable root port PCIe AER
    # "setpci -s 00:10.0 0x5c.b=0x1f",
    # "setpci -s 00:12.0 0x5c.b=0x1f",
    # "setpci -s 00:14.0 0x5c.b=0x1f",
    # "setpci -s 14:02.0 0x5c.b=0x1f",
    # "setpci -s 14:03.0 0x5c.b=0x1f",
    # "setpci -s 14:04.0 0x5c.b=0x1f",
    # "setpci -s 14:05.0 0x5c.b=0x1f"
]

REBOOT_CAUSE_PARA = {}

UPGRADE_SUMMARY = {
    "devtype": 0x40e8,

    "slot0": {
        "subtype": 0,
        "VME": {
            "chain1": {
                "name": "CPU_CPLD",
                "is_support_warm_upg": 0,
            },
            "chain2": {
                "name": "BASE_CPLD",
                "is_support_warm_upg": 0,
            },
            "chain3": {
                "name": "MAC_CPLDA",
                "is_support_warm_upg": 0,
            },
            "chain4": {
                "name": "MAC_CPLDB",
                "is_support_warm_upg": 0,
            },
            "chain5": {
                "name": "FCB_CPLD",
                "is_support_warm_upg": 0,
            },
            "chain6": {
                "name": "MISC_CPLD",
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
                {"chain": 4, "file": "/etc/.upgrade_test/mac_cpldb_test_header.vme", "display_name": "MAC_CPLDB"},
                {"chain": 5, "file": "/etc/.upgrade_test/fcb_cpld_test_header.vme", "display_name": "FCB_CPLD"},
                {"chain": 6, "file": "/etc/.upgrade_test/misc_cpld_test_header.vme", "display_name": "MISC_CPLD"},
            ],
        },
    },
}
