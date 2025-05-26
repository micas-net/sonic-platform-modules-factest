#!/usr/bin/python
# -*- coding: UTF-8 -*-
from platform_common import *

STARTMODULE = {
    "hal_fanctrl": 1,
    "hal_ledctrl": 1,
    "avscontrol": 1,
    "dev_monitor": 1,
    "tty_console": 0,
    "reboot_cause": 1,
    "pmon_syslog": 1,
    "sff_temp_polling": 1,
    "generate_airflow": 0,
    "product_name": 1,
    "set_eth_mac": 1,
    "generate_mgmt_version": 0,
}

DEV_MONITOR_PARAM = {
    "polling_time": 10,
    "psus": [
        {
            "name": "psu1",
            "present": {"gettype": "io", "io_addr": 0x942, "presentbit": 0, "okval": 0},
            "device": [
                {"id": "psu1pmbus", "name": "wb_fsp1200", "bus": 240, "loc": 0x58, "attr": "hwmon"},
                {"id": "psu1frue2", "name": "wb_24c02", "bus": 240, "loc": 0x50, "attr": "eeprom"},
            ],
        },
        {
            "name": "psu2",
            "present": {"gettype": "io", "io_addr": 0x942, "presentbit": 1, "okval": 0},
            "device": [
                {"id": "psu2pmbus", "name": "wb_fsp1200", "bus": 241, "loc": 0x58, "attr": "hwmon"},
                {"id": "psu2frue2", "name": "wb_24c02", "bus": 241, "loc": 0x50, "attr": "eeprom"},
            ],
        },
        {
            "name": "psu3",
            "present": {"gettype": "io", "io_addr": 0x942, "presentbit": 2, "okval": 0},
            "device": [
                {"id": "psu3pmbus", "name": "wb_fsp1200", "bus": 242, "loc": 0x58, "attr": "hwmon"},
                {"id": "psu3frue2", "name": "wb_24c02", "bus": 242, "loc": 0x50, "attr": "eeprom"},
            ],
        },
        {
            "name": "psu4",
            "present": {"gettype": "io", "io_addr": 0x942, "presentbit": 3, "okval": 0},
            "device": [
                {"id": "psu4pmbus", "name": "wb_fsp1200", "bus": 243, "loc": 0x58, "attr": "hwmon"},
                {"id": "psu4frue2", "name": "wb_24c02", "bus": 243, "loc": 0x50, "attr": "eeprom"},
            ],
        },
    ],
    "fans": [
        {
            "name": "fan1",
            "present": {"gettype": "i2c", "bus": 178, "loc": 0x0d, "offset": 0x27, "presentbit": 0, "okval": 0},
            "device": [
                {"id": "fan1frue2", "name": "wb_24c64", "bus": 260, "loc": 0x50, "attr": "eeprom"},
            ],
        },
        {
            "name": "fan2",
            "present": {"gettype": "i2c", "bus": 178, "loc": 0x0d, "offset": 0x37, "presentbit": 0, "okval": 0},
            "device": [
                {"id": "fan2frue2", "name": "wb_24c64", "bus": 259, "loc": 0x50, "attr": "eeprom"},
            ],
        },
        {
            "name": "fan3",
            "present": {"gettype": "i2c", "bus": 178, "loc": 0x0d, "offset": 0x47, "presentbit": 0, "okval": 0},
            "device": [
                {"id": "fan3frue2", "name": "wb_24c64", "bus": 258, "loc": 0x50, "attr": "eeprom"},
            ],
        },
        {
            "name": "fan4",
            "present": {"gettype": "i2c", "bus": 178, "loc": 0x0d, "offset": 0x57, "presentbit": 0, "okval": 0},
            "device": [
                {"id": "fan4frue2", "name": "wb_24c64", "bus":257, "loc": 0x50, "attr": "eeprom"},
            ],
        },
        {
            "name": "fan5",
            "present": {"gettype": "i2c", "bus": 178, "loc": 0x0d, "offset": 0x67, "presentbit": 0, "okval": 0},
            "device": [
                {"id": "fan5frue2", "name": "wb_24c64", "bus": 256, "loc": 0x50, "attr": "eeprom"},
            ],
        },
    ],
    "others": [
        {
            "name": "eeprom",
            "device": [
                {"id": "eeprom_1", "name": "wb_24c02", "bus": 1, "loc": 0x56, "attr": "eeprom"},
                {"id": "eeprom_2", "name": "wb_24c02", "bus": 1, "loc": 0x57, "attr": "eeprom"},
                {"id": "eeprom_3", "name": "wb_24c02", "bus": 167, "loc": 0x50, "attr": "eeprom"},
                {"id": "eeprom_4", "name": "wb_24c64", "bus": 168, "loc": 0x51, "attr": "eeprom"},
                {"id": "eeprom_5", "name": "wb_24c02", "bus": 172, "loc": 0x57, "attr": "eeprom"},
                {"id": "eeprom_6", "name": "wb_24c02", "bus": 192, "loc": 0x51, "attr": "eeprom"},
                {"id": "eeprom_7", "name": "wb_24c02", "bus": 262, "loc": 0x56, "attr": "eeprom"},
            ],
        },
        {
            "name": "tmp275",
            "device": [
                {"id": "tmp275_1", "name": "wb_tmp275", "bus": 244, "loc": 0x48, "attr": "hwmon"},
                {"id": "tmp275_2", "name": "wb_tmp275", "bus": 254, "loc": 0x48, "attr": "hwmon"},
                {"id": "tmp275_3", "name": "wb_tmp275", "bus": 263, "loc": 0x49, "attr": "hwmon"},
                {"id": "tmp275_4", "name": "wb_tmp275", "bus": 263, "loc": 0x48, "attr": "hwmon"},
            ],
        },
        {
            "name": "ina3221",
            "device": [
                {"id": "ina3221_1", "name": "wb_ina3221", "bus": 185, "loc": 0x43, "attr": "hwmon"},
                {"id": "ina3221_2", "name": "wb_ina3221", "bus": 200, "loc": 0x41, "attr": "hwmon"},
                {"id": "ina3221_3", "name": "wb_ina3221", "bus": 201, "loc": 0x41, "attr": "hwmon"},
                {"id": "ina3221_4", "name": "wb_ina3221", "bus": 202, "loc": 0x41, "attr": "hwmon"},
                {"id": "ina3221_5", "name": "wb_ina3221", "bus": 203, "loc": 0x41, "attr": "hwmon"},
                {"id": "ina3221_6", "name": "wb_ina3221", "bus": 224, "loc": 0x40, "attr": "hwmon"},
                {"id": "ina3221_7", "name": "wb_ina3221", "bus": 225, "loc": 0x40, "attr": "hwmon"},
                {"id": "ina3221_8", "name": "wb_ina3221", "bus": 226, "loc": 0x40, "attr": "hwmon"},
                {"id": "ina3221_9", "name": "wb_ina3221", "bus": 227, "loc": 0x40, "attr": "hwmon"},
                {"id": "ina3221_10", "name": "wb_ina3221", "bus": 228, "loc": 0x40, "attr": "hwmon"},
                {"id": "ina3221_11", "name": "wb_ina3221", "bus": 229, "loc": 0x40, "attr": "hwmon"},
                {"id": "ina3221_12", "name": "wb_ina3221", "bus": 230, "loc": 0x40, "attr": "hwmon"},
                {"id": "ina3221_13", "name": "wb_ina3221", "bus": 231, "loc": 0x40, "attr": "hwmon"},
            ],
        },
        {
            "name": "xdpe12284",
            "device": [
                {"id": "xdpe12284_1", "name": "wb_xdpe12284", "bus": 185, "loc": 0x67, "attr": "hwmon"},
                {"id": "xdpe12284_2", "name": "wb_xdpe12284", "bus": 185, "loc": 0x6c, "attr": "hwmon"},
                {"id": "xdpe12284_3", "name": "wb_xdpe12284", "bus": 210, "loc": 0x60, "attr": "hwmon"},
                {"id": "xdpe12284_4", "name": "wb_xdpe12284", "bus": 211, "loc": 0x60, "attr": "hwmon"},
                {"id": "xdpe12284_5", "name": "wb_xdpe12284", "bus": 212, "loc": 0x60, "attr": "hwmon"},
                {"id": "xdpe12284_6", "name": "wb_xdpe12284", "bus": 213, "loc": 0x60, "attr": "hwmon"},
                {"id": "xdpe12284_7", "name": "wb_xdpe12284", "bus": 214, "loc": 0x60, "attr": "hwmon"},
                {"id": "xdpe12284_8", "name": "wb_xdpe12284", "bus": 215, "loc": 0x60, "attr": "hwmon"},
            ],
        },
        {
            "name": "xdpe132g5c",
            "device": [
                {"id": "xdpe132g5c_1", "name": "wb_xdpe132g5c_pmbus", "bus": 208, "loc": 0x60, "attr": "hwmon"},
                {"id": "xdpe132g5c_2", "name": "wb_xdpe132g5c_pmbus", "bus": 209, "loc": 0x60, "attr": "hwmon"},
            ],
        },

        {
            "name": "mac_bsc",
            "device": [
                {"id": "mac_bsc_1", "name": "wb_mac_bsc_td3_x2", "bus": 18, "loc": 0x44, "attr": "hwmon"},
            ],
        },
        {
            "name":"ct7318",
            "device":[
                {"id":"ct7318_1", "name":"ct7318","bus":248, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_2", "name":"ct7318","bus":249, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_3", "name":"ct7318","bus":250, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_4", "name":"ct7318","bus":251, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_5", "name":"ct7318","bus":252, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_6", "name":"ct7318","bus":253, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_7", "name":"ct7318","bus":288, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_8", "name":"ct7318","bus":289, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_9", "name":"ct7318","bus":290, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_10", "name":"ct7318","bus":291, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_11", "name":"ct7318","bus":292, "loc":0x4c, "attr":"hwmon"},
                {"id":"ct7318_12", "name":"ct7318","bus":293, "loc":0x4c, "attr":"hwmon"},
            ],
        },
    ],
}

