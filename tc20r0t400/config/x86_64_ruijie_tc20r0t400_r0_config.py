#!/usr/bin/python3
# -*- coding: UTF-8 -*-
from platform_common import *

STARTMODULE  =  {
                "avscontrol": 1,
                "dev_monitor": 1,
                "hal_fanctrl":1,
                "hal_ledctrl":1,
                "intelligent_monitor":0,
                "sff_temp_polling":1,
                "pmon_syslog":0,
                "reboot_cause":1,
                "plugins_init":0,
                "product_name": 1,
            }

DRIVERLISTS = [
        {"name": "wb_i2c_i801", "delay": 0},
        {"name": "i2c_dev", "delay": 0},
        {"name": "regmap_i2c", "delay": 0},
        {"name": "wb_i2c_algo_bit", "delay": 0},
        {"name": "wb_i2c_gpio", "delay": 0},
        {"name": "i2c_mux", "delay": 0},
        {"name": "wb_i2c_gpio_device gpio_sda=181 gpio_scl=180 gpio_chip_name=INTC3001:00 bus_num=1", "delay": 0},
        {"name": "platform_common dfd_my_type=0x410c", "delay": 1},
        {"name": "wb_logic_dev_common", "delay": 0},
        {"name": "wb_fpga_pcie", "delay": 0},
        {"name": "wb_pcie_dev", "delay": 0},
        {"name": "wb_pcie_dev_device", "delay": 0},
        {"name": "wb_io_dev", "delay": 0},
        {"name": "wb_io_dev_device", "delay": 0},
        {"name": "wb_i2c_dev", "delay": 0},
        {"name": "wb_fpga_i2c_bus_drv", "delay": 0},
        {"name": "wb_fpga_i2c_bus_device", "delay": 0},
        {"name": "wb_fpga_pca954x_drv", "delay": 0},
        {"name": "wb_fpga_pca954x_device", "delay": 0},
        {"name": "wb_i2c_dev_device", "delay": 0},
        {"name": "mdio_bitbang", "delay": 0},
        {"name": "mdio_gpio", "delay": 0},
        {"name": "wb_mdio_gpio_device gpio_mdc=69 gpio_mdio=70 gpio_chip_name=INTC3001:00", "delay": 0},
        {"name": "wb_wdt", "delay": 0},
        {"name": "wb_wdt_device", "delay": 0},
        {"name": "ipmi_si", "delay": 0},
        # {"name": "intel_spi writeable=on", "delay": 0},
        # {"name": "intel_spi_pci", "delay": 0},
        {"name": "wb_eeprom_93xx46", "delay": 0},
        {"name": "wb_lm75", "delay": 0},
        {"name": "wb_tmp401", "delay": 0},
        {"name": "wb_optoe", "delay": 0},
        {"name": "wb_spd", "delay": 0},
        {"name": "wb_at24", "delay": 0},
        #{"name": "wb_mac_bsc", "delay": 0},
        {"name": "wb_pmbus_core", "delay": 0},
        {"name": "wb_csu550", "delay": 0},
        {"name": "wb_ina3221", "delay": 0},
        {"name": "wb_tps53622", "delay": 0},
        {"name": "wb_ucd9000", "delay": 0},
        {"name": "wb_xdpe12284", "delay": 0},
        {"name": "wb_xdpe132g5c_pmbus", "delay": 0},
        {"name": "wb_xdpe132g5c", "delay": 0},
        #{"name": "wb_ssd_power", "delay": 0},
        #{"name": "wb_ssd_power_device", "delay": 0},
        {"name": "firmware_driver_cpld", "delay": 0},
        {"name": "firmware_driver_ispvme", "delay": 0},
        {"name": "firmware_driver_sysfs", "delay": 0},
        {"name": "wb_firmware_upgrade_device", "delay": 0},
        {"name": "hw_test", "delay": 0},
        {"name": "wb_rc32312", "delay": 0},

        {"name": "s3ip_sysfs", "delay": 0},
        {"name": "wb_switch_driver", "delay": 0},
        {"name": "syseeprom_device_driver", "delay": 0},
        {"name": "fan_device_driver", "delay": 0},
        {"name": "cpld_device_driver", "delay": 0},
        {"name": "sysled_device_driver", "delay": 0},
        {"name": "psu_device_driver", "delay": 0},
        {"name": "transceiver_device_driver", "delay": 0},
        {"name": "temp_sensor_device_driver", "delay": 0},
        #{"name": "vol_sensor_device_driver", "delay": 0},
        #{"name": "curr_sensor_device_driver", "delay": 0},
        {"name": "fpga_device_driver", "delay": 0},
        {"name": "watchdog_device_driver", "delay": 0},
        {"name": "ice", "delay": 0},
]

