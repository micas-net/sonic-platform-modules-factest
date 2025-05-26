#!/usr/bin/python3

psu_fan_airflow = {
    "intake": [
        'CSU550AP-3-300', 'CSU550AP-3-500', 'DPS-550AB-39 A', 'DPS-550AB-39 B',
        'DPS-1300AB-6 S', 'FSP1200-20ERM', 'CSU800AP-3-300', 'CSU550AP-3', 'CSU800AP-3',
        'GW-CRPS1300D',
    ],
    "exhaust": ['CSU550AP-3-501', 'DPS-550AB-40 A']
}

fanairflow = {
    "intake": ['M6510-FAN-F', 'M2EFAN I-F'],
    "exhaust": [],
}

psu_display_name = {
    "RG-PA550I-F": ['CSU550AP-3', 'CSU550AP-3-300', 'DPS-550AB-39 A', 'DPS-550AB-39 B'],
    "RG-PA800I-F": ['CSU800AP-3', 'CSU800AP-3-300', 'DPS-800AB-48 A', 'DPS-800AB-48 D', 'DPS-850AB-16 A'],
    "RG-PA1200I-F": ['GW-CRPS1300D', 'DPS-1300AB-6 S', 'FSP1200-20ERM'],
}

psutypedecode = {
    0x00: 'N/A',
    0x01: 'AC',
    0x02: 'DC',
}


class Unit:
    Temperature = "C"
    Voltage = "V"
    Current = "A"
    Power = "W"
    Speed = "RPM"


PSU_NOT_PRESENT_PWM = 100


class threshold:
    PSU_TEMP_MIN = -10 * 1000
    PSU_TEMP_MAX = 60 * 1000

    PSU_FAN_SPEED_MIN = 2000
    PSU_FAN_SPEED_MAX = 18000

    PSU_FAN_SPEED_TOLERANCE = 40

    PSU_OUTPUT_VOLTAGE_MIN = 11 * 1000
    PSU_OUTPUT_VOLTAGE_MAX = 14 * 1000

    PSU_AC_INPUT_VOLTAGE_MIN = 200 * 1000
    PSU_AC_INPUT_VOLTAGE_MAX = 240 * 1000

    PSU_DC_INPUT_VOLTAGE_MIN = 190 * 1000
    PSU_DC_INPUT_VOLTAGE_MAX = 290 * 1000

    ERR_VALUE = -9999999

    PSU_OUTPUT_POWER_MIN = 10 * 1000 * 1000
    PSU_OUTPUT_POWER_MAX = 1300 * 1000 * 1000

    PSU_INPUT_POWER_MIN = 10 * 1000 * 1000
    PSU_INPUT_POWER_MAX = 1444 * 1000 * 1000

    PSU_OUTPUT_CURRENT_MIN = 2 * 1000
    PSU_OUTPUT_CURRENT_MAX = 107 * 1000

    PSU_INPUT_CURRENT_MIN = 0.2 * 1000
    PSU_INPUT_CURRENT_MAX = 7 * 1000

    FAN_SPEED_MAX = 23500
    FAN_SPEED_MIN = 5000

    FAN_SPEED_TOLERANCE = 30

class Description:
    CPLD = "Used for managing IO modules, SFP+ modules and system LEDs"
    BIOS = "Performs initialization of hardware components during booting"
    FPGA = "Platform management controller for on-board temperature monitoring, in-chassis power"

