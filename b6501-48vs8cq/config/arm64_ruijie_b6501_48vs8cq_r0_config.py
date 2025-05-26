#!/usr/bin/python
# -*- coding: UTF-8 -*-
from platform_common import *

STARTMODULE = {
    "hal_fanctrl": 1,
    "hal_ledctrl": 1,
    "avscontrol": 1,
    "dev_monitor": 1,
    "pmon_syslog": 1,
    "tty_console": 1,
    "macledreset": 1,
    "sff_temp_polling": 1,
    "generate_airflow": 1,
    "reboot_cause": 1,
    "set_eth_mac": 1,
    "get_mac_temperature": 1,
}

BLACKLIST_DRIVERS = [
    {"name": "spi_phytium_qspi", "delay":0},
    {"name": "spi_nor", "delay":0},
]

DRIVERLISTS = [
        {"name": "phytium_mailbox", "delay": 0},
        {"name": "arm_scpi", "delay": 0},
        {"name": "scpi_cpufreq", "delay": 0},
        {"name": "clk_scpi", "delay": 0},
        {"name": "scpi_hwmon", "delay": 0},
        {"name": "yt_phy", "delay": 0},
        {"name": "wb_stmmac", "delay": 0},
        {"name": "wb_stmmac_platform", "delay": 0},
        {"name": "wb_dwmac_generic", "delay": 0, "removable":0},
        {"name": "yt6801", "delay": 0},
        {"name": "wb-i2c-designware-core", "delay": 0},
        {"name": "wb-i2c-designware-platform", "delay": 0},
        {"name": "spi_nor", "delay":0},
        {"name": "wb_spi_phytium_qspi", "delay":0},
        {"name": "platform_common dfd_my_type_i2c_addr=0x57", "delay":0},
        {"name": "wb_spi_dev", "delay":0, "removable":0},
        {"name": "wb_io_dev", "delay":0},
        {"name": "wb_pcie_dev", "delay":0},
        {"name": "wb_i2c_dev", "delay":0},
        {"name": "wb_cpld_spi_master", "delay":0, "removable":0},
        {"name": "wb_wdt", "delay":0, "removable":0},
        {"name": "dal", "delay":0},
        {"name": "wb_optoe", "delay": 0},
        {"name": "wb_at24", "delay": 0},
        {"name": "wb_indirect_dev", "delay": 0},
        {"name": "wb_i2c_mux_pca9641", "delay": 0},
        {"name": "wb_i2c_mux_pca954x", "delay": 0},
        {"name": "wb_fpga_i2c_bus_drv", "delay": 0},
        {"name": "wb_fpga_pca954x_drv", "delay": 0},
        {"name": "wb_jwh6375", "delay": 0},
        {"name": "wb_ncs23322", "delay": 0},
        {"name": "s3ip_sysfs", "delay": 0},
        {"name": "wb_switch_driver", "delay": 0},
        {"name": "transceiver_device_driver", "delay": 0},
        {"name": "psu_device_driver", "delay": 0},
        {"name": "fan_device_driver", "delay": 0},
        {"name": "cpld_device_driver", "delay": 0},
        {"name": "vol_sensor_device_driver", "delay": 0},
        {"name": "temp_sensor_device_driver", "delay": 0},
        {"name": "sysled_device_driver", "delay": 0},
        {"name": "firmware_driver_ispvme", "delay": 0},
        {"name": "firmware_driver_sysfs", "delay": 0},
]

OPTOE = [
    {"name": "wb_optoe2", "startbus": 2, "endbus": 25},
    {"name": "wb_optoe2", "startbus": 34, "endbus": 57},
    {"name": "wb_optoe1", "startbus": 58, "endbus": 65},
]