DEVICE = [
        # E2
        {"name": "wb_24c02", "bus":1, "loc":0x56 },

        #psu
        {"name": "wb_fsp1200", "bus":16, "loc":0x60 },
        {"name": "wb_fsp1200", "bus":17, "loc":0x60 },

        #lm75
        {"name": "wb_lm75", "bus": 18, "loc": 0x4b},
        {"name": "wb_lm75", "bus": 19, "loc": 0x4b},
        {"name": "wb_lm75", "bus": 31, "loc": 0x48},
        {"name": "wb_lm75", "bus": 32, "loc": 0x49},

        #avs control
        {"name": "wb_xdpe132g5c_pmbus", "bus": 38, "loc": 0x60},

        #other
        {"name": "wb_rc32312", "bus": 41, "loc": 0x09},
]

OPTOE = [
        {"name": "wb_optoe3", "startbus": 48, "endbus": 55},
        {"name": "wb_optoe2", "startbus": 56, "endbus": 57},
]

PLATFORM_INTF_OPTOE = {
    "port_num": 10,
    "optoe_start_bus": 48,
}

INIT_COMMAND = [
    # Enable the rc32312
    {"path": "/dev/cpld2", "offset": 0x43, "value": [0xbc], "gettype": "devfile"},
    {"path": "/dev/cpld2", "offset": 0x43, "value": [0xba], "gettype": "devfile"},
]

PRE_COMMAND = [

]

# driver blacklist parameter
BLACKLIST_DRIVERS = [
    "wb_fpga_pcie",
    "wb_i2c_i801",
    "wb_spi_gpio",
    "intel_spi",
    "intel_spi_platform",
    "wb_i2c_ismt",
    "intel_spi_pci",
    "i2c_i801",
    "i2c_ismt",
    "spi_gpio",
    "mei_me",
    "cdc_subset",
   ]

DEV_MONITOR_PARAM = {
    "polling_time": 2,
    "psus": [
        {
            "name": "psu1",
            "device": [
                {"id": "psu1pmbus", "name": "wb_fsp1200", "bus": 16, "loc": 0x60, "attr": "hwmon"},
            ],
         },
        {
            "name": "psu2",
            "device": [
                {"id": "psu2pmbus", "name": "wb_fsp1200", "bus": 17, "loc": 0x60, "attr": "hwmon"},
            ],
         },
    ],
    "others": [
        {
            "name":"eeprom",
            "device":[
                {"id":"eeprom_1", "name":"wb_24c02", "bus":1, "loc":0x56, "attr":"eeprom"},
            ],
        },
        {
            "name":"lm75",
            "device":[
                {"id":"lm75_1", "name":"wb_lm75", "bus":18, "loc":0x4b, "attr":"hwmon"},
                {"id":"lm75_2", "name":"wb_lm75", "bus":19, "loc":0x4b, "attr":"hwmon"},
                {"id":"lm75_3", "name":"wb_lm75", "bus":31, "loc":0x48, "attr":"hwmon"},
                {"id":"lm75_4", "name":"wb_lm75", "bus":32, "loc":0x49, "attr":"hwmon"},
            ],
        },
    ],
}

##################### MAC Voltage adjust####################################
MAC_DEFAULT_PARAM = [
    {
        "name": "mac_core",              # AVS name
        "type": 1,                       # 1: used default value, if rov value not in range. 0: do nothing, if rov value not in range
        "default": 0x73,                 # default value, if rov value not in range
        "rov_source": 0,                 # 0: get rov value from cpld, 1: get rov value from SDK
        "cpld_avs": {"path": "/dev/cpld2", "offset": 0x81, "read_len": 1, "gettype": "devfile"},
        "set_avs": {
            "loc": "/sys/bus/i2c/devices/38-0060/hwmon/hwmon*/avs0_vout", "gettype": "sysfs", "formula": "int((%f)*1000000)"
        },
        "mac_avs_param": {
            0x72: 0.90000,
            0x73: 0.89375,
            0x74: 0.88750,
            0x75: 0.88125,
            0x76: 0.87500,
            0x77: 0.86875,
            0x78: 0.86250,
            0x79: 0.85625,
            0x7a: 0.85000,
            0x7b: 0.84375,
            0x7c: 0.83750,
            0x7d: 0.83125,
            0x7e: 0.82500,
            0x7f: 0.81875,
            0x80: 0.81250,
            0x81: 0.80625,
            0x82: 0.80000,
            0x83: 0.79375,
            0x84: 0.78750,
            0x85: 0.78125,
            0x86: 0.77500,
            0x87: 0.76875,
            0x88: 0.76250,
            0x89: 0.75625,
            0x8a: 0.75000,
            0x8b: 0.74375,
            0x8c: 0.73750,
            0x8d: 0.73125,
            0x8e: 0.72500,
            0x8f: 0.71875,
            0x90: 0.71250,
            0x91: 0.70625,
            0x92: 0.70000,
            0x93: 0.69375,
            0x94: 0.68750,
            0x95: 0.68125,
            0x96: 0.67500,
            0x97: 0.66875,
            0x98: 0.66250,
            0x99: 0.65625,
            0x9a: 0.65000,
        }
    }
]