devices = {
    # platform_sensor.py get sensor info from s3ip
    "sensor_print_src": "s3ip",

    # platform_sensor.py get temp info from s3ip
    "temp_data_source": [
        {
            "path": "/sys/s3ip/temp_sensor",
            "type": "temp",
            "Unit": Unit.Temperature,
        },
    ],

    # platform_sensor.py get vol info from s3ip
    "dcdc_data_source": [
        {
            "path": "/sys/s3ip/vol_sensor",
            "type": "vol",
            "Unit": Unit.Voltage,
            "read_times": 3,
            "format": "float(float(%s)/1000)"
        },
    ],

    "onie_e2": [
        {
            "name": "ONIE_E2",
            "e2loc": {"loc": "/sys/bus/i2c/devices/2-0057/eeprom", "way": "sysfs"},
            "airflow": "intake"
        },
    ],

    "psus": [
        {
            "e2loc": {"loc": "/sys/bus/i2c/devices/7-0050/eeprom", "way": "sysfs"},
            "pmbusloc": {"bus": 7, "addr": 0x58, "way": "i2c"},
            "present": {"bus": 2, "addr": 0x37, "offset": 0x51, "way": "i2c", "mask": 0x01},
            "name": "PSU1",
            "psu_display_name": psu_display_name,
            "airflow": psu_fan_airflow,
            "psu_not_present_pwm": PSU_NOT_PRESENT_PWM,
            "psu_fan_tolerance": threshold.PSU_FAN_SPEED_TOLERANCE,
            "TempStatus": {"bus": 7, "addr": 0x58, "offset": 0x79, "way": "i2cword", "mask": 0x0004},
            "Temperature": {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/temp1_input", "way": "sysfs"},
                "Min": threshold.PSU_TEMP_MIN,
                "Max": threshold.PSU_TEMP_MAX,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            },
            "FanStatus": {"bus": 7, "addr": 0x58, "offset": 0x79, "way": "i2cword", "mask": 0x0400},
            "FanSpeed": {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/fan1_input", "way": "sysfs"},
                "Min": threshold.PSU_FAN_SPEED_MIN,
                "Max": threshold.PSU_FAN_SPEED_MAX,
                "Unit": Unit.Speed
            },
            "InputsStatus": {"bus": 7, "addr": 0x58, "offset": 0x79, "way": "i2cword", "mask": 0x2000},
            "InputsType": {"bus": 7, "addr": 0x58, "offset": 0x80, "way": "i2c", 'psutypedecode': psutypedecode},
            "InputsVoltage": {
                'AC': {
                    "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/in1_input", "way": "sysfs"},
                    "Min": threshold.PSU_AC_INPUT_VOLTAGE_MIN,
                    "Max": threshold.PSU_AC_INPUT_VOLTAGE_MAX,
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                },
                'DC': {
                    "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/in1_input", "way": "sysfs"},
                    "Min": threshold.PSU_DC_INPUT_VOLTAGE_MIN,
                    "Max": threshold.PSU_DC_INPUT_VOLTAGE_MAX,
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                },
                'other': {
                    "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/in1_input", "way": "sysfs"},
                    "Min": threshold.ERR_VALUE,
                    "Max": threshold.ERR_VALUE,
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                }
            },
            "InputsCurrent":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/curr1_input", "way": "sysfs"},
                "Min": threshold.PSU_INPUT_CURRENT_MIN,
                "Max": threshold.PSU_INPUT_CURRENT_MAX,
                "Unit": Unit.Current,
                "format": "float(float(%s)/1000)"
            },
            "InputsPower":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/power1_input", "way": "sysfs"},
                "Min": threshold.PSU_INPUT_POWER_MIN,
                "Max": threshold.PSU_INPUT_POWER_MAX,
                "Unit": Unit.Power,
                "format": "float(float(%s)/1000000)"
            },
            "OutputsStatus": {"bus": 7, "addr": 0x58, "offset": 0x79, "way": "i2cword", "mask": 0x8800},
            "OutputsVoltage":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/in2_input", "way": "sysfs"},
                "Min": threshold.PSU_OUTPUT_VOLTAGE_MIN,
                "Max": threshold.PSU_OUTPUT_VOLTAGE_MAX,
                "Unit": Unit.Voltage,
                "format": "float(float(%s)/1000)"
            },
            "OutputsCurrent":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/curr2_input", "way": "sysfs"},
                "Min": threshold.PSU_OUTPUT_CURRENT_MIN,
                "Max": threshold.PSU_OUTPUT_CURRENT_MAX,
                "Unit": Unit.Current,
                "format": "float(float(%s)/1000)"
            },
            "OutputsPower":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-7/7-0058/hwmon/hwmon*/power2_input", "way": "sysfs"},
                "Min": threshold.PSU_OUTPUT_POWER_MIN,
                "Max": threshold.PSU_OUTPUT_POWER_MAX,
                "Unit": Unit.Power,
                "format": "float(float(%s)/1000000)"
            },
            "psu_sn": {"loc": "/sys/s3ip/psu/psu1/serial_number", "way": "sysfs"},
            "psu_hw": {"loc": "/sys/s3ip/psu/psu1/hardware_version", "way": "sysfs"},
            "psu_pn": {"loc": "/sys/s3ip/psu/psu1/type", "way": "sysfs"},
            "psu_vendor": {"loc": "/sys/s3ip/psu/psu1/vendor", "way": "sysfs"},
        },
        {
            "e2loc": {"loc": "/sys/bus/i2c/devices/8-0053/eeprom", "way": "sysfs"},
            "pmbusloc": {"bus": 8, "addr": 0x5b, "way": "i2c"},
            "present": {"bus": 2, "addr": 0x37, "offset": 0x51, "way": "i2c", "mask": 0x10},
            "name": "PSU2",
            "psu_display_name": psu_display_name,
            "airflow": psu_fan_airflow,
            "psu_not_present_pwm": PSU_NOT_PRESENT_PWM,
            "psu_fan_tolerance": threshold.PSU_FAN_SPEED_TOLERANCE,
            "TempStatus": {"bus": 8, "addr": 0x5b, "offset": 0x79, "way": "i2cword", "mask": 0x0004},
            "Temperature": {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/temp1_input", "way": "sysfs"},
                "Min": threshold.PSU_TEMP_MIN,
                "Max": threshold.PSU_TEMP_MAX,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            },
            "FanStatus": {"bus": 8, "addr": 0x5b, "offset": 0x79, "way": "i2cword", "mask": 0x0400},
            "FanSpeed": {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/fan1_input", "way": "sysfs"},
                "Min": threshold.PSU_FAN_SPEED_MIN,
                "Max": threshold.PSU_FAN_SPEED_MAX,
                "Unit": Unit.Speed
            },
            "InputsStatus": {"bus": 8, "addr": 0x5b, "offset": 0x79, "way": "i2cword", "mask": 0x2000},
            "InputsType": {"bus": 8, "addr": 0x5b, "offset": 0x80, "way": "i2c", 'psutypedecode': psutypedecode},
            "InputsVoltage": {
                'AC': {
                    "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/in1_input", "way": "sysfs"},
                    "Min": threshold.PSU_AC_INPUT_VOLTAGE_MIN,
                    "Max": threshold.PSU_AC_INPUT_VOLTAGE_MAX,
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                },
                'DC': {
                    "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/in1_input", "way": "sysfs"},
                    "Min": threshold.PSU_DC_INPUT_VOLTAGE_MIN,
                    "Max": threshold.PSU_DC_INPUT_VOLTAGE_MAX,
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                },
                'other': {
                    "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/in1_input", "way": "sysfs"},
                    "Min": threshold.ERR_VALUE,
                    "Max": threshold.ERR_VALUE,
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                }
            },
            "InputsCurrent":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/curr1_input", "way": "sysfs"},
                "Min": threshold.PSU_INPUT_CURRENT_MIN,
                "Max": threshold.PSU_INPUT_CURRENT_MAX,
                "Unit": Unit.Current,
                "format": "float(float(%s)/1000)"
            },
            "InputsPower":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/power1_input", "way": "sysfs"},
                "Min": threshold.PSU_INPUT_POWER_MIN,
                "Max": threshold.PSU_INPUT_POWER_MAX,
                "Unit": Unit.Power,
                "format": "float(float(%s)/1000000)"
            },
            "OutputsStatus": {"bus": 8, "addr": 0x5b, "offset": 0x79, "way": "i2cword", "mask": 0x8800},
            "OutputsVoltage":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/in2_input", "way": "sysfs"},
                "Min": threshold.PSU_OUTPUT_VOLTAGE_MIN,
                "Max": threshold.PSU_OUTPUT_VOLTAGE_MAX,
                "Unit": Unit.Voltage,
                "format": "float(float(%s)/1000)"
            },
            "OutputsCurrent":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/curr2_input", "way": "sysfs"},
                "Min": threshold.PSU_OUTPUT_CURRENT_MIN,
                "Max": threshold.PSU_OUTPUT_CURRENT_MAX,
                "Unit": Unit.Current,
                "format": "float(float(%s)/1000)"
            },
            "OutputsPower":
            {
                "value": {"loc": "/sys/bus/i2c/devices/i2c-8/8-005b/hwmon/hwmon*/power2_input", "way": "sysfs"},
                "Min": threshold.PSU_OUTPUT_POWER_MIN,
                "Max": threshold.PSU_OUTPUT_POWER_MAX,
                "Unit": Unit.Power,
                "format": "float(float(%s)/1000000)"
            },
            "psu_sn": {"loc": "/sys/s3ip/psu/psu2/serial_number", "way": "sysfs"},
            "psu_hw": {"loc": "/sys/s3ip/psu/psu2/hardware_version", "way": "sysfs"},
            "psu_pn": {"loc": "/sys/s3ip/psu/psu2/type", "way": "sysfs"},
            "psu_vendor": {"loc": "/sys/s3ip/psu/psu2/vendor", "way": "sysfs"},
        }
    ],

    "temps": [
        {
            "name": "BOARD_TEMP",
            "temp_id": "TEMP1",
            "api_name": "Board",
            "Temperature": {
                "value": {"loc": "/sys/bus/i2c/devices/2-004a/hwmon/hwmon*/temp1_input", "way": "sysfs"},
                "Min": -10000,
                "Low": 0,
                "High": 70000,
                "Max": 75000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "CPU_TEMP",
            "temp_id": "TEMP2",
            "api_name": "CPU",
            "Temperature": {
                "value": {"loc": "/sys/bus/platform/devices/coretemp.0/hwmon/hwmon*/temp1_input", "way": "sysfs"},
                "Min": 2000,
                "Low": 10000,
                "High": 95000,
                "Max": 103000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "INLET_TEMP",
            "temp_id": "TEMP3",
            "api_name": "Inlet",
            "Temperature": {
                "value": {"loc": "/sys/bus/i2c/devices/2-0048/hwmon/hwmon*/temp1_input", "way": "sysfs"},
                "Min":-10000,
                "Low": 0,
                "High": 50000,
                "Max": 60000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "OUTLET_TEMP",
            "temp_id": "TEMP4",
            "api_name": "Outlet",
            "Temperature": {
                "value": {"loc": "/sys/bus/i2c/devices/2-0049/hwmon/hwmon*/temp1_input", "way": "sysfs"},
                "Min":-10000,
                "Low": 0,
                "High": 50000,
                "Max": 60000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "SWITCH_TEMP",
            "temp_id": "TEMP5",
            "api_name": "Switch ASIC",
            "Temperature": {
                "value": {"loc": "/sys/s3ip/temp_sensor/temp5/value", "way": "sysfs"},
                "Min": 2000,
                "Low": 10000,
                "High": 95000,
                "Max": 103000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "SFF_TEMP",
            "Temperature": {
                "value": {"loc": "/tmp/highest_sff_temp", "way": "sysfs", "flock_path": "/tmp/highest_sff_temp"},
                "Min":-15000,
                "Low": 0,
                "High": 80000,
                "Max": 100000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
    ],
    "leds": [
        {
            "name": "FRONT_SYS_LED",
            "led_type": "SYS_LED",
            "led": {"bus": 2, "addr": 0x33, "offset": 0xb2, "way": "i2c"},
            "led_attrs": {
                "green": 0x01, "red": 0x02, "amber": 0x03, "default": 0x01,
                "flash": 0xff, "light": 0xff, "off": 0, "mask": 0xff
            },
        },
        {
            "name": "BACK_SYS_LED",
            "led_type": "SYS_LED",
            "led": {"bus": 2, "addr": 0x37, "offset": 0xb2, "way": "i2c"},
            "led_attrs": {
                "green": 0x01, "red": 0x02, "amber": 0x03, "default": 0x01,
                "flash": 0xff, "light": 0xff, "off": 0, "mask": 0xff
            },
        },
        {
            "name": "FRONT_BMC_LED",
            "led_type": "BMC_LED",
            "led": {"bus": 2, "addr": 0x33, "offset": 0xb1, "way": "i2c"},
            "led_attrs": {
                "green": 0x04, "red": 0x02, "amber": 0x06, "default": 0x04,
                "flash": 0xff, "light": 0xff, "off": 0, "mask": 0xff
            },
        },
        {
            "name": "BACK_BMC_LED",
            "led_type": "BMC_LED",
            "led": {"bus": 2, "addr": 0x33, "offset": 0xb1, "way": "i2c"},
            "led_attrs": {
                "green": 0x04, "red": 0x02, "amber": 0x06, "default": 0x04,
                "flash": 0xff, "light": 0xff, "off": 0, "mask": 0xff
            },
        },
        {
            "name": "FRONT_PSU_LED",
            "led_type": "PSU_LED",
            "led": {"bus": 2, "addr": 0x33, "offset": 0xb3, "way": "i2c"},
            "led_attrs": {
                "green": 0x04, "red": 0x02, "amber": 0x06, "default": 0x04,
                "flash": 0xff, "light": 0xff, "off": 0, "mask": 0xff
            },
        },
        {
            "name": "FRONT_FAN_LED",
            "led_type": "FAN_LED",
            "led": {"bus": 2, "addr": 0x33, "offset": 0xb4, "way": "i2c"},
            "led_attrs": {
                "green": 0x04, "red": 0x02, "amber": 0x06, "default": 0x04,
                "flash": 0xff, "light": 0xff, "off": 0, "mask": 0xff
            },
        },
    ],
    "fans": [
        {
            "name": "FAN1",
            "StatusTry": 7,
            "airflow": fanairflow,
            "e2loc": {"loc": "/sys/bus/i2c/devices/3-0053/eeprom", "way": "sysfs"},
            "e2_type": "fantlv",
            "present": {"bus": 2, "addr": 0x37, "offset": 0x30, "way": "i2c", "mask": 0x01},
            "SpeedMin": threshold.FAN_SPEED_MIN,
            "SpeedMax": threshold.FAN_SPEED_MAX,
            "led": {"bus": 2, "addr": 0x32, "offset": 0x23, "way": "i2c"},
            "led_attrs": {
                "green": 0x09, "red": 0x0a, "amber": 0x03, "default": 0xf5,
                "flash": 0xff, "light": 0xff, "off": 0xfb
            },
            "Rotor": {
                "Rotor1_config": {
                    "name": "Rotor1",
                    "SpeedMin": threshold.FAN_SPEED_MIN,
                    "SpeedMax": threshold.FAN_SPEED_MAX,
                    "Set_speed": {"bus": 2, "addr": 0x32, "offset": 0x15, "way": "i2c"},
                    "Running": {"bus": 2, "addr": 0x37, "offset": 0x31, "way": "i2c", "mask": 0x01, "is_runing": 0x01},
                    "HwAlarm": {"bus": 2, "addr": 0x37, "offset": 0x31, "way": "i2c", "mask": 0x01, "no_alarm": 0x01},
                    "Speed": {
                        "value": {"loc": "/sys/s3ip/fan/fan1/motor1/speed", "way": "sysfs"},
                        "Min": threshold.FAN_SPEED_MIN,
                        "Max": threshold.FAN_SPEED_MAX,
                        "tolerance": threshold.FAN_SPEED_TOLERANCE,
                        "Unit": Unit.Speed,
                    },
                },
            },
        },
        {
            "name": "FAN2",
            "StatusTry": 7,
            "airflow": fanairflow,
            "e2loc": {"loc": "/sys/bus/i2c/devices/4-0053/eeprom", "way": "sysfs"},
            "e2_type": "fantlv",
            "present": {"bus": 2, "addr": 0x37, "offset": 0x30, "way": "i2c", "mask": 0x02},
            "SpeedMin": threshold.FAN_SPEED_MIN,
            "SpeedMax": threshold.FAN_SPEED_MAX,
            "led": {"bus": 2, "addr": 0x32, "offset": 0x24, "way": "i2c"},
            "led_attrs": {
                "green": 0x09, "red": 0x0a, "amber": 0x03, "default": 0xf5,
                "flash": 0xff, "light": 0xff, "off": 0xfb
            },
            "Rotor": {
                "Rotor1_config": {
                    "name": "Rotor1",
                    "SpeedMin": threshold.FAN_SPEED_MIN,
                    "SpeedMax": threshold.FAN_SPEED_MAX,
                    "Set_speed": {"bus": 2, "addr": 0x32, "offset": 0x15, "way": "i2c"},
                    "Running": {"bus": 2, "addr": 0x37, "offset": 0x31, "way": "i2c", "mask": 0x02, "is_runing": 0x02},
                    "HwAlarm": {"bus": 2, "addr": 0x37, "offset": 0x31, "way": "i2c", "mask": 0x02, "no_alarm": 0x02},
                    "Speed": {
                        "value": {"loc": "/sys/s3ip/fan/fan2/motor1/speed", "way": "sysfs"},
                        "Min": threshold.FAN_SPEED_MIN,
                        "Max": threshold.FAN_SPEED_MAX,
                        "tolerance": threshold.FAN_SPEED_TOLERANCE,
                        "Unit": Unit.Speed,
                    },
                },
            },
        },
        {
            "name": "FAN3",
            "StatusTry": 7,
            "airflow": fanairflow,
            "e2loc": {"loc": "/sys/bus/i2c/devices/5-0053/eeprom", "way": "sysfs"},
            "e2_type": "fantlv",
            "present": {"bus": 2, "addr": 0x37, "offset": 0x30, "way": "i2c", "mask": 0x04},
            "SpeedMin": threshold.FAN_SPEED_MIN,
            "SpeedMax": threshold.FAN_SPEED_MAX,
            "led": {"bus": 2, "addr": 0x32, "offset": 0x25, "way": "i2c"},
            "led_attrs": {
                "green": 0x09, "red": 0x0a, "amber": 0x03, "default": 0xf5,
                "flash": 0xff, "light": 0xff, "off": 0xfb
            },
            "Rotor": {
                "Rotor1_config": {
                    "name": "Rotor1",
                    "SpeedMin": threshold.FAN_SPEED_MIN,
                    "SpeedMax": threshold.FAN_SPEED_MAX,
                    "Set_speed": {"bus": 2, "addr": 0x32, "offset": 0x15, "way": "i2c"},
                    "Running": {"bus": 2, "addr": 0x37, "offset": 0x31, "way": "i2c", "mask": 0x04, "is_runing": 0x04},
                    "HwAlarm": {"bus": 2, "addr": 0x37, "offset": 0x31, "way": "i2c", "mask": 0x04, "no_alarm": 0x04},
                    "Speed": {
                        "value": {"loc": "/sys/s3ip/fan/fan3/motor1/speed", "way": "sysfs"},
                        "Min": threshold.FAN_SPEED_MIN,
                        "Max": threshold.FAN_SPEED_MAX,
                        "tolerance": threshold.FAN_SPEED_TOLERANCE,
                        "Unit": Unit.Speed,
                    },
                },
            },
        },
        {
            "name": "FAN4",
            "StatusTry": 7,
            "airflow": fanairflow,
            "e2loc": {"loc": "/sys/bus/i2c/devices/6-0053/eeprom", "way": "sysfs"},
            "e2_type": "fantlv",
            "present": {"bus": 2, "addr": 0x37, "offset": 0x30, "way": "i2c", "mask": 0x08},
            "SpeedMin": threshold.FAN_SPEED_MIN,
            "SpeedMax": threshold.FAN_SPEED_MAX,
            "led": {"bus": 2, "addr": 0x32, "offset": 0x26, "way": "i2c"},
            "led_attrs": {
                "green": 0x09, "red": 0x0a, "amber": 0x03, "default": 0xf5,
                "flash": 0xff, "light": 0xff, "off": 0xfb
            },
            "Rotor": {
                "Rotor1_config": {
                    "name": "Rotor1",
                    "SpeedMin": threshold.FAN_SPEED_MIN,
                    "SpeedMax": threshold.FAN_SPEED_MAX,
                    "Set_speed": {"bus": 2, "addr": 0x32, "offset": 0x15, "way": "i2c"},
                    "Running": {"bus": 2, "addr": 0x37, "offset": 0x31, "way": "i2c", "mask": 0x08, "is_runing": 0x08},
                    "HwAlarm": {"bus": 2, "addr": 0x37, "offset": 0x31, "way": "i2c", "mask": 0x08, "no_alarm": 0x08},
                    "Speed": {
                        "value": {"loc": "/sys/s3ip/fan/fan4/motor1/speed", "way": "sysfs"},
                        "Min": threshold.FAN_SPEED_MIN,
                        "Max": threshold.FAN_SPEED_MAX,
                        "tolerance": threshold.FAN_SPEED_TOLERANCE,
                        "Unit": Unit.Speed,
                    },
                },
            },
        },
    ],
    "cplds": [
        {
            "name": "MAC_CPLDA",
            "cpld_id": "CPLD1",
            "VersionFile": {"loc": "/dev/cpld4", "offset": 0, "len": 4, "way": "devfile_ascii"},
            "desc": "Used for SFP+ modules",
            "slot": 0,
            "warm": 0,
        },
        {
            "name": "MAC_CPLDB",
            "cpld_id": "CPLD2",
            "VersionFile": {"loc": "/dev/cpld7", "offset": 0, "len": 4, "way": "devfile_ascii"},
            "desc": "Used for SFP+ modules",
            "slot": 0,
            "warm": 0,
        },
        {
            "name": "CONNECT_CPLDA",
            "cpld_id": "CPLD3",
            "VersionFile": {"loc": "/dev/cpld3", "offset": 0, "len": 4, "way": "devfile_ascii"},
            "desc": "Used for system LEDs and FANs",
            "slot": 0,
            "warm": 0,
        },
        {
            "name": "CPU_CPLD",
            "cpld_id": "CPLD4",
            "VersionFile": {"loc": "/dev/cpld1", "offset": 0, "len": 4, "way": "devfile_ascii"},
            "desc": "Used for system power",
            "slot": 0,
            "warm": 0,
        },
    ],
    "cpu": [
        {
            "name": "cpu",
            "CpuResetCntReg": {"bus": 1, "addr": 0x36, "offset": 0xa1, "way": "i2c"},
            "reboot_cause_path": "/etc/sonic/.reboot/.previous-reboot-cause.txt"
        }
    ],
    "sfps": {
        "libplatform_flag": True,
        "port_index_start": 0,
        "cage_type": ['SFP'] * 48 + ['QSFP'] * 8,
        "eeprom_offset": 11,
        "eeprom_path_s3ip_flag": False,
        "presence_path_s3ip_flag": False,
        "txdis_path_s3ip_flag": False,
        "reset_path_s3ip_flag": False,
        "eeprom_path_rg_plat_flag": False,
        "presence_path_rg_plat_flag": False,
        "txdis_path_rg_plat_flag": False,
        "reset_path_rg_plat_flag": False,
        "presence_path": None,
        "txdis_path": None,
        "reset_path": None,
        "presence_cpld": {
            "dev_id": {
                5: {
                    "offset": {
                        0x30: "1-8",
                        0x31: "9-16",
                        0x32: "17-24",
                    },
                },
                6: {
                    "offset": {
                        0x30: "25-32",
                        0x31: "33-40",
                        0x32: "41-48",
                        0x33: "49-56",
                    },
                },
            }
        },
        "txdis_cpld": {
            "dev_id": {
                5: {
                    "offset": {
                        0x60: "1-8",
                        0x61: "9-16",
                        0x62: "17-24",
                    },
                    "flip": "1-24"
                },
                6: {
                    "offset": {
                        0x60: "25-32",
                        0x61: "33-40",
                        0x62: "41-48",
                    },
                    "flip": "25-48"
                },
            },
        },
        "reset_cpld": {
            "dev_id": {
                6: {
                    "offset": {
                        0xb9: "49-56",
                    },
                },
            }
        }
    }
}