DEVICE = [
    #EEPROM
    {"name": "wb_24c02", "bus":0, "loc":0x57 },
    #POWER
    {"name": "wb_csu550", "bus":26, "loc":0x58 },
    {"name": "wb_24c02", "bus":26, "loc":0x50 },
    {"name": "wb_csu550", "bus":27, "loc":0x5B },
    {"name": "wb_24c02", "bus":27, "loc":0x53 },
    #FAN
    {"name": "wb_24c02", "bus":66, "loc":0x53 },
    {"name": "wb_24c02", "bus":67, "loc":0x53 },
    {"name": "wb_24c02", "bus":68, "loc":0x53 },
    {"name": "wb_24c02", "bus":69, "loc":0x53 },
    #TEMP
    {"name": "wb_lm75", "bus":0, "loc":0x48 },
    {"name": "wb_lm75", "bus":0, "loc":0x49 },
    {"name": "wb_lm75", "bus":0, "loc":0x4a },
    {"name": "wb_lm75", "bus":0, "loc":0x4b },
    #AVS
    {"name": "wb_jwh63", "bus":28, "loc":0x68 },
    #CLOCK
    {"name": "wb_ncs23322", "bus":29, "loc":0x69 },
]

DEV_MONITOR_PARAM = {
    "polling_time": 10,
    "psus": [
        {
            "name": "psu1",
            "present": {"gettype": "devfile", "path": "/dev/cpld1", "offset": 0x2041,
                        "read_len": 1, "presentbit": 0, "okval": 0},
            "device": [
                {"id": "psu1pmbus", "name": "wb_csu550", "bus": 26, "loc": 0x58, "attr": "hwmon"},
                {"id": "psu1frue2", "name": "wb_24c02", "bus": 26, "loc": 0x50, "attr": "eeprom"},
            ],
        },
        {
            "name": "psu2",
            "present": {"gettype": "devfile", "path": "/dev/cpld1", "offset": 0x2041,
                        "read_len": 1, "presentbit": 1, "okval": 0},
            "device": [
                {"id": "psu2pmbus", "name": "wb_csu550", "bus": 27, "loc": 0x5b, "attr": "hwmon"},
                {"id": "psu2frue2", "name": "wb_24c02", "bus": 27, "loc": 0x53, "attr": "eeprom"},
            ],
        },
    ],
    "fans": [
        {
            "name": "fan1",
            "present": {"gettype": "devfile", "path": "/dev/cpld1", "offset": 0x4070,
                        "read_len": 1, "presentbit": 0, "okval": 0},
            "device": [
                {"id": "fan1frue2", "name": "wb_24c02", "bus": 66, "loc": 0x53, "attr": "eeprom"},
            ],
        },
        {
            "name": "fan2",
            "present":{"gettype": "devfile", "path": "/dev/cpld1", "offset": 0x4070,
                        "read_len": 1, "presentbit": 1, "okval": 0},
            "device": [
                {"id": "fan2frue2", "name": "wb_24c02", "bus": 67, "loc": 0x53, "attr": "eeprom"},
            ],
        },
        {
            "name": "fan3",
            "present": {"gettype": "devfile", "path": "/dev/cpld1", "offset": 0x4070,
                        "read_len": 1, "presentbit": 2, "okval": 0},
            "device": [
                {"id": "fan3frue2", "name": "wb_24c02", "bus": 68, "loc": 0x53, "attr": "eeprom"},
            ],
        },
        {
            "name": "fan4",
            "present": {"gettype": "devfile", "path": "/dev/cpld1", "offset": 0x4070,
                        "read_len": 1, "presentbit": 3, "okval": 0},
            "device": [
                {"id": "fan4frue2", "name": "wb_24c02", "bus": 69, "loc": 0x53, "attr": "eeprom"},
            ],
        },
    ],
    "others": [
        {
            "name": "eeprom",
            "device": [
                {"id": "eeprom_1", "name": "wb_24c02", "bus":0, "loc":0x57, "attr": "eeprom"},
            ],
        },
        {
            "name": "lm75",
            "device": [
                {"id": "lm75_1", "name": "wb_lm75", "bus": 0, "loc": 0x48, "attr": "hwmon"},
                {"id": "lm75_2", "name": "wb_lm75", "bus": 0, "loc": 0x49, "attr": "hwmon"},
                {"id": "lm75_3", "name": "wb_lm75", "bus": 0, "loc": 0x4a, "attr": "hwmon"},
                {"id": "lm75_4", "name": "wb_lm75", "bus": 0, "loc": 0x4b, "attr": "hwmon"},
            ],
        },
    ],
}