# 拉起进程的前置操作
INIT_PARAM_PRE = [
    # MAC_CORE_V
    {"loc": "38-0060/hwmon/hwmon*/avs0_vout_min", "value": "650000"},
    {"loc": "38-0060/hwmon/hwmon*/avs0_vout_max", "value": "900000"},
]

INIT_PARAM = [

]

INIT_COMMAND = [
    # monitor bmc tty
    {"path":"/dev/cpld1", "offset":0x58, "value":0x1, "gettype":"devfile"},

    # set sfp power enable
    {"path":"/dev/cpld1", "offset":0x36, "value":0xc9, "gettype":"devfile"},
    {"path":"/dev/cpld1", "offset":0x37, "value":0xc9, "gettype":"devfile"},
    {"path":"/dev/cpld1", "offset":0x38, "value":0xc7, "gettype":"devfile"},

    # mac led reset
    {"path":"/dev/fpga0", "offset":0x17e0, "value":0x01, "gettype":"devfile"},
    {"path":"/dev/fpga0", "offset":0x1328, "value":0x01, "gettype":"devfile"},

    # KR tx-disable enable
    {"path":"/dev/fpga0", "offset":0x1380, "value":0x01, "gettype":"devfile"},
]

REBOOT_CTRL_PARAM = {
    "cpu": {"path":"/dev/cpld1", "offset":0x23, "rst_val":0xdc, "rst_delay":0, "gettype":"devfile"},
    "mac": {"path":"/dev/cpld2", "offset":0x21, "rst_val":0xde, "rst_delay":0, "gettype": "devfile"},
    "phy": {"path":"/dev/cpld1", "offset":0x28, "rst_val":0xd6, "rst_delay":0, "gettype":"devfile"},
    "power": {"path":"/dev/cpld1", "offset":0x4e, "rst_val":0xb0, "rst_delay":0, "gettype":"devfile"},
}

