#!/usr/bin/env python3
import os
import re
from public.platform_common_config import SUB_VERSION_FILE, PRODUCT_NAME_PATH, BOARD_ID_PATH, HOST_MACHINE
from public.platform_common_config import platform_base_log
uboot_env_data = None
uboot_env_name = ["u-boot-env", "env"]

def log_message(message):
    platform_base_log(message)

def get_platform_name_from_file():
    product_name = "NA"
    if not os.path.exists(PRODUCT_NAME_PATH):
        return product_name

    with open(PRODUCT_NAME_PATH) as fd:
        product_name = fd.read().strip().lower()
    return product_name

def get_product_board_id_from_file():
    if not os.path.exists(BOARD_ID_PATH):
        return "NA"

    with open(BOARD_ID_PATH) as fd:
        id_str = fd.read().strip()
    return "0x%x" % (int(id_str, 10))

def find_mtd_device_by_name():
    mtd_base = '/sys/class/mtd'
    try:
        for target_name in uboot_env_name:
            for entry in os.listdir(mtd_base): # ls -l /sys/class/mtd
                entry_path = os.path.join(mtd_base, entry)
                name_path = os.path.join(entry_path, 'name')
                if os.path.isfile(name_path):
                    with open(name_path, 'r') as f:
                        name = f.read().strip()
                    if name == target_name:
                        mtd_dev = "/dev/" + entry
                        log_message("uboot env mtd dev: %s" % mtd_dev)
                        return mtd_dev
        log_message("Can't find uboot env name: %s" % uboot_env_name)
        return None
    except Exception as e:
        log_message(f"Error accessing {mtd_base}: {e}")
        return None

def get_val_from_env(val):
    global uboot_env_data
    try:
        if not uboot_env_data:
            uboot_dev = find_mtd_device_by_name()
            if uboot_dev is None:
                log = "uboot_env name: %s not found" % uboot_env_name
                return False, log
            with open(uboot_dev, 'rb') as f:
                uboot_env_data = f.read()

        decoded_data = uboot_env_data.decode('ascii', errors='ignore')
        match = re.search(fr'{val}=([^ \x00]+)', decoded_data)
        if match:
            return True, match.group(1)
        else:
            log = "%s not match" % val
            return False, log

    except Exception as e:
        log = "get %s from env fail, reason: %s" % (val, str(e))
        return False, log

def get_platform_device_name_from_env():
    ret, product_name = get_val_from_env("conffitname")
    if not ret:
        log_message("get product name error, reason: %s." % product_name)
        return "NA"
    return product_name

def get_product_board_id_from_env():
    ret, board_id = get_val_from_env("board_id")
    if not ret:
        log_message("get board id error, reason: %s." % board_id)
        return "NA"
    return board_id

def get_machine_info():
    if not os.path.isfile(HOST_MACHINE):
        return None
    machine_vars = {}
    with open(HOST_MACHINE) as machine_file:
        for line in machine_file:
            tokens = line.split('=')
            if len(tokens) < 2:
                continue
            machine_vars[tokens[0]] = tokens[1].strip()
    return machine_vars

def get_sonic_platform_info(machine_info=None):
    if machine_info is None:
        machine_info = get_machine_info()
    if machine_info is not None:
        if 'onie_platform' in machine_info:
            return machine_info['onie_platform']
        if 'aboot_platform' in machine_info:
            return machine_info['aboot_platform']
    return None

def get_bmc_platform_info():
    if os.path.isfile(PRODUCT_NAME_PATH):
        return get_platform_name_from_file()
    return get_platform_device_name_from_env()

def get_platform_info(machine_info = None):
    if os.path.isfile(HOST_MACHINE):
        # sonic get platform info
        return get_sonic_platform_info(machine_info)
    else:
        # bmc get platform info
        return get_bmc_platform_info()

def get_sonic_board_id(machine_info=None):
    if machine_info is None:
        machine_info = get_machine_info()
    if machine_info is not None:
        if 'onie_board_id' in machine_info:
            return machine_info['onie_board_id'].lower()
    return "NA"

def get_bmc_board_id():
    if os.path.isfile(BOARD_ID_PATH):
        return get_product_board_id_from_file()
    # This function is used by BMC
    return get_product_board_id_from_env();

def get_board_id(machine_info = None):
    if os.path.isfile(HOST_MACHINE):
        # sonic get board id
        return get_sonic_board_id(machine_info)
    else:
        # bmc get board id
        return get_bmc_board_id()


def get_onie_machine(machine_info):
    if machine_info is not None:
        if 'onie_machine' in machine_info:
            return machine_info['onie_machine']
    return None


def get_sub_version():
    if not os.path.isfile(SUB_VERSION_FILE):
        return "NA"
    with open(SUB_VERSION_FILE) as fd:
        sub_ver = fd.read().strip()
    return sub_ver