INIT_PARAM_PRE = [
    #{"loc": "7-0064/hwmon/hwmon*/avs0_vout_max", "value": "900000"},
    #{"loc": "7-0064/hwmon/hwmon*/avs0_vout_min", "value": "750000"},
]
INIT_COMMAND_PRE = [
    # Enable the power for QSFP.
    "dfd_debug sysfs_data_wr /dev/cpld1 0x20c0 0xa7",
    "dfd_debug sysfs_data_wr /dev/cpld1 0x20c1 0xa7",
    "dfd_debug sysfs_data_wr /dev/cpld1 0x20c2 0xa7",
    "dfd_debug sysfs_data_wr /dev/cpld1 0x20c3 0xa7",
    "dfd_debug sysfs_data_wr /dev/cpld1 0x20c4 0xa7",
    "dfd_debug sysfs_data_wr /dev/cpld1 0x20c5 0xa7",
    "dfd_debug sysfs_data_wr /dev/cpld1 0x20c6 0xa7",
    "dfd_debug sysfs_data_wr /dev/cpld1 0x20c7 0xa7",
    # Default MAC control code stream indicator.
    "dfd_debug sysfs_data_wr /dev/cpld1 0x4050 0x3",
    # create uboot info.
    "uboot_info.py start",
]

INIT_PARAM = []

INIT_COMMAND = [
    #"i2cset -y -f 8 0x30 0x60 0x00",  # enable txdis[1~8]
    #"i2cset -y -f 8 0x30 0x61 0x00",  # enable txdis[9~16]
    #"i2cset -y -f 8 0x30 0x62 0x00",  # enable txdis[17~24]
    #"i2cset -y -f 8 0x31 0x60 0x00",  # enable txdis[24~32]
    #"i2cset -y -f 8 0x31 0x61 0x00",  # enable txdis[33~40]
    #"i2cset -y -f 8 0x31 0x62 0x00",  # enable txdis[41~48]
]

