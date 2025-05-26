#!/usr/bin/python3
import re

__all__ = [
    "CPO_PLATFORM_CONFIG",
    "CPO_PLATFORM_PORT_CONFIG",
    "CPO_PLATFORM_MEDIA_SETTINGS",
    "OE_NUM",
    "OE_BANK_NUM",
    "OE_RLM_NUM",
    "OE_INDEX_LIST",
    "RLM_INDEX_LIST",
    "OE_BUS_MAP",
    "OE_I2C_PATH_MAP",
    "helper_logger",
    "Port_Mapping",
    "PORT_INDEX_LIST",
    "BANK_LANE_NUM"
]

debug_level = 1

CPO_PLATFORM_CONFIG = {
    "oe_num": 8,
    "oe_index_start_index": 0,
    "port_index_start_index": 1,
    "rlm_index_start_index": 0,
    "oe_bank_num": 8,
    "oe_rlm_num": 2,
    "oe_index_list": list(range(0, 8)),
    "rlm_index_list": list(range(0, 16)),
    "oe_i2c_path_format": "/sys/bus/i2c/devices/{}-0050/eeprom",
    "rlm_presence_fpga_dict": {
        "/dev/fpga1": {
            "offset": {
                "0x64": [None]*8 + list(range(8)) + [None]*8 + list(range(8,16))
            }
        }
    },
    "oe_bus_map": {
        0: 24,
        1: 25,
        2: 26,
        3: 27,
        4: 28,
        5: 29,
        6: 30,
        7: 31
    },
    "rlm_oe_map": {
        0:0,     1:0,    
        2:1,     3:1,
        4:2,     5:2,
        6:3,     7:3,
        8:4,     9:4,
        10:5,    11:5,
        12:6,    13:6,
        14:7,    15:7
    },
    "bank_oe_map": {
        0:0,     1:0,     2:0,     3:0,     4:0,     5:0,     6:0,     7:0,    
        8:1,     9:1,     10:1,    11:1,    12:1,    13:1,    14:1,    15:1,
        16:2,    17:2,    18:2,    19:2,    20:2,    21:2,    22:2,    23:2,
        24:3,    25:3,    26:3,    27:3,    28:3,    29:3,    30:3,    31:3,
        32:4,    33:4,    34:4,    35:4,    36:4,    37:4,    38:4,    39:4,
        40:5,    41:5,    42:5,    43:5,    44:5,    45:5,    46:5,    47:5,
        48:6,    49:6,    50:6,    51:6,    52:6,    53:6,    54:6,    55:6,
        56:7,    57:7,    58:7,    59:7,    60:7,    61:7,    62:7,    63:7
    },
    "port_bank_map": {
        1:0,     3:1,     2:0,     4:1,     5:2,     7:3,     6:2,     8:3,     9:4,     10:4,    11:5,    12:5,    13:6,    14:6,    15:7,    16:7,
        17:8,    18:8,    19:9,    20:9,    21:10,   22:10,   23:11,   24:11,   25:12,   26:12,   27:13,   28:13,   29:14,   30:14,   31:15,   32:15,
        33:16,   34:16,   35:17,   36:17,   37:18,   38:18,   39:19,   40:19,   41:20,   42:20,   43:21,   44:21,   45:22,   46:22,   47:23,   48:23,
        49:24,   50:24,   51:25,   52:25,   53:26,   54:26,   55:27,   56:27,   57:28,   59:29,   58:28,   60:29,   61:30,   63:31,   62:30,   64:31,
        65:32,   67:33,   66:32,   68:33,   69:34,   71:35,   70:34,   72:35,   73:36,   74:36,   75:37,   76:37,   77:38,   78:38,   79:39,   80:39,
        81:40,   82:40,   83:41,   84:41,   85:42,   86:42,   87:43,   88:43,   89:44,   90:44,   91:45,   92:45,   93:46,   94:46,   95:47,   96:47,
        97:48,   98:48,   99:49,   100:49,  101:50,  102:50,  103:51,  104:51,  105:52,  106:52,  107:53,  108:53,  109:54,  110:54,  111:55,  112:55,
        113:56,  114:56,  115:57,  116:57,  117:58,  118:58,  119:59,  120:59,  121:60,  123:61,  122:60,  124:61,  125:62,  127:63,  126:62,  128:63
    },
    "port_lanemap": {
        1:0xf,    3:0xf,    2:0xf0,   4:0xf0,   5:0xf,    7:0xf,    6:0xf0,   8:0xf0,   9:0xf,    10:0xf0,  11:0xf,   12:0xf0,  13:0xf,   14:0xf0,  15:0xf,   16:0xf0,
        17:0xf,   18:0xf0,  19:0xf,   20:0xf0,  21:0xf,   22:0xf0,  23:0xf,   24:0xf0,  25:0xf,   26:0xf0,  27:0xf,   28:0xf0,  29:0xf,   30:0xf0,  31:0xf,   32:0xf0,
        33:0xf,   34:0xf0,  35:0xf,   36:0xf0,  37:0xf,   38:0xf0,  39:0xf,   40:0xf0,  41:0xf,   42:0xf0,  43:0xf,   44:0xf0,  45:0xf,   46:0xf0,  47:0xf,   48:0xf0,
        49:0xf,   50:0xf0,  51:0xf,   52:0xf0,  53:0xf,   54:0xf0,  55:0xf,   56:0xf0,  57:0xf,   59:0xf,   58:0xf0,  60:0xf0,  61:0xf,   63:0xf,   62:0xf0,  64:0xf0,
        65:0xf,   67:0xf,   66:0xf0,  68:0xf0,  69:0xf,   71:0xf,   70:0xf0,  72:0xf0,  73:0xf,   74:0xf0,  75:0xf,   76:0xf0,  77:0xf,   78:0xf0,  79:0xf,   80:0xf0,
        81:0xf,   82:0xf0,  83:0xf,   84:0xf0,  85:0xf,   86:0xf0,  87:0xf,   88:0xf0,  89:0xf,   90:0xf0,  91:0xf,   92:0xf0,  93:0xf,   94:0xf0,  95:0xf,   96:0xf0,
        97:0xf,   98:0xf0,  99:0xf,   100:0xf0, 101:0xf,  102:0xf0, 103:0xf,  104:0xf0, 105:0xf,  106:0xf0, 107:0xf,  108:0xf0, 109:0xf,  110:0xf0, 111:0xf,  112:0xf0,
        113:0xf,  114:0xf0, 115:0xf,  116:0xf0, 117:0xf,  118:0xf0, 119:0xf,  120:0xf0, 121:0xf,  123:0xf,  122:0xf0, 124:0xf0, 125:0xf,  127:0xf,  126:0xf0, 128:0xf0
    },
    "port_rlm_map": {
        1:0,     3:0,     2:0,     4:0,     5:0,     7:0,     6:0,     8:0,     9:1,     10:1,    11:1,    12:1,    13:1,    14:1,    15:1,    16:1,
        17:2,    18:2,    19:2,    20:2,    21:2,    22:2,    23:2,    24:2,    25:3,    26:3,    27:3,    28:3,    29:3,    30:3,    31:3,    32:3,
        33:4,    34:4,    35:4,    36:4,    37:4,    38:4,    39:4,    40:4,    41:5,    42:5,    43:5,    44:5,    45:5,    46:5,    47:5,    48:5,
        49:6,    50:6,    51:6,    52:6,    53:6,    54:6,    55:6,    56:6,    57:7,    59:7,    58:7,    60:7,    61:7,    63:7,    62:7,    64:7,
        65:8,    67:8,    66:8,    68:8,    69:8,    71:8,    70:8,    72:8,    73:9,    74:9,    75:9,    76:9,    77:9,    78:9,    79:9,    80:9,
        81:10,   82:10,   83:10,   84:10,   85:10,   86:10,   87:10,   88:10,   89:11,   90:11,   91:11,   92:11,   93:11,   94:11,   95:11,   96:11,
        97:12,   98:12,   99:12,   100:12,  101:12,  102:12,  103:12,  104:12,  105:13,  106:13,  107:13,  108:13,  109:13,  110:13,  111:13,  112:13,
        113:14,  114:14,  115:14,  116:14,  117:14,  118:14,  119:14,  120:14,  121:15,  123:15,  122:15,  124:15,  125:15,  127:15,  126:15,  128:15
    }
}
CPO_PLATFORM_PORT_CONFIG = {
    "M2-W6940-128X1-FR4": {
        "1-128": {
            "admin": "up",
            "speed": 400000,
            "lane": 4
        }
    },
    "M2-W6940-128X1-FR4-200G": {
        "1-128": {
            "admin": "up",
            "speed": 200000,
            "lane": 4
        }
    },
    "M2-W6940-128X1-FR4-100G": {
        "1-128": {
            "admin": "up",
            "speed": 100000,
            "lane": 4
        }
    }
}
CPO_PLATFORM_MEDIA_SETTINGS = {
    "GLOBAL_MEDIA_SETTINGS": {
        "1-128": {
            "Default": {
                "pre3": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                },
                "pre2": {
                    "lane0": "0x00000001",
                    "lane1": "0x00000001",
                    "lane2": "0x00000001",
                    "lane3": "0x00000001"
                },
                "pre1": {
                    "lane0": "0xfffffff8",
                    "lane1": "0xfffffff8",
                    "lane2": "0xfffffff8",
                    "lane3": "0xfffffff8"
                },
                "main": {
                    "lane0": "0x00000050",
                    "lane1": "0x00000050",
                    "lane2": "0x00000050",
                    "lane3": "0x00000050"
                },
                "post1": {
                    "lane0": "0xfffffffc",
                    "lane1": "0xfffffffc",
                    "lane2": "0xfffffffc",
                    "lane3": "0xfffffffc"
                },
                "post2": {
                    "lane0": "0xffffffff",
                    "lane1": "0xffffffff",
                    "lane2": "0xffffffff",
                    "lane3": "0xffffffff"
                },
                "post3": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                }
            },
            "400000-4lane": {
                "pre3": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                },
                "pre2": {
                    "lane0": "0x00000001",
                    "lane1": "0x00000001",
                    "lane2": "0x00000001",
                    "lane3": "0x00000001"
                },
                "pre1": {
                    "lane0": "0xfffffff8",
                    "lane1": "0xfffffff8",
                    "lane2": "0xfffffff8",
                    "lane3": "0xfffffff8"
                },
                "main": {
                    "lane0": "0x00000050",
                    "lane1": "0x00000050",
                    "lane2": "0x00000050",
                    "lane3": "0x00000050"
                },
                "post1": {
                    "lane0": "0xfffffffc",
                    "lane1": "0xfffffffc",
                    "lane2": "0xfffffffc",
                    "lane3": "0xfffffffc"
                },
                "post2": {
                    "lane0": "0xffffffff",
                    "lane1": "0xffffffff",
                    "lane2": "0xffffffff",
                    "lane3": "0xffffffff"
                },
                "post3": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                }
            },
            "200000-4lane": {
                "pre3": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                },
                "pre2": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                },
                "pre1": {
                    "lane0": "0xfffffffe",
                    "lane1": "0xfffffffe",
                    "lane2": "0xfffffffe",
                    "lane3": "0xfffffffe"
                },
                "main": {
                    "lane0": "0x0000004B",
                    "lane1": "0x0000004B",
                    "lane2": "0x0000004B",
                    "lane3": "0x0000004B"
                },
                "post1": {
                    "lane0": "0xfffffffe",
                    "lane1": "0xfffffffe",
                    "lane2": "0xfffffffe",
                    "lane3": "0xfffffffe"
                },
                "post2": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                },
                "post3": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                }
            },
            "100000-4lane": {
                "pre3": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                },
                "pre2": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                },
                "pre1": {
                    "lane0": "0xfffffffe",
                    "lane1": "0xfffffffe",
                    "lane2": "0xfffffffe",
                    "lane3": "0xfffffffe"
                },
                "main": {
                    "lane0": "0x0000004B",
                    "lane1": "0x0000004B",
                    "lane2": "0x0000004B",
                    "lane3": "0x0000004B"
                },
                "post1": {
                    "lane0": "0xfffffffe",
                    "lane1": "0xfffffffe",
                    "lane2": "0xfffffffe",
                    "lane3": "0xfffffffe"
                },
                "post2": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                },
                "post3": {
                    "lane0": "0x00000000",
                    "lane1": "0x00000000",
                    "lane2": "0x00000000",
                    "lane3": "0x00000000"
                }
            }
        }
    }
}