REBOOT_CAUSE_PARA = {
    "reboot_cause_list": [
        {
            "name": "cold_reboot",
            "monitor_point": {"gettype":"devfile", "path":"/dev/cpld1", "offset":0xd8, "read_len":1, "mask":0xf, "okval": 0x1},
            "record": [
                {"record_type":"file", "mode":"cover", "log": "Power Loss, ", "path":"/usr/share/sonic/device/x86_64-baidu-r0/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "Power Loss, ", "path": "/usr/share/sonic/device/x86_64-baidu-r0/.history-reboot-cause.txt"}
            ]
        },
        {
            "name": "watchdog_reboot",
            "monitor_point": {"gettype":"devfile", "path":"/dev/cpld1", "offset":0xd8, "read_len":1, "mask":0xf, "okval": 0x0b},
            "record": [
                {"record_type":"file", "mode":"cover", "log":"Watchdog reboot, ", "path":"/usr/share/sonic/device/x86_64-baidu-r0/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "Watchdog reboot, ", "path": "/usr/share/sonic/device/x86_64-baidu-r0/.history-reboot-cause.txt"}
            ],
        },
        {  # bmc触发i2c命令复位CPU
            "name": "bmc_reboot",
            "monitor_point": {"gettype":"devfile", "path":"/dev/cpld1", "offset":0xd8, "read_len":1, "mask":0xf, "okval":0x04},
            "record": [
                {"record_type":"file", "mode":"cover", "log":"BMC reboot, ", "path":"/usr/share/sonic/device/x86_64-baidu-r0/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "BMC reboot, ", "path": "/usr/share/sonic/device/x86_64-baidu-r0/.history-reboot-cause.txt"}
            ],
        },
        {
            "name": "bmc_powerdown",
            "monitor_point": {"gettype":"devfile", "path":"/dev/cpld1", "offset":0xd8, "read_len":1, "mask":0xf, "okval":0x05},
            "record": [
                {"record_type":"file", "mode":"cover", "log": "BMC powerdown, ", "path":"/usr/share/sonic/device/x86_64-baidu-r0/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "BMC powerdown, ", "path": "/usr/share/sonic/device/x86_64-baidu-r0/.history-reboot-cause.txt"}
            ],
        },
        {
            "name": "otp_switch_reboot",
            "monitor_point": {"gettype": "file_exist", "judge_file": "/etc/.otp_switch_reboot_flag", "okval": True},
            "record": [
                {"record_type": "file", "mode": "cover", "log": "Thermal Overload: ASIC, ",
                    "path": "/usr/share/sonic/device/x86_64-baidu-r0/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "Thermal Overload: ASIC, ",
                    "path": "/usr/share/sonic/device/x86_64-baidu-r0/.history-reboot-cause.txt", "file_max_size":1 * 1024 * 1024}
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
                    "path": "/usr/share/sonic/device/x86_64-baidu-r0/.previous-reboot-cause.txt"},
                {"record_type": "file", "mode": "add", "log": "Thermal Overload: Other, ",
                    "path": "/usr/share/sonic/device/x86_64-baidu-r0/.history-reboot-cause.txt", "file_max_size":1 * 1024 * 1024}
            ],
            "finish_operation": [
                {"gettype": "cmd", "cmd": "rm -rf /etc/.otp_other_reboot_flag"},
            ]
        },
    ],
    "other_reboot_cause_record": [
        {"record_type": "file", "mode": "cover", "log": "Other, ", "path": "/usr/share/sonic/device/x86_64-baidu-r0/.previous-reboot-cause.txt"},
        {"record_type": "file", "mode": "add", "log": "Other, ", "path": "/usr/share/sonic/device/x86_64-baidu-r0/.history-reboot-cause.txt"}
    ],
}

PLATFORM_E2_CONF = {
    "syseeprom": [
        {"name": "syseeprom", "e2_type": "onie_tlv", "e2_path": "/sys/bus/i2c/devices/1-0056/eeprom"},
    ],
}

UPGRADE_SUMMARY = {
    "devtype":0x410c,
    "e2_devtype": {"gettype":"sysfs", "loc":"/sys/module/platform_common/parameters/dfd_my_type", "int_decode": 10},
    "slot0":{
        "subtype":0,
        "VME":{
            "chain1":{
                "name":"BASE_CPLD",
                "is_support_warm_upg":0,
            },
            "chain2":{
                "name":"MAC_CPLD",
                "is_support_warm_upg":0,
            },
            "chain3":{
                "name":"CPU_CPLD",
                "is_support_warm_upg":0,
            },
        },

        "SPI-LOGIC-DEV":{
            "chain1":{
                "name":"FPGA",
                "is_support_warm_upg":0,
            },
        },

        "SYSFS":{
            "chain3":{
                "name":"BCM53134",
                "is_support_warm_upg":0,
                "init_cmd":[
                    {"cmd": "modprobe wb_spi_gpio", "gettype":"cmd"},
                    {"cmd": "modprobe wb_spi_gpio_device sck=55  mosi=54 miso=52 cs=53 bus=0 gpio_chip_name=INTC3001:00", "gettype": "cmd"},
                    {"cmd": "modprobe wb_spi_93xx46", "gettype":"cmd", "delay":0.1},
                ],
                "finish_cmd":[
                    {"cmd": "rmmod wb_spi_93xx46", "gettype":"cmd"},
                    {"cmd": "rmmod wb_spi_gpio_device", "gettype": "cmd"},
                    {"cmd": "rmmod wb_spi_gpio", "gettype":"cmd", "delay":0.1},
                ],
            },
        },

        "TEST":{
            "fpga":[
                {
                    "chain": 1,
                    "file": "/etc/.upgrade_test/0x410c/fpga_test_header.bin",
                    "display_name": "FPGA",
                },
            ],
            "cpld":[
                {"chain":1, "file":"/etc/.upgrade_test/0x410c/base_cpld_test_header.vme", "display_name":"BASE_CPLD"},
                {"chain":2, "file":"/etc/.upgrade_test/0x410c/mac_cpld_test_header.vme", "display_name":"MAC_CPLD"},
                {"chain":3, "file":"/etc/.upgrade_test/0x410c/cpu_cpld_test_header.vme", "display_name":"CPU_CPLD"},
            ],
        },
    },

    "BMC": {
        "name": "BMC",
        "init_cmd":[
            # close BMC wdt
            {"cmd": "ipmitool raw 0x32 0x03 0x02", "gettype": "cmd", "ignore_result": 1},
            # BMC LED timeout time set to max
            {"cmd": "platform_ipmi.py 'dfd_debug sysfs_data_wr /dev/fpga0 0x10e0 0xff'", "gettype": "cmd", "ignore_result": 1},
            # BMC led set to green
            {"cmd": "platform_ipmi.py 'dfd_debug sysfs_data_wr /dev/fpga0 0x1154 0x3'", "gettype": "cmd", "ignore_result": 1},
        ],
        "finish_cmd": [ ],
    },
}