MANUINFO_CONF = {
    "bios": {
        "key": "BIOS",
        "head": True,
        "next": "cpu"
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
    "bios_boot": {
        "parent": "bios",
        "key": "Boot From",
        "devfile": {
            "loc": "/dev/cpld0",
            "offset":0x27,
            "len":1,
            "bit_width":1
        },
        "decode": {
            "01": "Master",
            "02": "Slave"
        },
        "arrt_index": 4,
    },

    "cpu": {
        "key": "CPU",
        "next": "cpld"
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
            "offset": 0x700,
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
    "cpld2_boot": {
        "parent": "cpld2",
        "key": "Boot From",
        "devfile": {
            "loc": "/dev/cpld1",
            "offset":0x06,
            "len":1,
            "bit_width":1
        },
        "decode": {
            "01": "Main",
            "02": "Golden"
        },
        "arrt_index": 5,
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
        "key": "CPLD4",
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

    "cpld5": {
        "key": "CPLD5",
        "parent": "cpld",
        "arrt_index": 5,
    },
    "cpld5_model": {
        "key": "Device Model",
        "parent": "cpld5",
        "config": "LCMXO3LF-2100C-5BG256C",
        "arrt_index": 1,
    },
    "cpld5_vender": {
        "key": "Vendor",
        "parent": "cpld5",
        "config": "LATTICE",
        "arrt_index": 2,
    },
    "cpld5_desc": {
        "key": "Description",
        "parent": "cpld5",
        "config": "MISC CPLD",
        "arrt_index": 3,
    },
    "cpld5_version": {
        "key": "Firmware Version",
        "parent": "cpld5",
        "i2c": {
            "bus": "232",
            "loc": "0x31",
            "offset": 0,
            "size": 4
        },
        "callback": "cpld_format",
        "arrt_index": 4,
    },

    "cpld6": {
        "key": "CPLD6",
        "parent": "cpld",
        "arrt_index": 6,
    },
    "cpld6_model": {
        "key": "Device Model",
        "parent": "cpld6",
        "config": "LCMXO3LF-2100C-5BG256C",
        "arrt_index": 1,
    },
    "cpld6_vender": {
        "key": "Vendor",
        "parent": "cpld6",
        "config": "LATTICE",
        "arrt_index": 2,
    },
    "cpld6_desc": {
        "key": "Description",
        "parent": "cpld6",
        "config": "FAN CPLD",
        "arrt_index": 3,
    },
    "cpld6_version": {
        "key": "Firmware Version",
        "parent": "cpld6",
        "i2c": {
            "bus": "178",
            "loc": "0x0d",
            "offset": 0,
            "size": 4
        },
        "callback": "cpld_format",
        "arrt_index": 4,
    },

    "psu": {
        "key": "PSU",
        "next": "fan"
    },

    "psu1": {
        "parent": "psu",
        "key": "PSU1",
        "arrt_index": 1,
    },
    "psu1_hw_version": {
        "key": "Hardware Version",
        "parent": "psu1",
        "extra": {
            "funcname": "getPsu",
            "id": "psu1",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "psu1_fw_version": {
        "key": "Firmware Version",
        "parent": "psu1",
        "config": "NA",
        "arrt_index": 2,
    },

    "psu2": {
        "parent": "psu",
        "key": "PSU2",
        "arrt_index": 2,
    },
    "psu2_hw_version": {
        "key": "Hardware Version",
        "parent": "psu2",
        "extra": {
            "funcname": "getPsu",
            "id": "psu2",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "psu2_fw_version": {
        "key": "Firmware Version",
        "parent": "psu2",
        "config": "NA",
        "arrt_index": 2,
    },

    "psu3": {
        "parent": "psu",
        "key": "PSU3",
        "arrt_index": 3,
    },
    "psu3_hw_version": {
        "key": "Hardware Version",
        "parent": "psu3",
        "extra": {
            "funcname": "getPsu",
            "id": "psu3",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "psu3_fw_version": {
        "key": "Firmware Version",
        "parent": "psu3",
        "config": "NA",
        "arrt_index": 2,
    },

    "psu4": {
        "parent": "psu",
        "key": "PSU4",
        "arrt_index": 4,
    },
    "psu4_hw_version": {
        "key": "Hardware Version",
        "parent": "psu4",
        "extra": {
            "funcname": "getPsu",
            "id": "psu4",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "psu4_fw_version": {
        "key": "Firmware Version",
        "parent": "psu4",
        "config": "NA",
        "arrt_index": 2,
    },

    "fan": {
        "key": "FAN",
        "next": "fpga"
    },
    "fan1": {
        "key": "FAN1",
        "parent": "fan",
        "arrt_index": 1,
    },
    "fan1_hw_version": {
        "key": "Hardware Version",
        "parent": "fan1",
        "extra": {
            "funcname": "checkFan",
            "id": "fan1",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "fan1_fw_version": {
        "key": "Firmware Version",
        "parent": "fan1",
        "config": "NA",
        "arrt_index": 2,
    },

    "fan2": {
        "key": "FAN2",
        "parent": "fan",
        "arrt_index": 2,
    },
    "fan2_hw_version": {
        "key": "Hardware Version",
        "parent": "fan2",
        "extra": {
            "funcname": "checkFan",
            "id": "fan2",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "fan2_fw_version": {
        "key": "Firmware Version",
        "parent": "fan2",
        "config": "NA",
        "arrt_index": 2,
    },

    "fan3": {
        "key": "FAN3",
        "parent": "fan",
        "arrt_index": 3,
    },
    "fan3_hw_version": {
        "key": "Hardware Version",
        "parent": "fan3",
        "extra": {
            "funcname": "checkFan",
            "id": "fan3",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "fan3_fw_version": {
        "key": "Firmware Version",
        "parent": "fan3",
        "config": "NA",
        "arrt_index": 2,
    },

    "fan4": {
        "key": "FAN4",
        "parent": "fan",
        "arrt_index": 4,
    },
    "fan4_hw_version": {
        "key": "Hardware Version",
        "parent": "fan4",
        "extra": {
            "funcname": "checkFan",
            "id": "fan4",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "fan4_fw_version": {
        "key": "Firmware Version",
        "parent": "fan4",
        "config": "NA",
        "arrt_index": 2,
    },

    "fan5": {
        "key": "FAN5",
        "parent": "fan",
        "arrt_index": 5,
    },
    "fan5_hw_version": {
        "key": "Hardware Version",
        "parent": "fan5",
        "extra": {
            "funcname": "checkFan",
            "id": "fan5",
            "key": "hw_version"
        },
        "arrt_index": 1,
    },
    "fan5_fw_version": {
        "key": "Firmware Version",
        "parent": "fan5",
        "config": "NA",
        "arrt_index": 2,
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
        "cmd": "ethtool -i eno1",
        "pattern": r"firmware-version",
        "separator": ":",
        "key": "Firmware Version",
        "arrt_index": 3,
    },

    "fpga": {
        "key": "FPGA",
    },

    "fpga1": {
        "key": "FPGA1",
        "parent": "fpga",
        "arrt_index": 1,
    },
    "fpga1_model": {
        "parent": "fpga1",
        "config": "XC7A50T-2FGG484C",
        "key": "Device Model",
        "arrt_index": 1,
    },
    "fpga1_vender": {
        "parent": "fpga1",
        "config": "XILINX",
        "key": "Vendor",
        "arrt_index": 2,
    },
    "fpga1_desc": {
        "key": "Description",
        "parent": "fpga1",
        "config": "IOB FPGA",
        "arrt_index": 3,
    },
    "fpga1_fw_version": {
        "parent": "fpga1",
        "devfile": {
            "loc": "/dev/fpga0",
            "offset": 0,
            "len": 4,
            "bit_width": 4
        },
        "key": "Firmware Version",
        "arrt_index": 4,
    },
    "fpga1_date": {
        "parent": "fpga1",
        "devfile": {
            "loc": "/dev/fpga0",
            "offset": 4,
            "len": 4,
            "bit_width": 4
        },
        "key": "Build Date",
        "arrt_index": 5,
    },
    "fpga1_boot": {
        "parent": "fpga1",
        "key": "Boot From",
        "devfile": {
            "loc": "/dev/fpga0",
            "offset":0x00,
            "len":1,
            "bit_width":1
        },
        "decode": {
            "00": "Golden",
            "default": "Main"
        },
        "arrt_index": 6,
    },
}

PMON_SYSLOG_STATUS = {
    "polling_time": 3,
    "fans": {
        "present": {"path": ["/sys/s3ip/fan/*/status"], "ABSENT": 0},
        "status": [
            {"path": "/sys/s3ip/fan/*/status", 'okval': 1},
            {"path": "/sys/s3ip/fan/*/status", 'okval': 1},
        ],
        "nochangedmsgflag": 1,
        "nochangedmsgtime": 60,
        "noprintfirsttimeflag": 0,
        "alias": {
            "fan1": "FAN1",
            "fan2": "FAN2",
            "fan3": "FAN3",
            "fan4": "FAN4",
            "fan5": "FAN5"
        }
    },
    "psus": {
        "present": {"path": ["/sys/s3ip/psu/*/present"], "ABSENT": 0},
        "status": [
            {"path": "/sys/s3ip/psu/%s/output", "okval": 1},
            {"path": "/sys/s3ip/psu/%s/alert", "okval": 0},
        ],
        "nochangedmsgflag": 1,
        "nochangedmsgtime": 60,
        "noprintfirsttimeflag": 0,
        "alias": {
            "psu1": "PSU1",
            "psu2": "PSU2",
            "psu3": "PSU3",
            "psu4": "PSU4"
        }
    }
}

##################### MAC Voltage adjust####################################
MAC_AVS_PARAM = {
    0x7A: 0.8500,
    0x7C: 0.8375,
    0x7E: 0.8250,
    0x80: 0.8125,
    0x82: 0.8000,
    0x84: 0.7875,
    0x86: 0.7750,
    0x88: 0.7625,
    0x8A: 0.7500,
    0x8C: 0.7375,
    0x8E: 0.7250,
    0x90: 0.7125,
    0x92: 0.7000,
    0x94: 0.6875,
    0x96: 0.6750,
    0x98: 0.6625,
    0x9A: 0.6500,
}
MAC_DEFAULT_PARAM = [
    {
        "name": "mac_die0_core",              # AVS name
        "type": 0,                       # 1: used default value, if rov value not in range. 0: do nothing, if rov value not in range
        "rov_source": 0,                 # 0: get rov value from cpld, 1: get rov value from SDK
        "cpld_avs": {"path": "/dev/cpld16", "offset": 0x72, "read_len": 1, "gettype": "devfile"},
        "set_avs": {
            "loc": "/sys/bus/i2c/devices/208-0060/hwmon/hwmon*/avs0_vout",
            "gettype": "sysfs", "formula": "int((%f)*1000000)"
        },
        "mac_avs_param": MAC_AVS_PARAM,
    },
    {
        "name": "mac_die1_core",              # AVS name
        "type": 0,                       # 1: used default value, if rov value not in range. 0: do nothing, if rov value not in range
        "rov_source": 0,                 # 0: get rov value from cpld, 1: get rov value from SDK
        "cpld_avs": {"path": "/dev/cpld16", "offset": 0x73, "read_len": 1, "gettype": "devfile"},
        "set_avs": {
            "loc": "/sys/bus/i2c/devices/209-0060/hwmon/hwmon*/avs0_vout",
            "gettype": "sysfs", "formula": "int((%f)*1000000)"
        },
        "mac_avs_param": MAC_AVS_PARAM,
    }
]

DRIVERLISTS = [
    {"name": "wb_i2c_i801", "delay": 1},
    {"name": "wb_gpio_d1500", "delay": 0},
    {"name": "i2c_dev", "delay": 0},
    {"name": "wb_i2c_algo_bit", "delay": 0},
    {"name": "wb_i2c_gpio", "delay": 0},
    {"name": "i2c_mux", "delay": 0},
    {"name": "wb_gpio_device", "delay": 0},
    {"name": "wb_i2c_gpio_device gpio_sda=17 gpio_scl=1 gpio_udelay=2", "delay": 0},
    {"name": "platform_common dfd_my_type=0x4100", "delay": 0},
    {"name": "wb_logic_dev_common", "delay":0},
    {"name": "wb_lpc_drv", "delay": 0},
    {"name": "wb_lpc_drv_device", "delay": 0},
    {"name": "wb_io_dev", "delay": 0},
    {"name": "wb_io_dev_device", "delay": 0},
    {"name": "wb_indirect_dev", "delay": 0},
    {"name": "wb_indirect_dev_device", "delay": 0},
    {"name": "wb_fpga_pcie", "delay": 0},
    {"name": "wb_pcie_dev", "delay": 0},
    {"name": "wb_pcie_dev_device", "delay": 0},
    {"name": "wb_i2c_dev", "delay": 0},
    {"name": "wb_i2c_ocores", "delay": 0},
    {"name": "wb_i2c_ocores_device", "delay": 0},
    {"name": "wb_i2c_mux_pca9641", "delay": 0},
    {"name": "wb_i2c_mux_pca954x", "delay": 0},
    {"name": "wb_i2c_mux_pca954x_device", "delay": 0},
    {"name": "wb_fpga_i2c_bus_drv", "delay": 0},
    {"name": "wb_fpga_i2c_bus_device", "delay": 0},
    {"name": "wb_fpga_pca954x_drv", "delay": 0},
    {"name": "wb_fpga_pca954x_device", "delay": 0},
    {"name": "wb_i2c_dev_device", "delay": 0},
    {"name": "mdio_bitbang", "delay": 0},
    {"name": "mdio_gpio", "delay": 0},
    {"name": "wb_mdio_gpio_device", "delay": 0},
    {"name": "hw_test", "delay": 0},
    {"name": "wb_wdt", "delay": 0},
    {"name": "wb_wdt_device", "delay": 0},
    {"name": "wb_lm75", "delay": 0},
    {"name": "wb_at24", "delay": 0},
    {"name": "wb_mac_bsc", "delay": 0},
    {"name": "wb_pmbus_core", "delay": 0},
    {"name": "wb_xdpe12284", "delay": 0},
    {"name": "wb_xdpe132g5c_pmbus", "delay": 0},
    {"name": "wb_csu550", "delay": 0},
    {"name": "wb_ina3221", "delay": 0},
    {"name": "wb_ucd9000", "delay": 0},
    {"name": "wb_xdpe132g5c", "delay": 0},
    {"name": "wb_optoe", "delay": 0},
    {"name": "firmware_driver_cpld", "delay": 0},
    {"name": "firmware_driver_ispvme", "delay": 0},
    {"name": "firmware_driver_sysfs", "delay": 0},
    {"name": "wb_firmware_upgrade_device", "delay": 0},

    {"name": "s3ip_sysfs", "delay": 0},
    {"name": "wb_switch_driver", "delay": 0},
    {"name": "syseeprom_device_driver", "delay": 0},
    {"name": "fan_device_driver", "delay": 0},
    {"name": "cpld_device_driver", "delay": 0},
    {"name": "sysled_device_driver", "delay": 0},
    {"name": "psu_device_driver", "delay": 0},
    {"name": "transceiver_device_driver", "delay": 0},
    {"name": "temp_sensor_device_driver", "delay": 0},
    {"name": "vol_sensor_device_driver", "delay": 0},
    {"name": "curr_sensor_device_driver", "delay": 0},
    {"name": "fpga_device_driver", "delay": 0},
    {"name": "watchdog_device_driver", "delay": 0},
    {"name": "wb_spd", "delay": 0},
    {"name": "ct7148", "delay": 0},
]

DEVICE = [
    {"name": "wb_24c02", "bus": 1, "loc": 0x56},
    {"name": "wb_24c02", "bus": 1, "loc": 0x57},
    {"name": "wb_24c02", "bus": 167, "loc": 0x50},
    {"name": "wb_24c64", "bus": 168, "loc": 0x51},
    {"name": "wb_24c02", "bus": 172, "loc": 0x57},
    {"name": "wb_24c02", "bus": 192, "loc": 0x51},
    {"name": "wb_24c02", "bus": 262, "loc": 0x56},

    {"name": "wb_tmp275", "bus": 244, "loc": 0x48},
    {"name": "wb_tmp275", "bus": 254, "loc": 0x48},
    {"name": "wb_tmp275", "bus": 263, "loc": 0x49},
    {"name": "wb_tmp275", "bus": 263, "loc": 0x48},

    {"name": "wb_ina3221", "bus": 185, "loc": 0x43},
    {"name": "wb_ina3221", "bus": 200, "loc": 0x41},
    {"name": "wb_ina3221", "bus": 201, "loc": 0x41},
    {"name": "wb_ina3221", "bus": 202, "loc": 0x41},
    {"name": "wb_ina3221", "bus": 203, "loc": 0x41},
    {"name": "wb_ina3221", "bus": 224, "loc": 0x40},
    {"name": "wb_ina3221", "bus": 225, "loc": 0x40},
    {"name": "wb_ina3221", "bus": 226, "loc": 0x40},
    {"name": "wb_ina3221", "bus": 227, "loc": 0x40},
    {"name": "wb_ina3221", "bus": 228, "loc": 0x40},
    {"name": "wb_ina3221", "bus": 229, "loc": 0x40},
    {"name": "wb_ina3221", "bus": 230, "loc": 0x40},
    {"name": "wb_ina3221", "bus": 231, "loc": 0x40},

    {"name": "wb_xdpe12284", "bus": 185, "loc": 0x67},
    {"name": "wb_xdpe12284", "bus": 185, "loc": 0x6c},
    {"name": "wb_xdpe12284", "bus": 210, "loc": 0x60},
    {"name": "wb_xdpe12284", "bus": 211, "loc": 0x60},
    {"name": "wb_xdpe12284", "bus": 212, "loc": 0x60},
    {"name": "wb_xdpe12284", "bus": 213, "loc": 0x60},
    {"name": "wb_xdpe12284", "bus": 214, "loc": 0x60},
    {"name": "wb_xdpe12284", "bus": 215, "loc": 0x60},

    {"name": "wb_24c64", "bus": 260, "loc": 0x50},
    {"name": "wb_24c64", "bus": 259, "loc": 0x50},
    {"name": "wb_24c64", "bus": 258, "loc": 0x50},
    {"name": "wb_24c64", "bus": 257, "loc": 0x50},
    {"name": "wb_24c64", "bus": 256, "loc": 0x50},

    {"name": "wb_24c02", "bus": 240, "loc": 0x50},
    {"name": "wb_fsp1200", "bus": 240, "loc": 0x58},
    {"name": "wb_24c02", "bus": 241, "loc": 0x50},
    {"name": "wb_fsp1200", "bus": 241, "loc": 0x58},
    {"name": "wb_24c02", "bus": 242, "loc": 0x50},
    {"name": "wb_fsp1200", "bus": 242, "loc": 0x58},
    {"name": "wb_24c02", "bus": 243, "loc": 0x50},
    {"name": "wb_fsp1200", "bus": 243, "loc": 0x58},

    {"name": "wb_xdpe132g5c_pmbus", "bus": 208, "loc": 0x60},
    {"name": "wb_xdpe132g5c_pmbus", "bus": 209, "loc": 0x60},

    {"name": "wb_xdpe132g5c", "bus": 208, "loc": 0x62},
    {"name": "wb_xdpe132g5c", "bus": 209, "loc": 0x62},

    {"name": "ct7318", "bus": 248, "loc": 0x4C},
    {"name": "ct7318", "bus": 249, "loc": 0x4C},
    {"name": "ct7318", "bus": 250, "loc": 0x4C},
    {"name": "ct7318", "bus": 251, "loc": 0x4C},
    {"name": "ct7318", "bus": 252, "loc": 0x4C},
    {"name": "ct7318", "bus": 253, "loc": 0x4C},
    {"name": "ct7318", "bus": 288, "loc": 0x4C},
    {"name": "ct7318", "bus": 289, "loc": 0x4C},
    {"name": "ct7318", "bus": 290, "loc": 0x4C},
    {"name": "ct7318", "bus": 291, "loc": 0x4C},
    {"name": "ct7318", "bus": 292, "loc": 0x4C},
    {"name": "ct7318", "bus": 293, "loc": 0x4C},
]

OPTOE = [
    {"name": "wb_optoe3", "startbus": 25, "endbus": 88},
]

REBOOT_CTRL_PARAM = {}

INIT_PARAM_PRE = [
    # set ina3221 shunt_resistor

    # set avs threshold
    # MAC_CORE_V
    {"loc": "208-0060/hwmon/hwmon*/avs0_vout_min", "value": "650000"},
    {"loc": "208-0060/hwmon/hwmon*/avs0_vout_max", "value": "850000"},
    {"loc": "209-0060/hwmon/hwmon*/avs0_vout_min", "value": "650000"},
    {"loc": "209-0060/hwmon/hwmon*/avs0_vout_max", "value": "850000"},
]

INIT_PARAM = []

INIT_COMMAND_PRE = []

INIT_COMMAND = [
    # set sfp power enable
    {"path": "/dev/cpld1", "offset": 0xda, "value": [0xff], "gettype": "devfile"},
    # set sysled
    {"path": "/dev/cpld1", "offset": 0x50, "value": [0x04], "gettype": "devfile"},
    {"path": "/dev/cpld1", "offset": 0xdc, "value": [0x18], "gettype": "devfile"},
    # enable root port PCIe AER
    "setpci -s 00:01.0 0xac.W=7:0x1f",
    "setpci -s 00:01.1 0xac.W=7:0x1f",
    "setpci -s 00:02.0 0xac.W=7:0x1f",
    "setpci -s 00:02.2 0xac.W=7:0x1f",
    "setpci -s 00:02.3 0xac.W=7:0x1f",
    "setpci -s 00:03.0 0xac.W=7:0x1f",
    "setpci -s 00:1c.4 0xac.W=7:0x1f",
    # Power on the panel ports
    {"path": "/dev/cpld16", "offset": 0x5d, "value": [0xa7], "gettype": "devfile"},
    {"path": "/dev/cpld16", "offset": 0x5e, "value": [0xa7], "gettype": "devfile"},
]

UPGRADE_SUMMARY = {
    "devtype": 0x4100,

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
                "name": "FCB_CPLD",
                "is_support_warm_upg": 0,
            },
            "chain6": {
                "name": "MAC_CPLDB",
                "is_support_warm_upg": 0,
            },
            "chain7": {
                "name": "MAC_CPLDA",
                "is_support_warm_upg": 0,
            },
            "chain8": {
                "name": "MISC_CPLD",
                "is_support_warm_upg": 0,
            },
        },

        "SPI-LOGIC-DEV": {
            "chain1": {
                "name": "FPGA",
                "is_support_warm_upg": 0,
                "init_cmd": [
                ],
                "finish_cmd": [
                ],
            },
            "chain2": {
                "name": "WHOLE_FPGA",
                "is_support_warm_upg": 0,
                "init_cmd": [
                ],
                "finish_cmd": [
                ],
            },
        },

        "MTD": {
            "chain3": {
                "name": "BIOS",
                "is_support_warm_upg": 0,
                "init_cmd": [
                    {"cmd": "modprobe mtd", "gettype": "cmd"},
                    {"cmd": "modprobe spi_nor", "gettype": "cmd"},
                    {"cmd": "modprobe ofpart", "gettype": "cmd"},
                    {"cmd": "modprobe intel_spi writeable=1", "gettype": "cmd"},
                    {"cmd": "modprobe intel_spi_platform writeable=1", "gettype": "cmd"},
                ],
                "finish_cmd": [
                    {"cmd": "rmmod intel_spi_platform", "gettype": "cmd"},
                    {"cmd": "rmmod intel_spi", "gettype": "cmd"},
                    {"cmd": "rmmod ofpart", "gettype": "cmd"},
                    {"cmd": "rmmod spi_nor", "gettype": "cmd"},
                    {"cmd": "rmmod mtd", "gettype": "cmd"},
                ],
            },
        },

        "TEST": {
            "fpga": [
                {"chain": 1, "file": "/etc/.upgrade_test/0x4100/fpga_test_header.bin", "display_name": "FPGA"},
            ],
            "cpld": [
                {"chain": 1, "file": "/etc/.upgrade_test/0x4100/cpu_cpld_test_header.vme", "display_name": "CPU_CPLD"},
                {"chain": 2, "file": "/etc/.upgrade_test/0x4100/base_cpld_test_header.vme", "display_name": "BASE_CPLD"},
                {"chain": 3, "file": "/etc/.upgrade_test/0x4100/fcb_cpld_test_header.vme", "display_name": "FCB_CPLD"},
                {"chain": 6, "file": "/etc/.upgrade_test/0x4100/mac_cpldb_test_header.vme", "display_name": "MAC_CPLDB"},
                {"chain": 7, "file": "/etc/.upgrade_test/0x4100/mac_cplda_test_header.vme", "display_name": "MAC_CPLDA"},
                {"chain": 8, "file": "/etc/.upgrade_test/0x4100/misc_cpld_test_header.vme", "display_name": "MISC_CPLD"},
            ],
        },
    },
}

PLATFORM_E2_CONF = {
    "fan": [
        {"name": "fan1", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/260-0050/eeprom"},
        {"name": "fan2", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/259-0050/eeprom"},
        {"name": "fan3", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/258-0050/eeprom"},
        {"name": "fan4", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/257-0050/eeprom"},
        {"name": "fan5", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/256-0050/eeprom"},
    ],
    "psu": [
        {"name": "psu1", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/240-0050/eeprom"},
        {"name": "psu2", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/241-0050/eeprom"},
        {"name": "psu3", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/242-0050/eeprom"},
        {"name": "psu4", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/243-0050/eeprom"},
    ],
    "syseeprom": [
        {"name": "syseeprom", "e2_type": "onie_tlv", "e2_path": "/sys/bus/i2c/devices/1-0056/eeprom"},
    ],
}

PLATFORM_POWER_CONF = [
]

REBOOT_CAUSE_PARA = {
    "reboot_cause_list": [
        {
            "name": "otp_switch_reboot",
            "monitor_point": {"gettype": "file_exist", "judge_file": "/etc/.otp_switch_reboot_flag", "okval": True},
            "record": [
                {"record_type": "file", "mode": "cover", "log": "Thermal Overload: ASIC, ",
                    "path": "/etc/sonic/.reboot/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "Thermal Overload: ASIC, ",
                    "path": "/etc/sonic/.reboot/.history-reboot-cause.txt", "file_max_size": 1 * 1024 * 1024}
            ],
            "finish_operation": [
                {"gettype": "cmd", "cmd": "rm -rf /etc/.otp_switch_reboot_flag"},
            ]
        },
        {
            "name": "otp_other_reboot",
            "monitor_point": {"gettype": "file_exist", "judge_file": "/etc/.otp_other_reboot_flag", "okval": True},
            "record": [
                {"record_type": "file", "mode": "cover", "log": "Thermal Overload: Other, ",
                    "path": "/etc/sonic/.reboot/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "Thermal Overload: Other, ",
                    "path": "/etc/sonic/.reboot/.history-reboot-cause.txt", "file_max_size": 1 * 1024 * 1024}
            ],
            "finish_operation": [
                {"gettype": "cmd", "cmd": "rm -rf /etc/.otp_other_reboot_flag"},
            ]
        },
    ],
    "other_reboot_cause_record": [
        {"record_type": "file", "mode": "cover", "log": "Other, ", "path": "/etc/sonic/.reboot/.previous-reboot-cause.txt"},
        {"record_type": "file", "mode": "add", "log": "Other, ", "path": "/etc/sonic/.reboot/.history-reboot-cause.txt"}
    ],
}

SET_MAC_CONF = [
    {
        "eth_name": "eth0",
        "e2_name": "syseeprom",
        "e2_type": "onie_tlv",
        "e2_path": "/sys/bus/i2c/devices/1-0056/eeprom",
        "mac_location": {"field": "Base MAC Address"},
        "ifcfg": {
            "ifcfg_file_path": "/etc/network/interfaces.d/ifcfg-eth0-mac", "file_mode": "add",
        }
    }
]