OE_NUM = CPO_PLATFORM_CONFIG.get("oe_num", 0)
OE_BANK_NUM = CPO_PLATFORM_CONFIG.get("oe_bank_num", 0)
OE_RLM_NUM = CPO_PLATFORM_CONFIG.get("oe_rlm_num", 0)
OE_INDEX_LIST = CPO_PLATFORM_CONFIG.get("oe_index_list", [])
RLM_INDEX_LIST = CPO_PLATFORM_CONFIG.get("rlm_index_list", [])
OE_BUS_MAP = CPO_PLATFORM_CONFIG.get("oe_bus_map", {})
OE_I2C_PATH_FORMAT = CPO_PLATFORM_CONFIG.get("oe_i2c_path_format", "{}")
OE_I2C_PATH_MAP = dict.fromkeys(OE_INDEX_LIST)
PORT_INDEX_LIST = list(CPO_PLATFORM_CONFIG.get("port_bank_map").keys())
PORT_INDEX_LIST.sort()
BANK_LANE_NUM = 8
for key in OE_I2C_PATH_MAP.keys():
    OE_I2C_PATH_MAP[key] = OE_I2C_PATH_FORMAT.format(OE_BUS_MAP[key])

import logging
from logging.handlers import RotatingFileHandler
logging.basicConfig(level=logging.DEBUG)
handler = RotatingFileHandler('/var/log/cpo_daemon_process.log', maxBytes=1024*1024*10, backupCount=5)
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
logging.getLogger('').addHandler(handler)

