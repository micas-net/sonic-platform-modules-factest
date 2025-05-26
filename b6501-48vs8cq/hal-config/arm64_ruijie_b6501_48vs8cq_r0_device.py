#!/usr/bin/python3

psu_fan_airflow = {
    "intake": ['CSU550AP-3-500', 'DPS-550AB-39 A', 'GW-CRPS550N2C', 'CSU550AP-3-300', 'DPS-550AB-39 B', 'CSU550AP-3', 'U1D-D10800-DRB'],
    "exhaust": ['CSU550AP-3-501', 'DPS-550AB-40 A', 'GW-CRPS550N2RC']
}

fanairflow = {
    "intake": ['M1HFAN III-F', 'M1EFAN II-F', 'M1EFAN III-F'],
    "exhaust": ['M1HFAN III-R', 'M1EFAN IV-R'],
}

psu_display_name = {
    "PA550II-F": ['CSU550AP-3-500', 'DPS-550AB-39 A', 'GW-CRPS550N2C', 'CSU550AP-3-300', 'DPS-550AB-39 B', 'CSU550AP-3'],
    "PA550II-R": ['CSU550AP-3-501', 'DPS-550AB-40 A', 'GW-CRPS550N2RC'],
    "PD800I-F": ['U1D-D10800-DRB']
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
    PSU_TEMP_MIN = -20 * 1000
    PSU_TEMP_MAX = 60 * 1000

    PSU_FAN_SPEED_MIN = 2000
    PSU_FAN_SPEED_MAX = 18000

    PSU_OUTPUT_VOLTAGE_MIN = 11 * 1000
    PSU_OUTPUT_VOLTAGE_MAX = 14 * 1000

    PSU_AC_INPUT_VOLTAGE_MIN = 200 * 1000
    PSU_AC_INPUT_VOLTAGE_MAX = 240 * 1000

    PSU_DC_INPUT_VOLTAGE_MIN = 190 * 1000
    PSU_DC_INPUT_VOLTAGE_MAX = 290 * 1000

    ERR_VALUE = -9999999

    PSU_OUTPUT_POWER_MIN = 10 * 1000 * 1000
    PSU_OUTPUT_POWER_MAX = 560 * 1000 * 1000

    PSU_INPUT_POWER_MIN = 10 * 1000 * 1000
    PSU_INPUT_POWER_MAX = 625 * 1000 * 1000

    PSU_OUTPUT_CURRENT_MIN = 1 * 1000
    PSU_OUTPUT_CURRENT_MAX = 45 * 1000

    PSU_INPUT_CURRENT_MIN = 0 * 1000
    PSU_INPUT_CURRENT_MAX = 7 * 1000

    FRONT_FAN_SPEED_MAX = 24000
    REAR_FAN_SPEED_MAX = 22500
    FAN_SPEED_MIN = 5000


    ASPOWER_DC_PSU_TEMP_MIN = -10 * 1000
    ASPOWER_DC_PSU_TEMP_MAX = 55 * 1000

    ASPOWER_DC_PSU_FAN_SPEED_MIN = 800
    ASPOWER_DC_PSU_FAN_SPEED_MAX = 24000

    ASPOWER_DC_PSU_OUTPUT_VOLTAGE_MIN = 11.4 * 1000
    ASPOWER_DC_PSU_OUTPUT_VOLTAGE_MAX = 12.6 * 1000

    ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MIN = 36 * 1000
    ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MAX = 72 * 1000

    ASPOWER_DC_ERR_VALUE = -9999999

    ASPOWER_DC_PSU_OUTPUT_POWER_MIN = 5 * 1000 * 1000
    ASPOWER_DC_PSU_OUTPUT_POWER_MAX = 800 * 1000 * 1000

    ASPOWER_DC_PSU_INPUT_POWER_MIN = 5 * 1000 * 1000
    ASPOWER_DC_PSU_INPUT_POWER_MAX = 880 * 1000 * 1000

    ASPOWER_DC_PSU_OUTPUT_CURRENT_MIN = 0.5 * 1000
    ASPOWER_DC_PSU_OUTPUT_CURRENT_MAX = 66 * 1000

    ASPOWER_DC_PSU_INPUT_CURRENT_MIN = 1 * 1000
    ASPOWER_DC_PSU_INPUT_CURRENT_MAX = 28 * 1000


class Description:
    CPLD = "Used for managing IO modules, SFP+ modules and system LEDs"
    BIOS = "Performs initialization of hardware components during booting"
    FPGA = "Platform management controller for on-board temperature monitoring, in-chassis power"


devices = {
    "sensor_print_src": "s3ip",

    "dcdc_data_source": [
        {
            "path": "/sys/s3ip/vol_sensor",
            "type": "vol",
            "Unit": Unit.Voltage,
            "read_times": 3,
            "format": "float(float(%s)/1000)"
        },
        {
            "path": "/sys/s3ip/curr_sensor",
            "type": "curr",
            "Unit": Unit.Current,
            "read_times": 3,
            "format": "float(float(%s)/1000)"
        },
    ],
    "temp_data_source": [
        {
            "path": "/sys/s3ip/temp_sensor",
            "type": "temp",
            "Unit": Unit.Temperature,
        },
    ],

    "onie_e2": [
        {
            "name": "ONIE_E2",
            "e2loc": {"loc": "/sys/bus/i2c/devices/0-0057/eeprom", "way": "sysfs"},
            "airflow": "intake"
        },
    ],
    "psus": [
        {
            "e2loc": {"loc": "/sys/bus/i2c/devices/26-0050/eeprom", "way": "sysfs"},
            "pmbusloc": {"bus": 26, "addr": 0x58, "way": "i2c"},
            "present": {"loc": "/sys/s3ip/psu/psu1/present", "way": "sysfs", "mask": 0x01, "okval": 1},
            "name": "PSU1",
            "psu_sn": {"loc": "/sys/s3ip/psu/psu1/serial_number", "way": "sysfs"},
            "psu_pn": {"loc": "/sys/s3ip/psu/psu1/part_number", "way": "sysfs"},
            "psu_hw": {"loc": "/sys/s3ip/psu/psu1/hardware_version", "way": "sysfs"},
            "psu_vendor": {"loc": "/sys/s3ip/psu/psu1/vendor", "way": "sysfs"},
            "get_threshold_by_model": 1,
            "psu_display_name": psu_display_name,
            "airflow": psu_fan_airflow,
            "TempStatus": {"bus": 26, "addr": 0x58, "offset": 0x79, "way": "i2cword", "mask": 0x0004},
            "Temperature": {
                "value": {"loc": "/sys/s3ip/psu/psu1/temp1/value", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_TEMP_MIN,
                    "other": threshold.PSU_TEMP_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_TEMP_MAX,
                    "other": threshold.PSU_TEMP_MAX,
                },
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            },
            "FanStatus": {"bus": 26, "addr": 0x58, "offset": 0x79, "way": "i2cword", "mask": 0x0400},
            "FanSpeed": {
                "value": {"loc": "/sys/s3ip/psu/psu1/fan_speed", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_FAN_SPEED_MIN,
                    "other": threshold.PSU_FAN_SPEED_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_FAN_SPEED_MAX,
                    "other": threshold.PSU_FAN_SPEED_MAX,
                },
                "Unit": Unit.Speed
            },
            "psu_fan_tolerance": 40,
            "InputsStatus": {"loc": "/sys/s3ip/psu/psu1/in_status", "way": "sysfs", "mask": 0x1, "okval": 1},
            "InputsType": {"bus": 26, "addr": 0x58, "offset": 0x80, "way": "i2c", 'psutypedecode': psutypedecode},
            "InputsVoltage": {
                'AC': {
                    "value": {"loc": "/sys/s3ip/psu/psu1/in_vol", "way": "sysfs"},
                    "Min": threshold.PSU_AC_INPUT_VOLTAGE_MIN,
                    "Max": threshold.PSU_AC_INPUT_VOLTAGE_MAX,
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"

                },
                'DC': {
                    "value": {"loc": "/sys/s3ip/psu/psu1/in_vol", "way": "sysfs"},
                    "Min": {
                        "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MIN,
                        "other": threshold.PSU_DC_INPUT_VOLTAGE_MIN,
                    },
                    "Max": {
                        "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MAX,
                        "other": threshold.PSU_DC_INPUT_VOLTAGE_MAX,
                    },
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                },
                'other': {
                    "value": {"loc": "/sys/s3ip/psu/psu1/in_vol", "way": "sysfs"},
                    "Min": {
                        "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MIN,
                        "other": threshold.ERR_VALUE,
                    },
                    "Max": {
                        "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MAX,
                        "other": threshold.ERR_VALUE,
                    },
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                }
            },
            "InputsCurrent": {
                "value": {"loc": "/sys/s3ip/psu/psu1/in_curr", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_INPUT_CURRENT_MIN,
                    "other": threshold.PSU_INPUT_CURRENT_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_INPUT_CURRENT_MAX,
                    "other": threshold.PSU_INPUT_CURRENT_MAX,
                },
                "Unit": Unit.Current,
                "format": "float(float(%s)/1000)"
            },
            "InputsPower": {
                "value": {"loc": "/sys/s3ip/psu/psu1/in_power", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_INPUT_POWER_MIN,
                    "other": threshold.PSU_INPUT_POWER_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_INPUT_POWER_MAX,
                    "other": threshold.PSU_INPUT_POWER_MAX,
                },
                "Unit": Unit.Power,
                "format": "float(float(%s)/1000000)"
            },
            "OutputsStatus": {"loc": "/sys/s3ip/psu/psu1/out_status", "way": "sysfs", "mask": 0x1, "okval": 1},
            "OutputsVoltage": {
                "value": {"loc": "/sys/s3ip/psu/psu1/out_vol", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_VOLTAGE_MIN,
                    "other": threshold.PSU_OUTPUT_VOLTAGE_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_VOLTAGE_MAX,
                    "other": threshold.PSU_OUTPUT_VOLTAGE_MAX,
                },
                "Unit": Unit.Voltage,
                "format": "float(float(%s)/1000)"
            },
            "OutputsCurrent": {
                "value": {"loc": "/sys/s3ip/psu/psu1/out_curr", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_CURRENT_MIN,
                    "other": threshold.PSU_OUTPUT_CURRENT_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_CURRENT_MAX,
                    "other": threshold.PSU_OUTPUT_CURRENT_MAX,
                },
                "Unit": Unit.Current,
                "format": "float(float(%s)/1000)"
            },
            "OutputsPower": {
                "value": {"loc": "/sys/s3ip/psu/psu1/out_power", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_POWER_MIN,
                    "other": threshold.PSU_OUTPUT_POWER_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_POWER_MAX,
                    "other": threshold.PSU_OUTPUT_POWER_MAX,
                },
                "Unit": Unit.Power,
                "format": "float(float(%s)/1000000)"
            },
        },
        {
            "e2loc": {"loc": "/sys/bus/i2c/devices/25-0050/eeprom", "way": "sysfs"},
            "pmbusloc": {"bus": 27, "addr": 0x5b, "way": "i2c"},
            "present": {"loc": "/sys/s3ip/psu/psu2/present", "way": "sysfs", "mask": 0x01, "okval": 1},
            "name": "PSU2",
            "psu_sn": {"loc": "/sys/s3ip/psu/psu2/serial_number", "way": "sysfs"},
            "psu_pn": {"loc": "/sys/s3ip/psu/psu2/part_number", "way": "sysfs"},
            "psu_hw": {"loc": "/sys/s3ip/psu/psu2/hardware_version", "way": "sysfs"},
            "psu_vendor": {"loc": "/sys/s3ip/psu/psu2/vendor", "way": "sysfs"},
            "get_threshold_by_model": 1,
            "psu_display_name": psu_display_name,
            "airflow": psu_fan_airflow,
            "TempStatus": {"bus": 27, "addr": 0x5b, "offset": 0x79, "way": "i2cword", "mask": 0x0004},
            "Temperature": {
                "value": {"loc": "/sys/s3ip/psu/psu2/temp1/value", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_TEMP_MIN,
                    "other": threshold.PSU_TEMP_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_TEMP_MAX,
                    "other": threshold.PSU_TEMP_MAX,
                },
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            },
            "FanStatus": {"bus": 27, "addr": 0x5b, "offset": 0x79, "way": "i2cword", "mask": 0x0400},
            "FanSpeed": {
                "value": {"loc": "/sys/s3ip/psu/psu2/fan_speed", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_FAN_SPEED_MIN,
                    "other": threshold.PSU_FAN_SPEED_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_FAN_SPEED_MAX,
                    "other": threshold.PSU_FAN_SPEED_MAX,
                },
                "Unit": Unit.Speed
            },
            "psu_fan_tolerance": 40,
            "InputsStatus": {"loc": "/sys/s3ip/psu/psu2/in_status", "way": "sysfs", "mask": 0x1, "okval": 1},
            "InputsType": {"bus": 27, "addr": 0x5b, "offset": 0x80, "way": "i2c", 'psutypedecode': psutypedecode},
            "InputsVoltage": {
                'AC': {
                    "value": {"loc": "/sys/s3ip/psu/psu2/in_vol", "way": "sysfs"},
                    "Min": threshold.PSU_AC_INPUT_VOLTAGE_MIN,
                    "Max": threshold.PSU_AC_INPUT_VOLTAGE_MAX,
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"

                },
                'DC': {
                    "value": {"loc": "/sys/s3ip/psu/psu2/in_vol", "way": "sysfs"},
                    "Min": {
                        "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MIN,
                        "other": threshold.PSU_DC_INPUT_VOLTAGE_MIN,
                    },
                    "Max": {
                        "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MAX,
                        "other": threshold.PSU_DC_INPUT_VOLTAGE_MAX,
                    },
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                },
                'other': {
                    "value": {"loc": "/sys/s3ip/psu/psu2/in_vol", "way": "sysfs"},
                    "Min": {
                        "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MIN,
                        "other": threshold.ERR_VALUE,
                    },
                    "Max": {
                        "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_DC_INPUT_VOLTAGE_MAX,
                        "other": threshold.ERR_VALUE,
                    },
                    "Unit": Unit.Voltage,
                    "format": "float(float(%s)/1000)"
                }
            },
            "InputsCurrent": {
                "value": {"loc": "/sys/s3ip/psu/psu2/in_curr", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_INPUT_CURRENT_MIN,
                    "other": threshold.PSU_INPUT_CURRENT_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_INPUT_CURRENT_MAX,
                    "other": threshold.PSU_INPUT_CURRENT_MAX,
                },
                "Unit": Unit.Current,
                "format": "float(float(%s)/1000)"
            },
            "InputsPower": {
                "value": {"loc": "/sys/s3ip/psu/psu2/in_power", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_INPUT_POWER_MIN,
                    "other": threshold.PSU_INPUT_POWER_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_INPUT_POWER_MAX,
                    "other": threshold.PSU_INPUT_POWER_MAX,
                },
                "Unit": Unit.Power,
                "format": "float(float(%s)/1000000)"
            },
            "OutputsStatus": {"loc": "/sys/s3ip/psu/psu2/out_status", "way": "sysfs", "mask": 0x1, "okval": 1},
            "OutputsVoltage": {
                "value": {"loc": "/sys/s3ip/psu/psu2/out_vol", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_VOLTAGE_MIN,
                    "other": threshold.PSU_OUTPUT_VOLTAGE_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_VOLTAGE_MAX,
                    "other": threshold.PSU_OUTPUT_VOLTAGE_MAX,
                },
                "Unit": Unit.Voltage,
                "format": "float(float(%s)/1000)"
            },
            "OutputsCurrent": {
                "value": {"loc": "/sys/s3ip/psu/psu2/out_curr", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_CURRENT_MIN,
                    "other": threshold.PSU_OUTPUT_CURRENT_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_CURRENT_MAX,
                    "other": threshold.PSU_OUTPUT_CURRENT_MAX,
                },
                "Unit": Unit.Current,
                "format": "float(float(%s)/1000)"
            },
            "OutputsPower": {
                "value": {"loc": "/sys/s3ip/psu/psu2/out_power", "way": "sysfs"},
                "Min": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_POWER_MIN,
                    "other": threshold.PSU_OUTPUT_POWER_MIN,
                },
                "Max": {
                    "U1D-D10800-DRB": threshold.ASPOWER_DC_PSU_OUTPUT_POWER_MAX,
                    "other": threshold.PSU_OUTPUT_POWER_MAX,
                },
                "Unit": Unit.Power,
                "format": "float(float(%s)/1000000)"
            },
        }
    ],
    "temps": [
        {
            "name": "CPU_TEMP",
            "temp_id": "TEMP1",
            "Temperature": {
                "value": {"loc": "/sys/s3ip/temp_sensor/temp5/value", "way": "sysfs"},
                "Min": -20000,
                "Low": -20000,
                "High": 80000,
                "Max": 85000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "INLET_TEMP",
            "temp_id": "TEMP2",
            "Temperature": {
                "value": {"loc": "/sys/s3ip/temp_sensor/temp4/value", "way": "sysfs"},
                "Min": -30000,
                "Low": -30000,
                "High": 55000,
                "Max": 60000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            },
            "fix_value": {
                "fix_type": "config",
                "addend": -3,
            }
        },
        {
            "name": "OUTLET_TEMP",
            "temp_id": "TEMP3",
            "Temperature": {
                "value": {"loc": "/sys/s3ip/temp_sensor/temp3/value", "way": "sysfs"},
                "Min": -30000,
                "Low": -30000,
                "High": 70000,
                "Max": 75000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "BOARD_TEMP",
            "temp_id": "TEMP4",
            "Temperature": {
                "value": [
                    {"loc": "/sys/s3ip/temp_sensor/temp1/value", "way": "sysfs"},
                    {"loc": "/sys/s3ip/temp_sensor/temp2/value", "way": "sysfs"},
                ],
                "Min": -55000,
                "Low": -55000,
                "High": 75000,
                "Max": 80000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "SWITCH_TEMP",
            "temp_id": "TEMP5",
            "api_name": "ASIC_TEMP",
            "Temperature": {
                "value": {"loc": "/sys/s3ip/temp_sensor/temp6/value", "way": "sysfs"},
                "Min": -55000,
                "Low": -55000,
                "High": 105000,
                "Max": 110000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
        {
            "name": "SFF_TEMP",
            "Temperature": {
                "value": {"loc": "/tmp/highest_sff_temp", "way": "sysfs", "flock_path": "/tmp/highest_sff_temp"},
                "Min": -40000,
                "Low": -40000,
                "High": 80000,
                "Max": 100000,
                "Unit": Unit.Temperature,
                "format": "float(float(%s)/1000)"
            }
        },
    ],
    "leds": [
        {
            "name": "SYS_LED",
            "led_type": "SYS_LED",
            "led": {"loc": "/sys/s3ip/sysled/sys_led_status", "way": "sysfs"},
            "led_attrs": {
                "off": 0x00, "red": 0x05, "green_flash": 0x03, "green": 0x01,
                "amber": 0x02
            },
        },
        {
            "name": "ID_LED",
            "led_type": "ID_LED",
            "led": {"loc": "/sys/s3ip/sysled/id_led_status", "way": "sysfs"},
            "led_attrs": {
                "off": 0x00, "blue": 0x04, "blue_flash": 0x08
            },
        },
    ],
    "fans": [
        {
            "name": "FAN1",
            "airflow": fanairflow,
            "e2_type": "fantlv",
            "e2loc": {'loc': '/sys/bus/i2c/devices/66-0053/eeprom', 'way': 'sysfs'},
            "present": {"loc": "/sys/s3ip/fan/fan1/status", "way": "sysfs", "mask": 0x03, "okval": [1, 2]},
            "SpeedMin": threshold.FAN_SPEED_MIN,
            "SpeedMax": threshold.FRONT_FAN_SPEED_MAX,
            "led": {"loc": "/sys/s3ip/fan/fan1/led_status", "way": "sysfs"},
            "led_attrs": {
                "off": 0x00, "red": 0x03, "green": 0x01, "amber": 0x02
            },
            "PowerMax": 38.4,
            "Rotor": {
                "Rotor1_config": {
                    "name": "Rotor1",
                    "Set_speed": {'loc': '/dev/cpld1', "offset": 0x4080, "len": 1, "way": "devfile"},
                    "Running": {"loc": "/sys/s3ip/fan/fan1/status", "way": "sysfs", "mask": 0x03, "is_runing": 1},
                    "HwAlarm": {"loc": "/sys/s3ip/fan/fan1/status", "way": "sysfs", "mask": 0x03, "no_alarm": 1},
                    "SpeedMin": threshold.FAN_SPEED_MIN,
                    "SpeedMax": threshold.FRONT_FAN_SPEED_MAX,
                    "Speed": {
                        "value": {"loc": "/sys/s3ip/fan/fan1/motor1/speed", "way": "sysfs"},
                        "Min": threshold.FAN_SPEED_MIN,
                        "Max": threshold.FRONT_FAN_SPEED_MAX,
                        "Unit": Unit.Speed,
                    },
                    "tolerance": 46,
                },
            },
        },
        {
            "name": "FAN2",
            "airflow": fanairflow,
            "e2_type": "fantlv",
            "e2loc": {'loc': '/sys/bus/i2c/devices/67-0053/eeprom', 'way': 'sysfs'},
            "present": {"loc": "/sys/s3ip/fan/fan2/status", "way": "sysfs", "mask": 0x03, "okval": [1, 2]},
            "SpeedMin": threshold.FAN_SPEED_MIN,
            "SpeedMax": threshold.FRONT_FAN_SPEED_MAX,
            "led": {"loc": "/sys/s3ip/fan/fan2/led_status", "way": "sysfs"},
            "led_attrs": {
                "off": 0x00, "red": 0x03, "green": 0x01, "amber": 0x02
            },
            "PowerMax": 38.4,
            "Rotor": {
                "Rotor1_config": {
                    "name": "Rotor1",
                    "Set_speed": {'loc': '/dev/cpld1', "offset": 0x4081, "len": 1, "way": "devfile"},
                    "Running": {"loc": "/sys/s3ip/fan/fan2/status", "way": "sysfs", "mask": 0x03, "is_runing": 1},
                    "HwAlarm": {"loc": "/sys/s3ip/fan/fan2/status", "way": "sysfs", "mask": 0x03, "no_alarm": 1},
                    "SpeedMin": threshold.FAN_SPEED_MIN,
                    "SpeedMax": threshold.FRONT_FAN_SPEED_MAX,
                    "Speed": {
                        "value": {"loc": "/sys/s3ip/fan/fan2/motor1/speed", "way": "sysfs"},
                        "Min": threshold.FAN_SPEED_MIN,
                        "Max": threshold.FRONT_FAN_SPEED_MAX,
                        "Unit": Unit.Speed,
                    },
                    "tolerance": 46,
                },
            },
        },
        {
            "name": "FAN3",
            "airflow": fanairflow,
            "e2_type": "fantlv",
            "e2loc": {'loc': '/sys/bus/i2c/devices/68-0053/eeprom', 'way': 'sysfs'},
            "present": {"loc": "/sys/s3ip/fan/fan3/status", "way": "sysfs", "mask": 0x03, "okval": [1, 2]},
            "SpeedMin": threshold.FAN_SPEED_MIN,
            "SpeedMax": threshold.FRONT_FAN_SPEED_MAX,
            "led": {"loc": "/sys/s3ip/fan/fan3/led_status", "way": "sysfs"},
            "led_attrs": {
                "off": 0x00, "red": 0x03, "green": 0x01, "amber": 0x02
            },
            "PowerMax": 38.4,
            "Rotor": {
                "Rotor1_config": {
                    "name": "Rotor1",
                    "Set_speed": {'loc': '/dev/cpld1', "offset": 0x4082, "len": 1, "way": "devfile"},
                    "Running": {"loc": "/sys/s3ip/fan/fan3/status", "way": "sysfs", "mask": 0x03, "is_runing": 1},
                    "HwAlarm": {"loc": "/sys/s3ip/fan/fan3/status", "way": "sysfs", "mask": 0x03, "no_alarm": 1},
                    "SpeedMin": threshold.FAN_SPEED_MIN,
                    "SpeedMax": threshold.FRONT_FAN_SPEED_MAX,
                    "Speed": {
                        "value": {"loc": "/sys/s3ip/fan/fan3/motor1/speed", "way": "sysfs"},
                        "Min": threshold.FAN_SPEED_MIN,
                        "Max": threshold.FRONT_FAN_SPEED_MAX,
                        "Unit": Unit.Speed,
                    },
                    "tolerance": 46,
                },
            },
        },
        {
            "name": "FAN4",
            "airflow": fanairflow,
            "e2_type": "fantlv",
            "e2loc": {'loc': '/sys/bus/i2c/devices/69-0053/eeprom', 'way': 'sysfs'},
            "present": {"loc": "/sys/s3ip/fan/fan4/status", "way": "sysfs", "mask": 0x03, "okval": [1, 2]},
            "SpeedMin": threshold.FAN_SPEED_MIN,
            "SpeedMax": threshold.FRONT_FAN_SPEED_MAX,
            "led": {"loc": "/sys/s3ip/fan/fan4/led_status", "way": "sysfs"},
            "led_attrs": {
                "off": 0x00, "red": 0x03, "green": 0x01, "amber": 0x02
            },
            "PowerMax": 38.4,
            "Rotor": {
                "Rotor1_config": {
                    "name": "Rotor1",
                    "Set_speed": {'loc': '/dev/cpld1', "offset": 0x4083, "len": 1, "way": "devfile"},
                    "Running": {"loc": "/sys/s3ip/fan/fan4/status", "way": "sysfs", "mask": 0x03, "is_runing": 1},
                    "HwAlarm": {"loc": "/sys/s3ip/fan/fan4/status", "way": "sysfs", "mask": 0x03, "no_alarm": 1},
                    "SpeedMin": threshold.FAN_SPEED_MIN,
                    "SpeedMax": threshold.FRONT_FAN_SPEED_MAX,
                    "Speed": {
                        "value": {"loc": "/sys/s3ip/fan/fan4/motor1/speed", "way": "sysfs"},
                        "Min": threshold.FAN_SPEED_MIN,
                        "Max": threshold.FRONT_FAN_SPEED_MAX,
                        "Unit": Unit.Speed,
                    },
                    "tolerance": 46,
                },
            },
        },

    ],
    "cplds": [
        {
            "name": "CPU_CPLD",
            "cpld_id": "CPLD1",
            "VersionFile": {"loc": "/dev/cpld0", "offset": 0, "len": 4, "way": "devfile_ascii"},
            "desc": "Used for system power",
            "slot": 0,
            "warm": 0,
        },
        {
            "name": "MAC_MAIN_CPLD",
            "cpld_id": "CPLD2",
            "VersionFile": {"loc": "/dev/cpld1", "offset": 0x2000, "len": 4, "way": "devfile_ascii"},
            "desc": "Used for base functions",
            "slot": 0,
            "warm": 0,
        },
        {
            "name": "MAC_SECOND_CPLD",
            "cpld_id": "CPLD3",
            "VersionFile": {"loc": "/dev/cpld1", "offset": 0x4000, "len": 4, "way": "devfile_ascii"},
            "desc": "Used for sff functions",
            "slot": 0,
            "warm": 0,
        },
        {
            "name": "RAS_CPLD",
            "cpld_id": "CPLD4",
            "VersionFile": {"loc": "/dev/cpld1", "offset": 0x20a0, "len": 4, "way": "devfile_ascii"},
            "desc": "Used for dcdc functions",
            "slot": 0,
            "warm": 0,
        },
    ],
    "cpu": [
        {
            "name": "cpu",
            "reboot_cause_path": "/etc/sonic/.reboot/.previous-reboot-cause.txt"
        }
    ],
    "sfps": {
        "ver": '2.0',
        "port_index_start": 1,
        "port_num": 56,
        "log_level": 2,
        "eeprom_retry_times": 5,
        "eeprom_retry_break_sec": 0.2,
        "presence_path": "/sys/s3ip/transceiver/eth%d/present",
        "presence_val_is_present": 1,
        "eeprom_path": "/sys/s3ip/transceiver/eth%d/eeprom",
        "eeprom_path_key": list(range(1, 57)),
        "optoe_driver_path": "/sys/bus/i2c/devices/i2c-%d/%d-0050/dev_class",
        "optoe_driver_key": list(range(2, 26)) + list(range(34, 66)),
        "reset_path": "/sys/s3ip/transceiver/eth%d/reset",
        "txdis_path": "/sys/s3ip/transceiver/eth%d/tx_disable",
    }
}