REBOOT_CAUSE_PARA = {
    "reboot_cause_list": [
        {
            "name": "cold_reboot",
            "monitor_point": {"gettype":"devfile", "path":"/dev/cpld1", "offset":0x20e6, "read_len":1, "mask":0x0f, "okval":0x01},
            "record": [
                {"record_type": "file", "mode": "cover", "log": "Power Loss, ",
                    "path": "/etc/sonic/.reboot/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "Power Loss, ",
                    "path": "/etc/sonic/.reboot/.history-reboot-cause.txt", "file_max_size": 1 * 1024 * 1024}
            ]
        },
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

UPGRADE_SUMMARY = {
    "devtype": 0x40e2,
    "slot0": {
        "subtype": 0,
        "VME": {
            "chain1": {
                "name": "VME_CPLD",
                "is_support_warm_upg": 0,
            },
            "chain2": {
                "name": "VME_CPLD",
                "is_support_warm_upg": 0,
            },
        },
        "MTD": {
            "chain1": {
                "name": "UBOOT",
                "is_support_warm_upg": 0,
            },
        },
        "TEST": {
            "cpld": [
                {"chain": 1, "file": "/etc/.upgrade_test/0x40e2/cpu_cpld_test_header.vme", "display_name": "CPU_CPLD"},
                {"chain": 2, "file": "/etc/.upgrade_test/0x40e2/base_cpld_test_header.vme", "display_name": "BASE_CPLD"},
            ],
        },
    },
}

UBOOT_INFO_CONF = [
    {
        "active": "master",  
        "value": {"gettype": "devfile", "path": "/dev/cpld0", "offset": 0x28, "read_len": 1, "mask": 0xf0, "okval": 0x80},
    },
    {
        "active": "slave", 
        "value": {"gettype": "devfile", "path": "/dev/cpld0", "offset": 0x28, "read_len": 1, "mask": 0xf0, "okval": 0x40},
    },
]

PLATFORM_E2_CONF = {
    "fan": [
        {
            "name": "fan1", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/16-0050/eeprom",
            "e2_decode": [
                {
                    "area": "productInfoArea", "field": "productVersion", "decode_type": "func", "func_name": "fru_decode_hw"
                },
                {
                    "area": "boardInfoArea", "field": "boardextra1", "decode_type": "func", "func_name": "fru_decode_hw"
                },
            ],
        },
        {
            "name": "fan2", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/17-0050/eeprom",
            "e2_decode": [
                {
                    "area": "productInfoArea", "field": "productVersion", "decode_type": "func", "func_name": "fru_decode_hw"
                },
                {
                    "area": "boardInfoArea", "field": "boardextra1", "decode_type": "func", "func_name": "fru_decode_hw"
                },
            ],
        },
        {
            "name": "fan3", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/18-0050/eeprom",
            "e2_decode": [
                {
                    "area": "productInfoArea", "field": "productVersion", "decode_type": "func", "func_name": "fru_decode_hw"
                },
                {
                    "area": "boardInfoArea", "field": "boardextra1", "decode_type": "func", "func_name": "fru_decode_hw"
                },
            ],
        },
        {
            "name": "fan4", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/19-0050/eeprom",
            "e2_decode": [
                {
                    "area": "productInfoArea", "field": "productVersion", "decode_type": "func", "func_name": "fru_decode_hw"
                },
                {
                    "area": "boardInfoArea", "field": "boardextra1", "decode_type": "func", "func_name": "fru_decode_hw"
                },
            ],
        },
    ],
    "psu": [
        {"name": "psu1", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/24-0050/eeprom"},
        {"name": "psu2", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/25-0050/eeprom"},
    ],
    "syseeprom": [
        {"name": "syseeprom", "e2_type": "onie_tlv", "e2_path": "/sys/bus/i2c/devices/0-0056/eeprom"},
    ],
}

SET_MAC_CONF = [
    {
        "eth_name": "eth0",
        "e2_name": "syseeprom",
        "e2_type": "onie_tlv",
        "e2_path": "/sys/bus/i2c/devices/0-0057/eeprom",
        "mac_location": {"field": "Base MAC Address"},
        "ifcfg": {
            "ifcfg_file_path": "/etc/network/interfaces.d/ifcfg-eth0-mac", "file_mode": "add",
        }
    },
    {
        "eth_name": "eth1",
        "e2_name": "syseeprom",
        "e2_type": "onie_tlv",
        "e2_path": "/sys/bus/i2c/devices/0-0057/eeprom",
        "mac_location": {"field": "Base MAC Address"},
        "increment": 1,
        "ifcfg": {
            "ifcfg_file_path": "/etc/network/interfaces.d/ifcfg-eth1-mac", "file_mode": "add",
        }
    }
]

AIR_FLOW_CONF = {
    "psu_fan_airflow": {
        "intake": ['CSU550AP-3-500', 'DPS-550AB-39 A', 'GW-CRPS550N2C', 'CSU550AP-3-300', 'DPS-550AB-39 B', 'CSU550AP-3', 'U1D-D10800-DRB'],
        "exhaust": ['CSU550AP-3-501', 'DPS-550AB-40 A', 'GW-CRPS550N2RC']
    },

    "fanairflow": {
        "intake": ['M1HFAN III-F'],
        "exhaust": ['M1HFAN III-R']
    },

    "fans": [
        {
            "name": "FAN1", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/16-0050/eeprom",
            "area": "productInfoArea", "field": "productName", "decode": "fanairflow"
        },
        {
            "name": "FAN2", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/17-0050/eeprom",
            "area": "productInfoArea", "field": "productName", "decode": "fanairflow"
        },
        {
            "name": "FAN3", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/18-0050/eeprom",
            "area": "productInfoArea", "field": "productName", "decode": "fanairflow"
        },
        {
            "name": "FAN4", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/19-0050/eeprom",
            "area": "productInfoArea", "field": "productName", "decode": "fanairflow"
        }
    ],

    "psus": [
        {
            "name": "PSU1", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/24-0050/eeprom",
            "area": "productInfoArea", "field": "productPartModelName", "decode": "psu_fan_airflow"
        },
        {
            "name": "PSU2", "e2_type": "fru", "e2_path": "/sys/bus/i2c/devices/25-0050/eeprom",
            "area": "productInfoArea", "field": "productPartModelName", "decode": "psu_fan_airflow"
        }
    ]
}