class helper_logger():
    def __init__(self) -> None:
        pass
    
    @staticmethod
    def log_error(message):
        global debug_level
        if (debug_level <= 5):
            logging.error(message)
    @staticmethod
    def log_warn(message):
        global debug_level
        if (debug_level <= 4):
            logging.warning(message)
    @staticmethod
    def log_notice(message):
        global debug_level
        if (debug_level <= 3):
            logging.info(message)
    @staticmethod
    def log_info(message):
        global debug_level
        if (debug_level <= 2):
            logging.info(message)
    @staticmethod
    def log_debug(message):
        global debug_level
        if (debug_level <= 1):
            logging.debug(message)
    @staticmethod
    def log_trace(message):
        global debug_level
        if (debug_level <= 0):
            logging.debug(message)

def parse_port_range(input):
    result = list()
    if input.find(",") >= 0:
        input_split = input.split(",")
        for item in input_split:
            result.extend(parse_port_range(item))
    elif input.find("-") >= 0:
        input_split = input.split("-")
        port_start = int(input_split[0])
        port_end = int(input_split[-1]) + 1
        result.extend(list(range(port_start, port_end)))
    else:
        result.append(int(input))
    return result

def take_port_number(item):
    # use first number
    if isinstance(item, str):
        return int(re.search("\d+", item).group())
    else:
        return item

