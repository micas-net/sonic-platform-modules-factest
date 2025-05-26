#coding:utf-8

psu_fan_airflow = {
    "F2B": ["PSU"],
}

psu_display_name = {
    "PSU": ["PSU"],
}

fanairflow = {
    "F2B": ["FAN"],
}

fan_display_name = {
    "FAN": ["FAN"],
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
    Power =  "W"
    Speed = "RPM"

class threshold:
    PSU_TEMP_MIN = -10 * 1000
    PSU_TEMP_MAX =  60 * 1000

    PSU_FAN_SPEED_MIN = 2000
    PSU_FAN_SPEED_MAX = 28000

    PSU_OUTPUT_VOLTAGE_MIN =11 * 1000
    PSU_OUTPUT_VOLTAGE_MAX =14 * 1000

    PSU_AC_INPUT_VOLTAGE_MIN = 200 * 1000
    PSU_AC_INPUT_VOLTAGE_MAX = 240 * 1000

    PSU_DC_INPUT_VOLTAGE_MIN = 190 * 1000
    PSU_DC_INPUT_VOLTAGE_MAX = 290 * 1000

    ERR_VALUE =  -9999999

    PSU_OUTPUT_POWER_MIN  = 10 * 1000 * 1000
    PSU_OUTPUT_POWER_MAX = 1300 * 1000 * 1000

    PSU_INPUT_POWER_MIN = 10 * 1000 * 1000
    PSU_INPUT_POWER_MAX = 1444 * 1000 * 1000

    PSU_OUTPUT_CURRENT_MIN = 2 * 1000
    PSU_OUTPUT_CURRENT_MAX = 107 * 1000

    PSU_INPUT_CURRENT_MIN = 0.2 * 1000
    PSU_INPUT_CURRENT_MAX = 7 * 1000

    FAN_SPEED_MAX = 40000
    FAN_SPEED_MIN = 5000

devices = {
    "sensor_print_src": "s3ip",

    "onie_e2": [
        {
            "name": "ONIE_E2",
            "e2loc": {"loc": "/sys/s3ip/syseeprom", "way": "sysfs"},
        },
    ],
    "psus": [
        {
            "pmbusloc": {"bus": 16, "addr": 0x60,  "way":"i2c"},
            "airflow" :psu_fan_airflow,
            "FanStatus":{"bus": 16, "addr": 0x60,  "offset":0x79, "way":"i2cword", "mask": 0x0400},
            "FanSpeed" : {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/fan1_input","way":"sysfs"},
                         "Min": threshold.PSU_FAN_SPEED_MIN,
                         "Max": threshold.PSU_FAN_SPEED_MAX,
                         "Unit": Unit.Speed
                          },
            "present": {"loc":"/sys/s3ip/psu/psu1/present", "way":"sysfs", "mask": 0x01, "okval": 1},
            "name": "PSU1",
            "psu_display_name": psu_display_name,
            "TempStatus":{"bus": 16, "addr": 0x60,  "offset":0x79, "way":"i2cword", "mask": 0x0004},
            "Temperature": {
                           "value": {"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/temp1_input","way":"sysfs"},
                           "Min": threshold.PSU_TEMP_MIN,
                           "Max": threshold.PSU_TEMP_MAX,
                           "Unit": Unit.Temperature,
                           "format":"float(float(%s)/1000)"
                          },
            "InputsStatus":{"bus": 16, "addr": 0x60,  "offset":0x79, "way":"i2cword", "mask": 0x2000},
            "InputsType":{"bus": 16, "addr": 0x60,  "offset":0x80, "way":"i2c", 'psutypedecode':psutypedecode},
            "InputsVoltage": {
                            'AC': {
                                "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/in1_input","way":"sysfs"},
                                "Min": threshold.PSU_AC_INPUT_VOLTAGE_MIN ,
                                "Max": threshold.PSU_AC_INPUT_VOLTAGE_MAX,
                                "Unit": Unit.Voltage,
                                "format":"float(float(%s)/1000)"
                                
                            },
                            'DC': {
                                "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/in1_input","way":"sysfs"},
                                "Min": threshold.PSU_DC_INPUT_VOLTAGE_MIN ,
                                "Max": threshold.PSU_DC_INPUT_VOLTAGE_MAX,
                                "Unit": Unit.Voltage,
                                "format":"float(float(%s)/1000)"
                            },
                            'other': {
                                "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/in1_input","way":"sysfs"},
                                "Min": threshold.ERR_VALUE,
                                "Max": threshold.ERR_VALUE,
                                "Unit": Unit.Voltage,
                                "format":"float(float(%s)/1000)"
                            }
                        },
            "InputsCurrent":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/curr1_input","way":"sysfs"},
                         "Min": threshold.PSU_INPUT_CURRENT_MIN,
                         "Max": threshold.PSU_INPUT_CURRENT_MAX,
                         "Unit": Unit.Current,
                         "format":"float(float(%s)/1000)"
                          },
            "InputsPower":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/power1_input","way":"sysfs"},
                         "Min": threshold.PSU_INPUT_POWER_MIN,
                         "Max": threshold.PSU_INPUT_POWER_MAX,
                         "Unit": Unit.Power,
                         "format":"float(float(%s)/1000000)"
                          },
            "OutputsStatus":{"bus": 16, "addr": 0x60,  "offset":0x79, "way":"i2cword", "mask": 0x8800},
            "OutputsVoltage":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/in2_input","way":"sysfs"},
                         "Min": threshold.PSU_OUTPUT_VOLTAGE_MIN,
                         "Max": threshold.PSU_OUTPUT_VOLTAGE_MAX,
                         "Unit": Unit.Voltage,
                         "format":"float(float(%s)/1000)"
                          },
            "OutputsCurrent":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/curr2_input","way":"sysfs"},
                         "Min": threshold.PSU_OUTPUT_CURRENT_MIN,
                         "Max": threshold.PSU_OUTPUT_CURRENT_MAX,
                         "Unit": Unit.Current,
                         "format":"float(float(%s)/1000)"
                          },
            "OutputsPower":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/power2_input","way":"sysfs"},
                         "Min": threshold.PSU_OUTPUT_POWER_MIN,
                         "Max": threshold.PSU_OUTPUT_POWER_MAX,
                         "Unit": Unit.Power,
                         "format":"float(float(%s)/1000000)"
                          },
        },
        {
            "pmbusloc": {"bus": 17, "addr": 0x60,  "way":"i2c"},
            "airflow" :psu_fan_airflow,
            "FanStatus":{"bus": 17, "addr": 0x60,  "offset":0x79, "way":"i2cword", "mask": 0x0400},
            "FanSpeed" : {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-16/16-0060/hwmon/hwmon*/fan1_input","way":"sysfs"},
                         "Min": threshold.PSU_FAN_SPEED_MIN,
                         "Max": threshold.PSU_FAN_SPEED_MAX,
                         "Unit": Unit.Speed
                          },
            "present": {"loc":"/sys/s3ip/psu/psu2/present", "way":"sysfs", "mask": 0x01, "okval": 1},
            "name": "PSU2",
            "psu_display_name": psu_display_name,
            "TempStatus":{"bus": 17, "addr": 0x60,  "offset":0x79, "way":"i2cword", "mask": 0x0004},
            "Temperature": {
                           "value": {"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/temp1_input","way":"sysfs"},
                           "Min": threshold.PSU_TEMP_MIN,
                           "Max": threshold.PSU_TEMP_MAX,
                           "Unit": Unit.Temperature,
                           "format":"float(float(%s)/1000)"
                          },
            "InputsStatus":{"bus": 17, "addr": 0x60,  "offset":0x79, "way":"i2cword", "mask": 0x2000},
            "InputsType":{"bus": 17, "addr": 0x60,  "offset":0x80, "way":"i2c", 'psutypedecode':psutypedecode},
            "InputsVoltage": {
                            'AC': {
                                "value":{"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/in1_input","way":"sysfs"},
                                "Min": threshold.PSU_AC_INPUT_VOLTAGE_MIN ,
                                "Max": threshold.PSU_AC_INPUT_VOLTAGE_MAX,
                                "Unit": Unit.Voltage,
                                "format":"float(float(%s)/1000)"
                                
                            },
                            'DC': {
                                "value":{"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/in1_input","way":"sysfs"},
                                "Min": threshold.PSU_DC_INPUT_VOLTAGE_MIN ,
                                "Max": threshold.PSU_DC_INPUT_VOLTAGE_MAX,
                                "Unit": Unit.Voltage,
                                "format":"float(float(%s)/1000)"
                            },
                            'other': {
                                "value":{"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/in1_input","way":"sysfs"},
                                "Min": threshold.ERR_VALUE,
                                "Max": threshold.ERR_VALUE,
                                "Unit": Unit.Voltage,
                                "format":"float(float(%s)/1000)"
                            }
                        },
            "InputsCurrent":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/curr1_input","way":"sysfs"},
                         "Min": threshold.PSU_INPUT_CURRENT_MIN,
                         "Max": threshold.PSU_INPUT_CURRENT_MAX,
                         "Unit": Unit.Current,
                         "format":"float(float(%s)/1000)"
                          },
            "InputsPower":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/power1_input","way":"sysfs"},
                         "Min": threshold.PSU_INPUT_POWER_MIN,
                         "Max": threshold.PSU_INPUT_POWER_MAX,
                         "Unit": Unit.Power,
                         "format":"float(float(%s)/1000000)"
                          },
            "OutputsStatus":{"bus": 17, "addr": 0x60,  "offset":0x79, "way":"i2cword", "mask": 0x8800},
            "OutputsVoltage":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/in2_input","way":"sysfs"},
                         "Min": threshold.PSU_OUTPUT_VOLTAGE_MIN,
                         "Max": threshold.PSU_OUTPUT_VOLTAGE_MAX,
                         "Unit": Unit.Voltage,
                         "format":"float(float(%s)/1000)"
                          },
            "OutputsCurrent":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/curr2_input","way":"sysfs"},
                         "Min": threshold.PSU_OUTPUT_CURRENT_MIN,
                         "Max": threshold.PSU_OUTPUT_CURRENT_MAX,
                         "Unit": Unit.Current,
                         "format":"float(float(%s)/1000)"
                          },
            "OutputsPower":
                    {
                         "value":{"loc":"/sys/bus/i2c/devices/i2c-17/17-0060/hwmon/hwmon*/power2_input","way":"sysfs"},
                         "Min": threshold.PSU_OUTPUT_POWER_MIN,
                         "Max": threshold.PSU_OUTPUT_POWER_MAX,
                         "Unit": Unit.Power,
                         "format":"float(float(%s)/1000000)"
                          },
        },
    ],
    "temps":[
        {
            "name": "SWITCH_TEMP",
            "Temperature": {
                           "value": {"loc":"/sys/s3ip/temp_sensor/temp1/value", "way":"sysfs"},
                           "Min": -30000,
                           "Max": 105000,
                           "Unit": Unit.Temperature,
                           "format":"float(float(%s)/1000)"
           }
        },
        {
            "name": "INLET_TEMP",
            "Temperature": {
                           "value": [
                                {"loc":"/sys/bus/i2c/devices/18-004b/hwmon/hwmon*/temp1_input","way":"sysfs"},
                                {"loc":"/sys/bus/i2c/devices/19-004b/hwmon/hwmon*/temp1_input","way":"sysfs"},
                            ],
                           "Min": -30000,
                           "Max": 55000,
                           "Unit": Unit.Temperature,
                           "format":"float(float(%s)/1000)"
                }
        },
        {
            "name": "OUTLET_TEMP",
            "Temperature": {
                           "value": [
                                {"loc":"/sys/bus/i2c/devices/31-0048/hwmon/hwmon*/temp1_input","way":"sysfs"},
                                {"loc":"/sys/bus/i2c/devices/32-0049/hwmon/hwmon*/temp1_input","way":"sysfs"},
                            ],
                           "Min": -30000,
                           "Max": 75000,
                           "Unit": Unit.Temperature,
                           "format":"float(float(%s)/1000)"
                }
        },
        {
            "name": "CPU_TEMP",
            "Temperature": {
                           "value": {"loc":"/sys/bus/platform/devices/coretemp.0/hwmon/hwmon*/temp1_input","way":"sysfs"},
                           "Min": -30000,
                           "Max": 102000,
                           "Unit": Unit.Temperature,
                           "format":"float(float(%s)/1000)"
                }
        },
        {
            "name": "SFF_TEMP",
            "Temperature": {
                           "value": {"loc":"/tmp/highest_sff_temp","way":"sysfs", "flock_path": "/tmp/highest_sff_temp"},
                           "Min": -15000,
                           "Max": 100000,
                           "Unit": Unit.Temperature,
                           "format":"float(float(%s)/1000)"
                }
        },
        {
            "name": "BOARD_TEMP",
            "Temperature": {
                           "value": [
                                {"loc":"/sys/bus/i2c/devices/32-0049/hwmon/hwmon*/temp1_input", "way":"sysfs"},
                                {},
                            ],
                           "Min":-30000,
                           "Max": 75000,
                           "Unit": Unit.Temperature,
                           "format":"float(float(%s)/1000)"
                }
        },
    ],
    "leds": [
        {
            "name": "BOARD_SYS_LED",
            "led": {'loc': '/dev/fpga0', 'offset': 0x1148, 'len': 1, 'way': 'devfile'},
            "led_attrs" : {
                           "green":0x04, "red":0x02, "amber":0x06, "default":0x04,
                           "flash":0x11, "light":0x04, "off": 0, "mask":0xff
            },
        },
        {
            "name": "BOARD_PSU_LED",
            "led": {'loc': '/dev/fpga0', 'offset': 0x114c, 'len': 1, 'way': 'devfile'},
            "led_attrs" : {
                           "green":0x04, "red":0x02, "amber":0x06, "default":0x04,
                           "flash":0xff, "light":0xff, "off": 0, "mask":0xff
                          },
        },
        {
            "name": "BOARD_FAN_LED",
            "led": {'loc': '/dev/fpga0', 'offset': 0x1150, 'len': 1, 'way': 'devfile'},
            "led_attrs" : {
                           "green":0x04, "red":0x02, "amber":0x06, "default":0x04,
                           "flash":0xff, "light":0xff, "off": 0, "mask":0xff
                          },
        },
    ],
    "fans": [
        {
            "name": "FAN1",
            "airflow" : fanairflow,
            "fan_display_name": fan_display_name,
            "e2loc": None,
            "present": {"loc":"/sys/s3ip/fan/fan1/present", "way":"sysfs", "mask": 0x01, "okval": 1},
            "SpeedMin" : threshold.FAN_SPEED_MIN,
            "SpeedMax" : threshold.FAN_SPEED_MAX,
            "led": None,
            "led_attrs" : None,
            "Rotor": {
                        "Rotor1_config": {  "name": "Rotor1",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1a40, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan1/motor1/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan1/motor1/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan1/motor1/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                        "Rotor2_config": {
                                            "name": "Rotor2",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1a40, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan1/motor2/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan1/motor2/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan1/motor2/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                },
        },
        {
            "name": "FAN2",
            "airflow" : fanairflow,
            "fan_display_name": fan_display_name,
            "e2loc": None,
            "present": {"loc":"/sys/s3ip/fan/fan2/present", "way":"sysfs", "mask": 0x01, "okval": 1},
            "SpeedMin" : threshold.FAN_SPEED_MIN,
            "SpeedMax" : threshold.FAN_SPEED_MAX,
            "led": None,
            "led_attrs" : None,
            "Rotor": {
                        "Rotor1_config": {  "name": "Rotor1",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1e40, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan2/motor1/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan2/motor1/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan2/motor1/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                        "Rotor2_config": {
                                            "name": "Rotor2",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1e40, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan2/motor2/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan2/motor2/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan2/motor2/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                },
        },
        {
            "name": "FAN3",
            "airflow" : fanairflow,
            "fan_display_name": fan_display_name,
            "e2loc": None,
            "present": {"loc":"/sys/s3ip/fan/fan3/present", "way":"sysfs", "mask": 0x01, "okval": 1},
            "SpeedMin" : threshold.FAN_SPEED_MIN,
            "SpeedMax" : threshold.FAN_SPEED_MAX,
            "led": None,
            "led_attrs" : None,
            "Rotor": {
                        "Rotor1_config": {  "name": "Rotor1",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1a44, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan3/motor1/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan3/motor1/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan3/motor1/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                        "Rotor2_config": {
                                            "name": "Rotor2",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1a44, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan3/motor2/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan3/motor2/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan3/motor2/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                },
        },
        {
            "name": "FAN4",
            "airflow" : fanairflow,
            "fan_display_name": fan_display_name,
            "e2loc": None,
            "present": {"loc":"/sys/s3ip/fan/fan4/present", "way":"sysfs", "mask": 0x01, "okval": 1},
            "SpeedMin" : threshold.FAN_SPEED_MIN,
            "SpeedMax" : threshold.FAN_SPEED_MAX,
            "led": None,
            "led_attrs" : None,
            "Rotor": {
                        "Rotor1_config": {  "name": "Rotor1",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1e44, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan4/motor1/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan4/motor1/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan4/motor1/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                        "Rotor2_config": {
                                            "name": "Rotor2",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1e44, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan4/motor2/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan4/motor2/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan4/motor2/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                },
        },
        {
            "name": "FAN5",
            "airflow" : fanairflow,
            "fan_display_name": fan_display_name,
            "e2loc": None,
            "present": {"loc":"/sys/s3ip/fan/fan5/present", "way":"sysfs", "mask": 0x01, "okval": 1},
            "SpeedMin" : threshold.FAN_SPEED_MIN,
            "SpeedMax" : threshold.FAN_SPEED_MAX,
            "led": None,
            "led_attrs" : None,
            "Rotor": {
                        "Rotor1_config": {  "name": "Rotor1",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1a48, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan5/motor1/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan5/motor1/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan5/motor1/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                        "Rotor2_config": {
                                            "name": "Rotor2",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1a48, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan5/motor2/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan5/motor2/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan5/motor2/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                },
        },
        {
            "name": "FAN6",
            "airflow" : fanairflow,
            "fan_display_name": fan_display_name,
            "e2loc": None,
            "present": {"loc":"/sys/s3ip/fan/fan6/present", "way":"sysfs", "mask": 0x01, "okval": 1},
            "SpeedMin" : threshold.FAN_SPEED_MIN,
            "SpeedMax" : threshold.FAN_SPEED_MAX,
            "led": None,
            "led_attrs" : None,
            "Rotor": {
                        "Rotor1_config": {  "name": "Rotor1",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1e48, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan6/motor1/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan6/motor1/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan6/motor1/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                        "Rotor2_config": {
                                            "name": "Rotor2",
                                            "Set_speed" : {'loc': '/dev/fpga0', 'offset': 0x1e48, 'len': 1, 'way': 'devfile'},
                                            "Running": {"loc":"/sys/s3ip/fan/fan6/motor2/status", "way":"sysfs", "mask": 0x01, "is_runing": 1},
                                            "HwAlarm": {"loc":"/sys/s3ip/fan/fan6/motor2/status", "way":"sysfs", "mask": 0x01, "no_alarm": 1},
                                            "SpeedMin": threshold.FAN_SPEED_MIN,
                                            "SpeedMax": threshold.FAN_SPEED_MAX,
                                            "Speed": {
                                                        "value": {"loc": "/sys/s3ip/fan/fan6/motor2/speed", "way": "sysfs"},
                                                        "Min": threshold.FAN_SPEED_MIN,
                                                        "Max": threshold.FAN_SPEED_MAX,
                                                        "Unit": Unit.Speed,
                                                    },
                                           },
                },
        },
    ],
}
