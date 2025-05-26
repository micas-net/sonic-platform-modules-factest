#!/usr/bin/env python3

import subprocess
import glob
try:
    import click
except ImportError as error:
    pass

import os
import sys
import syslog
library_path = '/usr/local/bin'
sys.path.append(library_path)
from platform_util import set_value, log_to_file, exec_os_cmd

DEFAULT_TEMP_CRIT_RECOVER_CMD = "/sbin/reboot"
BOARD_ID_PATH = "/sys/module/platform_common/parameters/dfd_my_type"
BOARD_AIRFLOW_PATH = "/etc/sonic/.airflow"
SUB_VERSION_FILE = "/etc/sonic/.subversion"
PRODUCT_NAME_PATH = "/etc/device/productname"
HOST_MACHINE = "/host/machine.conf"
COMMON_DRIVER_CONFIGS = []
FW_UPGRADE_STARTED_FLAG = "/etc/sonic/.doing_fw_upg"
WARM_UPG_FLAG = "/etc/sonic/.warm_upg_flag"
WARM_UPGRADE_STARTED_FLAG = "/etc/sonic/.doing_warm_upg"
AIRFLOW_RESULT_FILE = "/etc/sonic/.airflow"
avs_begin_sleep_time = 30

def write_sysfs_value(reg_name, value):
    mb_reg_file = "/sys/bus/i2c/devices/" + reg_name
    locations = glob.glob(mb_reg_file)
    if len(locations) == 0:
        print("%s not found" % mb_reg_file)
        return False
    sysfs_loc = locations[0]
    try:
        with open(sysfs_loc, 'w') as fd:
            fd.write(value)
    except Exception:
        return False
    return True

def platform_process_other_init(global_init_param, global_init_command):
    # Subsequent products disable the GLOBALINITPARAM configuration
    for index in global_init_param:
        if isinstance(index, dict):
            loc = index.get("loc", None)
            value = index.get("value", None)
            if loc and value:
                write_sysfs_value(loc, value)
            else:
                click.echo("%%WB_PLATFORM_PROCESS: failed to initialize the parameter, loc or value is None, config %s" % index)
        else:
            click.echo("%%WB_PLATFORM_PROCESS: failed to initialize the parameter, the config %s is not a dict type" % index)

    for index in global_init_command:
        if isinstance(index, dict):
            set_value(index)
        else:
            exec_os_cmd(index)

def platform_process_other_init_pre(global_init_param_pre, global_init_command_pre):
    # Subsequent products disable the GLOBALINITPARAM configuration
    for index in global_init_param_pre:
        if isinstance(index, dict):
            loc = index.get("loc", None)
            value = index.get("value", None)
            if loc and value:
                write_sysfs_value(loc, value)
            else:
                click.echo("%%WB_PLATFORM_PROCESS: failed to initialize the parameter, loc or value is None, config %s" % index)
        else:
            click.echo("%%WB_PLATFORM_PROCESS: failed to initialize the parameter, the config %s is not a dict type" % index)

    for index in global_init_command_pre:
        if isinstance(index, dict):
            set_value(index)
        else:
            exec_os_cmd(index)

# Constants
LOG_DIRECTORY = '/var/log/bsp_tech'
LOG_FILE_PATH = os.path.join(LOG_DIRECTORY, 'platform_base_debug.log')
LOG_WRITE_SIZE = 1 * 1024 * 1024  # 1 MB

def platform_base_log(message):
    # Ensure log directory exists
    os.makedirs(LOG_DIRECTORY, exist_ok=True)
    # Print the message and log it to the log file.
    log_to_file(message, LOG_FILE_PATH, LOG_WRITE_SIZE)