class Port_Mapping():
    def __init__(self) -> None:
        self.port_config_dict = dict()
    def rlm_global_index_to_local_index(self, global_index):
        oe_index = self.rlm_index_to_oe_index(global_index)
        rlm_list = self.oe_index_to_rlm_index_list(oe_index)
        return rlm_list.index(global_index)
    def rlm_index_to_oe_index(self, rlm_index):
        for _rlm_index, oe_index in CPO_PLATFORM_CONFIG.get("rlm_oe_map").items():
            if _rlm_index != rlm_index:
                continue
            return oe_index
    def oe_index_to_rlm_index_list(self, oe_index):
        rlm_list = list()
        rlm_oe_map = CPO_PLATFORM_CONFIG.get("rlm_oe_map")
        for rlm_index, _oe_index in rlm_oe_map.items():
            if _oe_index != oe_index:
                continue
            rlm_list.append(rlm_index)
        return list(rlm_list)
    def port_index_to_rlm_index(self, port_index):
        port_rlm_map = CPO_PLATFORM_CONFIG.get("port_rlm_map")
        return port_rlm_map.get(port_index)
    def bank_global_index_to_local_index(self, global_index):
        oe_index = self.bank_index_to_oe_index(global_index)
        bank_list = self.oe_index_to_bank_index_list(oe_index)
        return bank_list.index(global_index)
    def port_index_to_oe_index(self, port_index):
        bank_index = self.port_index_to_bank_index(port_index)
        return self.bank_index_to_oe_index(bank_index)
    def port_index_to_bank_index(self, port_index):
        port_bank_map = CPO_PLATFORM_CONFIG.get("port_bank_map")
        return port_bank_map.get(port_index)
    def oe_index_to_port_index_list(self, oe_index):
        port_list = list()
        bank_list = self.oe_index_to_bank_index_list(oe_index)
        for bank_index in bank_list:
            port_list.extend(self.bank_index_to_port_index_list(bank_index))
        return port_list
    def bank_index_to_port_index_list(self, bank_index):
        port_list = list()
        port_bank_map = CPO_PLATFORM_CONFIG.get("port_bank_map")
        for port_index, _bank_index in port_bank_map.items():
            if _bank_index != bank_index:
                continue
            port_list.append(port_index)
        return list(port_list)
    def oe_index_to_bank_index_list(self, oe_index):
        bank_list = list()
        bank_oe_map = CPO_PLATFORM_CONFIG.get("bank_oe_map")
        for bank_index, _oe_index in bank_oe_map.items():
            if _oe_index != oe_index:
                continue
            bank_list.append(bank_index)
        return list(bank_list)
    def bank_index_to_oe_index(self, bank_index):
        bank_oe_map = CPO_PLATFORM_CONFIG.get("bank_oe_map")
        return bank_oe_map.get(bank_index)
    def port_config_init(self, hw_sku):
        # for factest mode
        port_config = CPO_PLATFORM_PORT_CONFIG.get(hw_sku, None)
        if port_config == None:
            return
        for ports, values in port_config.items():
            port_index_list = parse_port_range(ports)
            for port_index in port_index_list:
                self.set_port(port_index, "Ethernet{}".format(port_index), values.get("speed", None), values.get("lane", None), values.get("admin", None))
    def get_port_lane_list(self, port_index):
        lanemap_dict = CPO_PLATFORM_CONFIG.get("port_lanemap", None)
        if lanemap_dict == None:
            return []
        port_lanemap = lanemap_dict.get(port_index, None)
        if port_lanemap == None:
            return []
        lane_list = list()
        for lane_index in range(8):
            if 1<<lane_index & port_lanemap:
                lane_list.append(lane_index)
        return lane_list
    def get_port_admin(self, port_index):
        try:
            for logical_port_dict in self.port_config_dict.get(port_index).values():
                if logical_port_dict.get("admin") == "up":
                    return True
            return False
        except Exception as e:
            helper_logger.log_error("get_physical_port_media_key error: {}".format(str(e)))
            return False
    def get_port_speed(self, port_index):
        try:
            total_speed = 0
            for logical_port_dict in self.port_config_dict.get(port_index).values():
                total_speed += logical_port_dict["speed"]
            return total_speed
        except Exception as e:
            helper_logger.log_error("get_physical_port_media_key error: {}".format(str(e)))
            return None
    def set_port(self, physical_port, logical_port, speed=None, lane=None, admin=None):
        notify_media_setting = False
        speed = int(speed)
        physical_port_dict = self.port_config_dict.get(physical_port, dict())
        logical_port_dict = physical_port_dict.get(logical_port, dict())
        if speed and logical_port_dict.get("speed", None) != speed:
            # helper_logger.log_trace("port_config update, port_index: {}, speed_change: {} --->>> {}".format(physical_port, logical_port_dict["speed"], speed))
            logical_port_dict["speed"] = speed
            notify_media_setting = True
        if lane and logical_port_dict.get("lane", None) != lane:
            # helper_logger.log_trace("port_config update, port_index: {}, lane_change: {} --->>> {}".format(physical_port, logical_port_dict["lane"], lane))
            logical_port_dict["lane"] = lane
            notify_media_setting = True
        if admin and logical_port_dict.get("admin", None) != admin:
            # helper_logger.log_trace("port_config update, port_index: {}, admin_change: {} --->>> {}".format(physical_port, logical_port_dict["admin"], admin))
            logical_port_dict["admin"] = admin
        physical_port_dict[logical_port] = logical_port_dict
        self.port_config_dict[physical_port] = physical_port_dict
        return notify_media_setting
    def del_port(self, physical_port, logical_port):
        physical_port_dict = self.port_config_dict.get(physical_port, dict())
        try:
            physical_port_dict.pop(logical_port)
        except:
            pass
        self.port_config_dict[physical_port] = physical_port_dict
    def get_logical_to_physical(self, logical_port):
        for physical_port, physical_port_dict in self.port_config_dict.items():
            if logical_port in physical_port_dict.keys():
                return physical_port
        return None
    def get_physical_to_logical(self, physical_port):
        physical_port_dict = self.port_config_dict.get(physical_port, dict())
        logical_port_list = list(physical_port_dict.keys())
        logical_port_list.sort(key=take_port_number)
        return logical_port_list
    def get_physical_port_media_key(self, physical_port):
        try:
            total_speed = 0
            total_lane = 0
            for logical_port_dict in self.port_config_dict.get(physical_port).values():
                total_speed += logical_port_dict["speed"]
                total_lane += logical_port_dict["lane"]
            return "{}-{}lane".format(total_speed, total_lane)
        except Exception as e:
            helper_logger.log_error("physical_port{} get_physical_port_media_key error: {}".format(physical_port, str(e)))
            return None