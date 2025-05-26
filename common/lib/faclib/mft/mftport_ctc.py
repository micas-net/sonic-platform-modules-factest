#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
u'''生测PORT组件'''
import copy
from datetime import datetime
import syslog
import sys
import re
import os
import abc
import time
import traceback
import click
import subprocess
import json
from .mftport_log import PortLogger
from faclib.config.facconfig import TESTCASE
from faclib.config.facconfig import sdk_cmd_lock
if sys.version > '3':
    py3 = True
else:
    import commands
    py3 = False


# 继承端口配置 TESTCASE["mft_port"]
#from faclib.config.facconfig import *
# 调用sdk的接口，里面有锁，用于互斥访问
#from faclib.factool.sdklib import *

# 返回值
RST_SUCCESS = 0
RST_PORT_LIST_ERR = -1
RST_PORT_STATUS_ERR = -2
RST_CHECK_FAILED = -3
RST_CMD_EXEC_FAILED = -100
RST_EXCEPTION= -999

# 如果有外部phy的话，一个阶段一个阶段执行，MAC->SYS->PHY
PRBS_PROC_SERIAL = 0
# MAC、SYS、PHY一起启动后校验
PRBS_PROC_PARALLEL = 1

LANE_MAX = 8

PRBS_BER_DEFAULT = 1.0e-8

# 生测Port相关配置, 从配置文件中获取
MFT_CONFIG_DEFAULT = {
    # xgs_sai_3：xgs3代，sai
    # xgs_sdk_3：xgs3代，裸sdk
    # xgs_sdk_4：xgs4/5代，裸sdk
    # ctc_sai: ctc，sai
    "environment": "ctc_sai",
    "sdk_cmd": "docker exec syncd ctc_shell -e",
    # 通用配置
    # "com_cfg": {
    #     # 连接CPU的口
    #     "kr_port":[],
    #     "prbs_port_list":[],
    #     "prbs_port_list_ext_phy":[],
    # },
    # 通用命令
    "com_cmd": {
        # 端口状态，命令变动涉及结果解析，没法只配置命令
        "ports_status": "show port mac-link",
        "port_status": "show port  mac-link-status",
        "port_en_set_enable": "port {port} port-en enable",
        "port_en_set_disable": "port {port} port-en disable",
        "port_mac_set_enable": "port {port} mac enable",
        "port_mac_set_disable": "port {port} mac disable",
        "show_serdes_info": "show datapath info serdes",
        # 统计，命令变动涉及结果解析，没法只配置命令
        "show_count": "show stats mac-all all",
        "show_count_tx_port": "show stats mac-tx port {port}",
        "show_count_rx_port": "show stats mac-rx port {port}",
        "clear_count": "clear stats mac-all all",
        # vlan
        "vlan_create": "vlan create vlan {vlan_id} default-entry",
        "vlan_remove": "vlan remove vlan {vlan_id} ",
        "vlan_show"  : "show vlan status",
        "vlan_mbr_add" : "vlan add port {port} vlan {vlan_id}",
        "vlan_mbr_del" : "vlan del port {port} vlan {vlan_id}",
        "pvlan_set"  : "port {port} default vlan {vlan_id}",
        "fdb_add"    : "l2 fdb add mac {mac} fid {vlan_id} port {port} static",
    },
    # 端口收发帧测试
    "test_frame": {
        # 测试发送的报文数量
        "pkt_count":10000,
        # 测试发送的报文长度
        "pkt_len": 1024,
        # 测试发送的报文dmac
        "pkt_dmac": "ff:ff:ff:ff:ff:ff",
        # 测试发送的报文smac。端口收到这个报文后不会转发
        "pkt_smac": "00:00:00:00:00:00",
        # 测试使用的vlan id
        "vlan_id": 4048,
        # 测试持续时间
        "duration_time": 5,
        # 测试失败重试次数
        "retry_times":1,
        # 测试环境回退等待时间(s)
        "cleanup_time": 10,
    },
    # 端口广播测试
    "test_brcst": {
        # 测试发送的报文数量
        "pkt_count":10000,
        # 测试发送的报文长度。继承之前hsdk的
        "pkt_len": 64,
        # 测试发送的报文dmac
        "pkt_dmac": "ff:ff:ff:ff:ff:ff",
        # 测试发送的报文smac。
        "pkt_smac": "00:00:00:00:00:01",
        # 测试使用的vlan id
        "vlan_id": 4048,
        # 测试持续时间
        "duration_time": 5,
        # 测试失败重试次数
        "retry_times":1,
        # 测试环境回退等待时间(s)
        "cleanup_time": 10,
    },
    # prbs测试配置
    "test_prbs": {
        # 端口列表，不带外部phy的端口
        "port_list":"1-8,25-32",
        # 端口列表，外部phy的端口
        "port_list_ext_phy":"9-24",
        # 带外部phy时，3侧并行跑还是顺序跑。默认并行跑提升测试效率
        "proc" : PRBS_PROC_PARALLEL,
        # prbs 命令列表
        "test_cmd":{
            "para_dict": {
                # PRBS pattern, 0:PRBS7+, 1:PRBS7-, 2:PRBS15+, 3:PRBS15-, 4:PRBS23+, 5:PRBS23-, 6:PRBS31+, 7:PRBS31-, 8:PRBS9, 9:Squareware(8081), 10:PRBS11, 11:PRBS1T(1010)           
                # 12:PRBS13,13:PRBS9Q, 14:PRBS13Q, 15:PRBS15Q, 16:PRBS31Q, 17:PRBS9QP, 18:PRBS13QP, 19:PRBS15QP, 20:PRBS31QP
                "static_prbs_mac_nrz":"6",
                "static_prbs_mac_pam4":"16",
                "static_prbs_phy":"p31",
            },
            "prbs_start":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} enable\"",
                    "para": ["dyn_serdes","static_prbs_mac_nrz"],
                },
                {
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} check keep\"",
                    "para": ["dyn_serdes","static_prbs_mac_nrz"],
                },
            ],
            "prbs_get":[
                {
                    # 带keep会启动rx check后不关闭
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} check keep\"",
                    "para": ["dyn_serdes","static_prbs_mac_nrz"],
                },
            ],
            "prbs_clear":[
                {
                    # 不带keep 会启动rx check后关闭
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} check delay 1\"",
                    "para": ["dyn_serdes","static_prbs_mac_nrz"],
                },
                {
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} disable\"",
                    "para": ["dyn_serdes","static_prbs_mac_nrz"],
                },
            ],
            "prbs_mac_start":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} enable\"",
                    "para": ["dyn_serdes","static_prbs_mac_pam4"],
                },
                
            ],
            "prbs_mac_pre_check":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} check\"",
                    "para": ["dyn_serdes","static_prbs_mac_pam4"],
                },
            ],
            "prbs_mac_get":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} check keep\"",
                    "para": ["dyn_serdes","static_prbs_mac_pam4"],
                },
            ],
            "prbs_mac_clear":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} check delay 1\"",
                    "para": ["dyn_serdes","static_prbs_mac_pam4"],
                },
                {
                    "cmd": "docker exec syncd ctc_shell -e \"dk\" \"serdes {} prbs {} disable\"",
                    "para": ["dyn_serdes","static_prbs_mac_pam4"],
                },
            ],
            "prbs_sys_start":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"phy user_diag {} prbs_set if_side=sys poly={}\"",
                    "para": ["dyn_port","static_prbs_phy"],
                },
            ],
            "prbs_sys_get":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"phy user_diag {} prbs_get if_side=sys\"",
                    "para": "dyn_port",
                },
            ],
            "prbs_sys_clear":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"phy user_diag {} prbs_clear if_side=sys\"",
                    "para": "dyn_port",
                },
            ],
            "prbs_line_start":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"phy user_diag {} prbs_set if_side=line poly={}\"",
                    "para": ["dyn_port","static_prbs_phy"],
                },
            ],
            "prbs_line_get":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"phy user_diag {} prbs_get if_side=line\"",
                    "para": "dyn_port",
                },
            ],
            "prbs_line_clear":[
                {
                    "cmd": "docker exec syncd ctc_shell -e \"phy user_diag {} prbs_clear if_side=line\"",
                    "para": "dyn_port",
                },
            ],
        },
        # 测试持续时间
        "duration_time": 60,
        # prbs测试允许的误码率
        "prbs_ber": 1.0e-9, 
        # 如果定义了端口的prbs则端口级的覆盖默认值
        # "prbs_ber_dict": {
        #     32:1.0e-1,
        # },
        # # 如果定义了lane级的prbs，则lane级的覆盖port级
        # "prbs_ber_lane_dict": {
        #     31: {
        #         1:1.0e-2,
        #         2:1.0e-3,
        #         5:1.0e-5,
        #     }
        # },
        # 测试失败重试次数
        "retry_times":1,
        # 测试环境回退等待时间(s)
        "cleanup_time": 10,
    },

    # "port_log_level": LOG_ERROR_LEVEL,  # PORT组件log级别
    "flag": False  # 是否已经载入port配置
}


log = PortLogger("MftPort", "debug")


import subprocess

class CommandExecutor:
    last_command = None
    last_status = None
    last_output = None

    @classmethod
    def sys_cmd(cls, cmd):
        """Execute a system command and return the status and output."""
        try:
            cls.last_command = cmd  # 记录最后一次执行的命令
            log.debug(cmd)
            # 应该不需要考虑兼容py2.x了，优化掉
            if py3:
                cls.last_status, cls.last_output = subprocess.getstatusoutput(cmd)
            else:
                cls.last_status, cls.last_output = commands.getstatusoutput(cmd)
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % cmd)
        return cls.last_status, cls.last_output

def sys_cmd(cmd):
    return CommandExecutor.sys_cmd(cmd)

def get_last_cmd():
    return CommandExecutor.last_command

def get_last_cmd_status():
    return CommandExecutor.last_status

def get_last_cmd_output():
    return CommandExecutor.last_output

def sdk_cmd(cmd):
    with sdk_cmd_lock:
        return sys_cmd(cmd)

# 创建线程来测试锁的行为
def sdk_cmd_lock_test():
    print("mft_port: Trying to acquire lock.")
    with sdk_cmd_lock:
        print("mft_port: Lock acquired.")
        time.sleep(2)
    print("mft_port: Lock released.")

class PortUtil():
    u'''Port工具类'''

    @staticmethod
    def get_platform():
        SONIC_CFGGEN_PATH = '/usr/local/bin/sonic-cfggen'
        PLATFORM_KEY = "DEVICE_METADATA['localhost']['platform']"
        try:
            proc = subprocess.Popen([SONIC_CFGGEN_PATH, '-H', '-v', PLATFORM_KEY],
                                    stdout=subprocess.PIPE,
                                    shell=False,
                                    stderr=subprocess.STDOUT)
            stdout = proc.communicate()[0]
            proc.wait()
            platform = stdout.decode().rstrip('\n')
        except OSError as e:
            raise OSError("Cannot detect platform. err:%s" %e)

        return platform

    @staticmethod
    def get_hwsku():
        SONIC_CFGGEN_PATH = '/usr/local/bin/sonic-cfggen'
        HWSKU_KEY = "DEVICE_METADATA['localhost']['hwsku']"
        try:
            proc = subprocess.Popen([SONIC_CFGGEN_PATH, '-d', '-v', HWSKU_KEY],
                                    stdout=subprocess.PIPE,
                                    shell=False,
                                    stderr=subprocess.STDOUT)
            stdout = proc.communicate()[0]
            proc.wait()
            hwsku = stdout.decode().rstrip('\n')
        except OSError as e:
            raise OSError("Cannot detect platform. err:%s" %e)

        return hwsku

    @staticmethod
    def get_port_config_ini_path():
        u'''返回port_config的地址'''

        PLATFORM_ROOT_PATH = '/usr/share/sonic/device'
        platform = PortUtil.get_platform()
        hwsku = PortUtil.get_hwsku()

        # Load platform module from source
        platform_path = "/".join([PLATFORM_ROOT_PATH, platform])
        hwsku_path = "/".join([platform_path, hwsku])

        # First check for the presence of the new 'port_config.ini' file
        port_config_file_path = "/".join([hwsku_path, "port_config.ini"])
        if not os.path.isfile(port_config_file_path):
            # port_config.ini doesn't exist. Try loading the legacy 'portmap.ini' file
            port_config_file_path = "/".join([hwsku_path, "portmap.ini"])

        return port_config_file_path

    @staticmethod
    def get_chip_info_xgs():
        u'''获取 chip'''

        log.debug("获取芯片信息")
        cmd = "lspci | grep Broadcom |  grep -oP 'Device\s+\K\w+'"
        _, output = sys_cmd(cmd)
        if not output:
            cmd = "lspci | grep Broadcom |  grep -oP 'subsidiaries\s+\K\w+'"
            _, output = sys_cmd(cmd)
        chips = output.split('\n')
        if len(chips) == 1 :
            log.info("chip:%s" % chips[0])
        else:
            log.error("chip err:%s" % chips)
        return chips[0]

    @staticmethod
    def get_chip_info_ctc():
        u'''获取 chip'''

        log.debug("获取芯片信息")
        cmd = "lspci | grep 'Communication controller' | grep -oP 'Device [a-zA-Z]{2}[0-9]{2}:\K[0-9a-fA-F]+'"
        _, output = sys_cmd(cmd)
        chips = output.split('\n')
        if len(chips) == 1 :
            log.info("chip:%s" % chips[0])
        else:
            log.error("chip err:%s" % chips)
        return chips[0]
    @staticmethod
    def convert_to_list(input_str):
        u'''"1-64,65,66" 转成list'''

        if not input_str:
            return []

        numbers_list = []
        
        # 以逗号分割字符串，得到各个部分
        parts = input_str.split(',')
        
        for part in parts:
            if '-' in part:
                # 如果部分中包含'-'，则将其视为范围
                start, end = map(int, part.split('-'))
                numbers_list.extend(range(start, end + 1))
            else:
                # 否则将其视为单个数字
                numbers_list.append(int(part))
        return numbers_list

def singleton(cls):
    u'''单例'''

    _instance = {}

    def _singleton(*args, **kargs):
        if cls not in _instance:
            _instance[cls] = cls(*args, **kargs)
        return _instance[cls]

    return _singleton

# 端口面板顺序和逻辑口等相关的映射
class PortClassCTC():
    u'''PortHsdkClass类主要用于存储端口在不同层级的别名及相关信息'''

    def __init__(self, **kwargs):

        # 面板口的索引从1开始
        self._idx = kwargs.get("fport_idx", "")
        # port_config.ini 中的name
        self._interface = kwargs.get("interface", "")
        self._lanes     = kwargs.get("lanes", "")
        self._gport     = kwargs.get("gport", "")
        self._prbs_ber  = kwargs.get("prbs_ber", PRBS_BER_DEFAULT)

    @property
    def interface(self):
        u'''返回interface'''

        return self._interface

    @property
    def idx(self):
        u'''返回lport'''

        return self._idx

    @property
    def lanes(self):
        u'''返回lport'''

        return self._lanes


    @property
    def gport(self):
        u'''返回lport'''

        return self._gport

    def set_gport(self, gport):
        u'''设置gport'''

        self._gport = gport

    def set_prbs_ber(self, prbs_ber):
        u'''设置prbs_ber'''

        self._prbs_ber = prbs_ber


# 端口测试相关的配置
class PortTestCTC():
    u'''PortTest父类'''


    mft_config_default = MFT_CONFIG_DEFAULT

    def __init__(self):
        self.device_port_list = []
        self.device_config = {
            "platform": None,
            "chip":None,
            # mft_config 是否初始化
            "mft_cfg_init": False,
        }
        self.mft_config = {
            #"device": "xgs_ctc",
        }
        self.base_config_init()
    
    def base_config_init(self):
        # log.debug("base_config_init")
        self.get_factest_port_config()
        self.cmd_init()
        if self.device_config['platform'] is None:
            self.device_config['platform'] = PortUtil.get_platform()
        if self.device_config['chip'] is None:
            self.device_config['chip'] = PortUtil.get_chip_info_ctc()
        if len(self.device_port_list) == 0:
            self.__parse_port_config_ini()
            self.__get_prbs_ber()
            self.__init_gport()

    def cmd_init(self):
        com_cmd = self.mft_config.get("com_cmd", {})
        default_cmd = self.mft_config_default["com_cmd"]
        # 端口状态
        self.cmd_ports_status = com_cmd.get("ports_status", default_cmd["ports_status"])
        self.cmd_port_status = com_cmd.get("port_status", default_cmd["port_status"])
        self.cmd_port_en_set_enable = com_cmd.get("port_en_set_enable", default_cmd["port_en_set_enable"])
        self.cmd_port_en_set_disable = com_cmd.get("port_en_set_disable", default_cmd["port_en_set_disable"])
        self.cmd_port_mac_set_enable = com_cmd.get("port_mac_set_enable", default_cmd["port_mac_set_enable"])
        self.cmd_port_mac_set_disable = com_cmd.get("port_mac_set_disable", default_cmd["port_mac_set_disable"])
        self.cmd_show_serdes_info = com_cmd.get("show_serdes_info", default_cmd["show_serdes_info"])
        
        # 统计
        self.cmd_show_count = com_cmd.get("show_count", default_cmd["show_count"])
        self.cmd_show_count_tx_port = com_cmd.get("show_count_tx_port", default_cmd["show_count_tx_port"])
        self.cmd_show_count_rx_port = com_cmd.get("show_count_rx_port", default_cmd["show_count_rx_port"])
        self.cmd_clear_count = com_cmd.get("clear_count", default_cmd["clear_count"])
        # vlan
        self.cmd_vlan_create = com_cmd.get("vlan_create", default_cmd["vlan_create"])
        self.cmd_vlan_remove = com_cmd.get("vlan_remove", default_cmd["vlan_remove"])
        self.cmd_vlan_show = com_cmd.get("vlan_show", default_cmd["vlan_show"])
        self.cmd_vlan_mbr_add = com_cmd.get("vlan_mbr_add", default_cmd["vlan_mbr_add"])
        self.cmd_vlan_mbr_del = com_cmd.get("vlan_mbr_del", default_cmd["vlan_mbr_del"])
        self.cmd_pvlan_set = com_cmd.get("pvlan_set", default_cmd["pvlan_set"])
        self.cmd_fdb_add = com_cmd.get("fdb_add", default_cmd["fdb_add"])

    def get_factest_port_config(self):
        if not self.device_config["mft_cfg_init"]:
            # log.debug("TESTCASE before init: %s" % TESTCASE)
            if TESTCASE:
                self.mft_config = copy.deepcopy(TESTCASE.get("mft_port", {}))
                # log.debug("mft_config after init: %s" % self.mft_config)

            # 配置校验
            if not self.mft_config:
                log.error("mft_port get failed!")
                return False

            # sdk命令标识
            environment = self.mft_config.get("environment")
            if not self.mft_config.get("sdk_cmd") and environment:
                # 从环境自动识别
                sdk_cmd_map = {
                    "ctc_sai": "docker exec syncd ctc_shell -e",
                    "xgs_sdk_4": "bcmcmdb",
                    "xgs_sdk_3": "bcmcmdb",
                    "xgs_sai": "bcmcmd",
                }
                self.mft_config["sdk_cmd"] = sdk_cmd_map.get(environment)

                if self.mft_config["sdk_cmd"] is None:
                    log.error("unknown environment %s!" % environment)
                    return False
            elif not environment:
                log.error("environment null!")
                return False

            self.device_config["mft_cfg_init"] = True
        else:
            log.debug("mft_config: %s" % self.mft_config)

        log.debug("sdk_cmd: %s" % self.mft_config.get("sdk_cmd"))
        return True

    def __parse_port_config_ini(self):
        u'''解析port_config.ini, 获取interface, lanes'''

        log.debug("解析port_config")
        with open(PortUtil.get_port_config_ini_path(), "r") as port_ini_file:
            lines = port_ini_file.readlines()
            # 找到表头行并获取 name 和 lanes 的列索引
            for idx, line in enumerate(lines):
                line = line.strip()
                if line.startswith("#"):
                    line = line[1:].strip()  # 去除行首的#号和多余空格
                header = line.strip().split()
                if 'name' in header and 'lanes' in header:
                    name_idx = header.index('name')
                    lanes_idx = header.index('lanes')
                    data_start_idx = idx + 1  # 数据行从表头的下一行开始
                    break

            fport_idx = 1
            for line in lines[data_start_idx:]:
                line = line.strip()
                # 这边假定了ini文件中的端口顺序是面板口顺序
                if line.startswith("Ethernet"):
                    # 解析行数据
                    parts = line.split()
                    #print("name_idx:%d, lanes_idx:%d, parts:%s" % (name_idx, lanes_idx, parts))
                    if len(parts) > max(name_idx, lanes_idx):
                        # 提取name和lanes，根据之前计算的索引来获取相应列的数据
                        name = parts[name_idx]
                        lanes_str = parts[lanes_idx]
                        
                        # 将lanes字符串转换为整数数组
                        lanes = [int(lane) for lane in lanes_str.split(',')]
                        
                        # 将解析结果添加到列表
                        # self.device_port_list.append(PortClassCTC(fport_idx=fport_idx,
                        #                                    interface=name,
                        #                                    lanes=lanes))
                        self.device_port_list.append({
                            "idx":fport_idx,
                            "interface":name,
                            "lanes":lanes})
                        fport_idx = fport_idx + 1

    def __get_prbs_ber(self):
        u'''获取单个端口误码率'''

        log.debug("获取prbs_ber")
        for port_obj in self.device_port_list:
            if self.mft_config.get("prbs_ber_dict", None):
                prbs_ber = self.mft_config["prbs_ber_dict"].get(port_obj["idx"], PRBS_BER_DEFAULT)
            else:
                prbs_ber = self.mft_config.get("prbs_ber", PRBS_BER_DEFAULT)
            port_obj["prbs_ber"] = prbs_ber

    def get_sdk_cmd(self, cmd, time_out=0, grep=""):

        # 如果 cmd 是列表，给每个元素加上双引号并用空格连接
        if isinstance(cmd, list):
            cmd_str = " ".join([f'"{c}"' for c in cmd])
        else:
            cmd_str = f'"{str(cmd)}"'
        t = int(time_out)
        if t != 0:
            cmd_str = self.mft_config["sdk_cmd"] + " -t %d "+ cmd_str
        else:
            cmd_str = self.mft_config["sdk_cmd"] + " " + cmd_str
        if grep != "":
            cmd_str += " | grep \"%s\"" % str(grep)
        return cmd_str 

    def update_port_status(self, **kwargs):
        u'''更新端口指定字段状态'''
        cmd = self.get_sdk_cmd(self.cmd_ports_status)
        ret, output = sdk_cmd(cmd)
        try:
            if ret == 0:
                # 按行分割输入文本
                lines = output.strip().splitlines()
                # 找到包含 "GPort" 的行，确定列的索引
                for idx, line in enumerate(lines):
                    if "GPort" in line:
                        header = line.strip().split()
                        gport_idx = header.index('GPort')
                        if kwargs.get("MAC", None):
                            mac_idx = header.index('MAC')
                            for port_obj in self.device_port_list:
                                port_obj["MAC"] = "NA"
                        if kwargs.get("Link", None):
                            link_idx = header.index('Link')
                            for port_obj in self.device_port_list:
                                port_obj["Link"] = "NA"
                        if kwargs.get("MAC-EN", None):
                            macen_idx = header.index('MAC-EN')
                            for port_obj in self.device_port_list:
                                port_obj["MAC-EN"] = "NA"
                        if kwargs.get("Speed", None):
                            speed_idx = header.index('Speed')
                            for port_obj in self.device_port_list:
                                port_obj["Speed"] = "NA"
                        data_start_idx = idx + 2  # 数据行从 GPort 行的两行之后开始
                        break
                # 遍历数据行
                for line in lines[data_start_idx:]:
                    line = line.strip()
                    ports = re.findall(r'^(0x[0-9a-fA-F]+)', line)
                    if ports:
                        parts = line.split()
                        # 提取指定字段
                        gport = parts[gport_idx].lower()
                        if kwargs.get("MAC", None):
                            self.gport_dict[gport]["MAC"] = parts[mac_idx]
                        if kwargs.get("Link", None):
                            self.gport_dict[gport]["Link"] = parts[link_idx]
                        if kwargs.get("MAC-EN", None):
                            self.gport_dict[gport]["MAC-EN"] = parts[macen_idx]
                        if kwargs.get("Speed", None):
                            self.gport_dict[gport]["Speed"] = parts[speed_idx]
            else :
                log.error("update_port_status Failed")
                log.error("cmd:%s" % get_last_cmd())
                log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
        

    def get_port_test_info(self, **kwargs):
        u'''更新端口测试用的信息'''
        ret_t = 0
        self.upports = []
        self.updownerrorports = []
        self.port_info_dict = {
                        # port : {
                        #     "port_info": port_info,
                        #     "status": "",
                        #     "log": ""
                        # }
                    }
        other_info = ""
        port_list = kwargs.get("port_list", self.port_list)
        if len(port_list) == 0:
            log_info = "port_list null \n"
            log.error(log_info)
            return RST_PORT_LIST_ERR, log_info
        log.info("开始获取端口状态")
        try:
            # 获取端口状态
            self.update_port_status(Link=True)
            if get_last_cmd_status() != 0:
                log.error("get_last_cmd_status:%d" % get_last_cmd_status())
                ret_t = RST_PORT_STATUS_ERR
            # 针对port_list初始化result_dict
            for port in port_list:
                # 获取port_info，加入result_dict["port_info_dict"][port]["port_info"]
                port_info = "port:%-3d %s" % (port, self.device_port_list[port - 1]["gport"])
                self.port_info_dict[port] = {
                    "port_info": port_info,
                    "status": "",
                    "log": ""}
                port_status = self.device_port_list[port - 1].get("Link", "NA")
                if port_status == "up":
                    self.upports.append(port)
                    self.port_info_dict[port]["status"] = "up"
                    log.debug("%-18s:up" % port_info)
                elif port_status == "down":
                    self.updownerrorports.append(port)
                    self.port_info_dict[port]["status"] = "down"
                    log.warning("%-18s:down" % port_info)
                else:
                    self.updownerrorports.append(port)
                    self.port_info_dict[port]["status"] = "NA"
                    log.warning("%-18s:NA, output:%s" % (get_last_cmd_output()))

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_t = RST_EXCEPTION
            return ret_t, other_info
        # print("updownerr:%s" % self.updownerrorports)
        return ret_t, other_info

    def __init_gport(self):
        u'''
        # 获取面板口对应的gport
        命令输出样例：
        CTC_CLI(ctc-sdk)# show port mac-link
        ----------------------------------------------------------------------------------------------------
        GPort   LPort MAC   Link   MAC-EN   Speed  Fec     Duplex  Auto-Neg  Interface     Rx-Rate   Tx-Rate
        ----------------------------------------------------------------------------------------------------
        0x0000  0     0     up     TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x0004  4     16    up     TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x0008  8     32    up     TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x000C  12    36    up     TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x0010  16    40    down   TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x0014  20    56    down   TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x0018  24    72    down   TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x001C  28    76    down   TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x0020  32    80    down   TRUE     100G   Rs544   FD      FALSE     CR2            0 bps     0 bps   
        0x0022  34    82    down   TRUE     100G   Rs544   FD      FALSE     CR2            0 bps     0 bps   
        0x0024  36    84    down   TRUE     100G   Rs544   FD      FALSE     CR2            0 bps     0 bps   
        0x0026  38    86    down   TRUE     100G   Rs544   FD      FALSE     CR2            0 bps     0 bps   
        0x0028  40    120   down   TRUE     100G   Rs544   FD      FALSE     CR2            0 bps     0 bps   
        0x002A  42    122   down   TRUE     100G   Rs544   FD      FALSE     CR2            0 bps     0 bps   
        0x002C  44    124   down   TRUE     100G   Rs544   FD      FALSE     CR2            0 bps     0 bps   
        0x002E  46    126   down   TRUE     100G   Rs544   FD      FALSE     CR2            0 bps     0 bps   
        0x0030  48    160   down   TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x0034  52    176   down   TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        0x0038  56    192   down   TRUE     100G   None    FD      FALSE     CR4            0 bps     0 bps   
        '''

        try:
            cmd = self.get_sdk_cmd(self.cmd_ports_status)
            ret, output = sdk_cmd(cmd)
            if ret == 0:
                # 按行分割输入文本
                lines = output.strip().splitlines()
                # 找到包含 "GPort" 的行，确定列的索引
                for idx, line in enumerate(lines):
                    if "GPort" in line:
                        header = line.strip().split()
                        gport_idx = header.index('GPort')
                        mac_idx = header.index('MAC')
                        data_start_idx = idx + 2  # 数据行从 GPort 行的两行之后开始
                        break
                # 遍历数据行
                for line in lines[data_start_idx:]:
                    line = line.strip()
                    ports = re.findall(r'^(0x[0-9a-fA-F]+)', line)
                    if ports:
                        parts = line.split()
                        # 提取LPORT和MAC
                        #gport = int(parts[gport_idx], 16)
                        mac = int(parts[mac_idx])
                        for port_obj in self.device_port_list:
                            if port_obj["lanes"][0] == mac:
                                port_obj["gport"] = parts[gport_idx].lower()
                                log.debug("gport[%d]=%s%s" % (port_obj["idx"], port_obj["gport"], port_obj["lanes"]))
                self.gport_dict = {port["gport"]: port for port in self.device_port_list}
            else :
                log.error("ctc gport init Failed")
                log.error("ret:%d, cmd:%s" % (ret, get_last_cmd()))
                log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))

    def show_config(self):
        formatted_dict = json.dumps(self.device_config, indent=4, sort_keys=True)
        print("device_config:%s" % formatted_dict)
        formatted_dict = json.dumps(self.mft_config, indent=4, sort_keys=True)
        print("mft_config:%s" % formatted_dict)
        formatted_dict = json.dumps(self.device_port_list, indent=4, sort_keys=True)
        print("device_port_list:%s" % formatted_dict)
        # formatted_dict = json.dumps(self.gport_dict, indent=4, sort_keys=True)
        # print("gport_dict:%s" % formatted_dict)
        # print(self.mft_config)
        # for port_obj in self.device_port_list:
        #     print("%2d:%s%s" % (port_obj["idx"], port_obj["gport"], port_obj["lanes"]))

    def get_port_status(self, port, update=True):
        u'''获取端口up/down情况, 返回up/down/!ena'''

        # 一起更新所有端口状态
        if update:
            self.update_port_status(Link=True)
        return True, self.device_port_list[port-1]["Link"]

    def get_port_status_f(self, **kwargs):
        ret_t = 0
        upports = []
        updownerrorports = []
        result_dict = {"port_info_dict": {}, 
                       "other_info": "", 
                       "test_result": False,
                       "updownerrorports": [], 
                       "errorports": [], 
                       "successports": []}
        port_list = kwargs.get("port_list", [])
        log.info("开始获取端口状态")
        try:
            # 传入port_list=[] 或 不传入port_list, 测试全部端口
            # 传入port_list=[7,8,9,10], 测试面板口7, 8, 9, 10
            if len(port_list) == 0:
                for i in range(len(self.device_port_list)):
                    port_list.append(i + 1)
            if isinstance(port_list[0], int):
                log.info("port_list %s" % port_list)
            else:
                other_info = result_dict.get("other_info", "")
                result_dict["other_info"] = other_info + "not find port in port_list:%s\n" % port_list
                log.error("not find port in port_list:%s " % port_list)
                ret_t -= 1
                port_list = []

            # 获取端口状态
            self.update_port_status(Link=True)
            # 针对port_list初始化result_dict
            for port in port_list:
                # 获取port_info，加入result_dict["port_info_dict"][port]["port_info"]
                port_info = "port:%-3d %s" % (port, self.device_port_list[port - 1]["gport"])
                result_dict["port_info_dict"][port] = {
                    "port_info": port_info,
                    "status": "",
                    "log": ""}
                port_status = self.device_port_list[port - 1].get("Link", "NA")
                if port_status == "up":
                    upports.append(port)
                    result_dict["port_info_dict"][port]["status"] = "up"
                    log.debug("%-18s:up" % port_info)
                elif port_status == "down":
                    updownerrorports.append(port)
                    result_dict["port_info_dict"][port]["status"] = "down"
                    log.warning("%-18s:down" % port_info)
                else:
                    updownerrorports.append(port)
                    result_dict["port_info_dict"][port]["status"] = "NA"
                    log.warning("%-18s:NA, output:%s" % (get_last_cmd_output()))

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_t = RST_EXCEPTION
            return ret_t, result_dict, upports, updownerrorports
        return ret_t, result_dict, upports, updownerrorports

    def compare_start_end_ports(self, **kwargs):
        u'''对比开始和结束测试项时的端口up情况'''

        last_upports = copy.deepcopy(self.upports)
        log_info = ""

        try:
            ret_t, _ = self.get_port_test_info()
            log.debug("up_port: %s" % self.upports)
            if ret_t == 0:
                if last_upports != self.upports:
                    ret_t -= 1
                    log_info = "first_uplist != second_uplist\nfirst_uplist:%s\nsecond_uplist:%s\n" % (
                            last_upports, self.upports)
                    log.error(log_info)
                else:
                    log.debug("两次端口up状态相同")
            else:
                ret_t -= 1
                log_info = "test end:get_port_status abnormal"
                log.warning(log_info)
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_t = RST_EXCEPTION
            return ret_t, log_info
        return ret_t, log_info

    def clear_port_packets(self):
        u'''清除SDK 统计的报文'''

        # 清除报文统计数据
        cmd = self.get_sdk_cmd(self.cmd_clear_count)
        ret, output = sdk_cmd(cmd)
        if ret == 0:
            log.debug("clear_port_packets success")

        return ret, output

    def update_port_packets(self):
        u'''更新端口报文统计'''
        cmd = self.get_sdk_cmd(self.cmd_show_count)
        ret, output = sdk_cmd(cmd)
        try:
            if ret == 0:
                for gport in self.gport_dict.values():
                    gport["RxCnt"]  = 0
                    gport["RxByte"] = 0
                    gport["TxCnt"]  = 0
                    gport["TxByte"] = 0
                data_start_idx = 0
                # 按行分割输入文本
                lines = output.strip().splitlines()
                # 找到包含 "Gport" 的行，确定列的索引
                for idx, line in enumerate(lines):
                    if "Gport" in line:
                        header = line.strip().split()
                        gport_idx = header.index('Gport')
                        rx_cnt_idx = header.index('RxCnt')
                        rx_byte_idx = header.index('RxByte')
                        tx_cnt_idx = header.index('TxCnt')
                        tx_byte_idx = header.index('TxByte')
                        data_start_idx = idx + 2  # 数据行从 GPort 行的两行之后开始
                        break
                # 遍历数据行
                for line in lines[data_start_idx:]:
                    line = line.strip()
                    ports = re.findall(r'^(0x[0-9a-fA-F]+)', line)
                    if ports:
                        parts = line.split()
                        # 提取指定字段
                        gport = parts[gport_idx].lower()
                        self.gport_dict[gport]["RxCnt"]  = int(parts[rx_cnt_idx])
                        self.gport_dict[gport]["RxByte"] = int(parts[rx_byte_idx])
                        self.gport_dict[gport]["TxCnt"]  = int(parts[tx_cnt_idx])
                        self.gport_dict[gport]["TxByte"] = int(parts[tx_byte_idx])
            else :
                log.error("update_port_packets Failed")
                log.error("ret:%d, cmd:%s" % (ret, get_last_cmd()))
                log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
        return ret, output

    def check_ports_packets(self, port_list, count, len, direc="tx"):
        self.successports = []
        self.errorports = []
        ret_t, log_info = RST_SUCCESS, ""

        self.update_port_packets()
        if direc == "tx" :
            cnt_key = "TxCnt"
            byte_key = "TxByte"
        elif direc == "rx" :
            cnt_key = "RxCnt"
            byte_key = "RxByte"
        
        expect_byte = (len+4)*count
        for port in port_list:
            port_cnt = self.device_port_list[port - 1][cnt_key]
            port_byte = self.device_port_list[port - 1][byte_key]
            port_info = "port:%d %s" % (port, self.device_port_list[port - 1]["gport"])
            
            if port_cnt == count and port_byte == expect_byte:
                self.successports.append(port)
                log.debug("%s: test success" % (port_info))
            else:
                ret_t -= 1
                self.errorports.append(port)
                log_info = "check ports packets failed! cnt:%d-%d,byte:%d-%d" % (port_cnt, count, port_byte, expect_byte)
                self.port_info_dict[port]["log"] += log_info
                log.warning("%-18s: %s" % (port_info, log_info))
        if ret_t == RST_SUCCESS:
            log_info = "check ports packets succeed"
        else:
            log_info = "check ports packets failed!"
            log_info += get_last_cmd_output()
        return ret_t, log_info

    def vlan_config(self, vlan):
        u'''测试vlan配置'''

        cmd = self.get_sdk_cmd(self.cmd_vlan_create.format(vlan_id=vlan))
        ret, output = sdk_cmd(cmd)
        if ret != RST_SUCCESS:
            log_info = "vlan_config vlan failed. output:%s" % output
            log.error(log_info)
            return ret, log_info
        # 遍历所有端口配置vlan
        for i in range(len(self.device_port_list)):
            gport = self.device_port_list[i]["gport"]
            cmd = self.get_sdk_cmd(self.cmd_vlan_mbr_add.format(vlan_id=vlan, port=gport))
            ret, output = sdk_cmd(cmd)
            if ret != RST_SUCCESS:
                log_info = "vlan %d add member %d fail, output:%s" % (vlan, i+1, output)
                log.error(log_info)
                return ret, log_info
            
            # 配置pvlan
            cmd = self.get_sdk_cmd(self.cmd_pvlan_set.format(vlan_id=vlan, port=gport))
            ret, output = sdk_cmd(cmd)
            if ret != RST_SUCCESS:
                log_info = "port %d pvlan %d set fail, output:%s" %(i+1, vlan, output)
                log.error(log_info)
                return ret, log_info

        log.debug("vlan_config success")
        return ret, "vlan_config success"

    def vlan_config_remove(self, vlan):
        u'''端口测试vlan环境恢复'''

        ret_v, ret_info = RST_SUCCESS, ""

        # 删除vlan
        cmd = self.get_sdk_cmd(self.cmd_vlan_remove.format(vlan_id=vlan))
        ret, output = sdk_cmd(cmd)
        if ret != 0:
            log_info = "remove vlan %d failed. output:%s" % (vlan, output)
            log.warning(log_info)
            ret_info += log_info
            ret_v = ret
            # 恢复环境不能直接返回
            # return ret_v, ret_info

        # 遍历所有端口配置pvid
        for i in range(len(self.device_port_list)):
            # 配置pvlan
            cmd = self.get_sdk_cmd(self.cmd_pvlan_set.format(vlan_id=1, port=self.device_port_list[i]["gport"]))
            ret, output = sdk_cmd(cmd)
            if ret != 0:
                log_info = "port %d pvlan %d set fail, output:%s" %(i+1, 1, output)
                log.warning(log_info)
                ret_info += log_info
                ret_v = ret
                # return ret_v, ret_info

        if ret_v:
            ret_info = "vlan_config_clear success"
            log.debug(ret_info)
        return ret_v, ret_info


    def start_send_port_packets(self, port, count, len, dst_mac, src_mac = "00:00:00:00:00:00"):
        u'''调用SDK指令进行发包'''

        # 使用tx指令发包
        cmd = self.get_sdk_cmd("tx %d VLantag=4048 PortBitMap=%s Length=%d DestMac=%s SourceMac=%s"
                          % (count, port, len, dst_mac, src_mac))
        return sdk_cmd(cmd)

    def get_port_counter(self, port, count):
        u'''获取端口统计'''
        # count支持列表
        rx_cnt_key_list = {
            "rx_guc" : "good unicast",
            "rx_gmc" : "good multicast",
            "rx_gbc" : "good broadcast",
            "rx_fcs" : "fcs error"
        }
        tx_cnt_key_list = {
            "tx_guc" : "good unicast",
            "tx_gmc" : "good multicast",
            "tx_gbc" : "good broadcast",
            "tx_fcs" : "fcs error"
        }
        # 定义用于存放最终结果的字典
        result = {}

        # 如果 count 是字符串，将其转换为列表，方便处理
        if isinstance(count, str):
            count = [count]

        # 遍历 count 列表
        tx_result_disk = {
            "valied": False,
            "pkt" : 0,
            "byte" : 0,
            "output": ""
        }
        rx_result_disk = {
            "valied": False,
            "pkt" : 0,
            "byte" : 0,
            "output": ""
        }
        for key in count:
            if key in rx_cnt_key_list:
                # 调用 RX 统计的获取函数，并获取统计值
                rx_result_disk = self.get_port_counter_rx(port, rx_cnt_key_list[key], rx_result_disk["output"])
                result[key] = {
                    "valied": rx_result_disk["valied"],
                    "pkt"   : rx_result_disk["pkt"],
                    "byte"  : rx_result_disk["byte"],
                }
            elif key in tx_cnt_key_list:
                # 调用 TX 统计的获取函数，并获取统计值
                tx_result_disk = self.get_port_counter_tx(port, tx_cnt_key_list[key], tx_result_disk["output"])
                result[key] = {
                    "valied": tx_result_disk["valied"],
                    "pkt"   : tx_result_disk["pkt"],
                    "byte"  : tx_result_disk["byte"],
                }
            else:
                # 如果 count 中的 key 不在 RX 或 TX 字典中，返回一个默认值
                result[key] = {
                    "valied": False,
                    "pkt" : 0,
                    "byte" : 0,
                }

        return result

    def get_port_counter_tx(self, port, count, output=""):
        u'''获取rx 统计"'''

        # 查看报文收发统计数据
        result_disk = {
            "valied": False,
            "pkt" : 0,
            "byte" : 0,
            "output": ""
        }
        gport = self.device_port_list[port - 1]["gport"]
        log.debug("port[%d]:%s direc:tx" % (port, gport))

        if output == "":
            cmd = self.get_sdk_cmd(self.cmd_show_count_tx_port.format(port=gport))
            ret, output = sdk_cmd(cmd)
            if ret != RST_SUCCESS:
                log.error("get port %s tx count Failed" % gport)
                log.error("ret:%d, cmd:%s" % (ret, get_last_cmd()))
                log.error("output:%s" % get_last_cmd_output())
                return result_disk
        try:
            pattern = r"%s\s+(\d+)\s+(\d+)" % (count)
            match = re.search(pattern, output)
            if match:
                pkt_cnt = match.group(1)
                byte_cnt = match.group(2)
                result_disk["pkt"] = int(pkt_cnt)
                result_disk["byte"] = int(byte_cnt)
                result_disk["valied"] = True
                result_disk["output"] = output
            else:
                log.error("get port %s tx count Failed" % gport)
                log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            return result_disk
        return result_disk

    def get_port_counter_rx(self, port, count, output=""):
        u'''获取rx 统计"'''

        # 查看报文收发统计数据
        result_disk = {
            "valied": False,
            "pkt" : 0,
            "byte" : 0,
            "output": ""
        }
        gport = self.device_port_list[port - 1]["gport"]
        log.debug("port[%d]:%s direc:rx" % (port, gport))

        if output == "":
            cmd = self.get_sdk_cmd(self.cmd_show_count_rx_port.format(port=gport))
            ret, output = sdk_cmd(cmd)
            if ret != RST_SUCCESS:
                log.error("get port %s rx count Failed" % gport)
                log.error("ret:%d, cmd:%s" % (ret, get_last_cmd()))
                log.error("output:%s" % get_last_cmd_output())
                return result_disk
        try:
            pattern = r"%s\s+(\d+)\s+(\d+)" % (count)
            match = re.search(pattern, output)
            if match:
                pkt_cnt = match.group(1)
                byte_cnt = match.group(2)
                result_disk["pkt"] = int(pkt_cnt)
                result_disk["byte"] = int(byte_cnt)
                result_disk["valied"] = True
                result_disk["output"] = output
            else:
                log.error("get port %s rx count Failed" % gport)
                log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            return result_disk
        return result_disk

    @abc.abstractmethod
    def test_setup(self):
        u'''设置测试环境，准备所需的数据，下发对应的测试配置。抽象方法,待继承'''

        return None

    @abc.abstractmethod
    def test_start(self, **kwargs):
        u'''启动测试，例如发包。抽象方法,待继承'''

        return None

    @abc.abstractmethod
    def test_verify(self, **kwargs):
        u'''测试结果校验。抽象方法,待继承'''

        return None

    @abc.abstractmethod
    def test_cleanup(self):
        u'''清理测试环境。抽象方法,待继承'''

        return None

    @abc.abstractmethod
    def test_run(self):
        u'''执行测试，返回测试结果。抽象方法,待继承'''

        return {}

@singleton
class PortFrameTestCTC(PortTestCTC):
    u'''
    端口收发帧
    vlan create vlan 10 default-entry
    vlan add port 0x0 vlan 10
    clear stats mac-all all
    tx 10000 VLantag=4048 PortBitMap=1 Length=64 DestMac=ff:ff:ff:ff:ff:ff
    packet tx mode 0 dest-gport 0x0 oper-type 4 pkt-sample ucast pkt-len 1024  count 10000
    show stats mac-all all

        "vlan_create": "vlan create vlan {vlan_id} default-entry",
        "vlan_remove": "vlan remove vlan {vlan_id} ",
        "vlan_show"  : "show vlan status",
        "vlan_mbr_add" : "vlan add port {port} vlan {vlan_id}",
        "vlan_mbr_del" : "vlan del port {port} vlan {vlan_id}",
        "pvlan_set"  : "port {port} default vlan {vlan_id}",
        "fdb_add"    : "l2 fdb add mac {mac} fid {vlan_id} port {port} static",
    '''
    def __init__(self):
        self.successports = []
        self.errorports = []
        self.updownerrorports = []

        super().__init__()
        # self.base_config_init()
        self.cfg = self.mft_config["test_frame"]
        self.cfg_default = self.mft_config_default["test_frame"]
        self.vlan_id = self.cfg.get("vlan_id", self.cfg_default["vlan_id"])
        self.pkt_count = self.cfg.get("pkt_count", self.cfg_default["pkt_count"])
        self.pkt_len = self.cfg.get("pkt_len", self.cfg_default["pkt_len"]) 
        self.pkt_smac = self.cfg.get("pkt_smac", self.cfg_default["pkt_smac"])
        self.pkt_dmac = self.cfg.get("pkt_dmac", self.cfg_default["pkt_dmac"])
        self.retry_times = self.cfg.get("retry_times", self.cfg_default["retry_times"])
        self.cleanup_time = self.cfg.get("cleanup_time", self.cfg_default["cleanup_time"])

    def test_setup(self):
        u'''端口收发帧测试初始化'''

        log.debug("test_setup")

        ret, output = self.vlan_config(self.vlan_id)
        if ret != RST_SUCCESS:
            log_info = "vlan_config fail, output:%s" % output
            log.error(log_info)
            return ret, log_info

        cmd = self.get_sdk_cmd("l2 fdb vlan-default-entry fid {vlan_id} learn-dis".format(vlan_id=self.vlan_id))
        ret, output = sdk_cmd(cmd)
        if ret != RST_SUCCESS:
            log_info = "vlan set learn-dis failed. output:%s" % output
            log.error(log_info)
            return ret, log_info

        ret, output = self.clear_port_packets()
        if ret != RST_SUCCESS:
            log_info = "clear_port_packets fail, output:%s" % output
            log.error(log_info)
            return ret, log_info

        cmd = self.get_sdk_cmd("show stats mac-all all")
        ret, output = sdk_cmd(cmd)
        log.debug("stats:%s" % output)

        log_info = "init_test success"
        log.debug(log_info)
        return ret, log_info

    def test_start(self, **kwargs):
        u'''端口收发帧测试开始'''

        log.debug("start_test")
        ret_t = RST_SUCCESS
        log_info = ""
        try:
            for port in self.upports:
                gport = self.device_port_list[port - 1]["gport"]
                port_info = "port:%d %s" % (port, gport)
                ret, output = self.start_send_port_packets(port, self.pkt_count, self.pkt_len, self.pkt_dmac, self.pkt_smac)
                if ret != RST_SUCCESS:
                    ret_t -= 1
                    log_info = "sending packet fail, output:%s\n" % output
                    self.port_info_dict[port]["log"] += log_info
                    log.warning("%-18s:%s" %(port_info, log_info))

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_t = RST_EXCEPTION
            return ret_t, log_info
        return ret_t, log_info

    def test_verify(self, **kwargs):
        u'''端口收发帧结果检测'''

        time.sleep(5)
        return self.check_ports_packets(self.upports, self.pkt_count, self.pkt_len, direc="rx")

    def test_cleanup(self):
        u'''端口收发帧测试环境恢复'''

        ret_v, ret_info = RST_SUCCESS, ""
        log.debug("test_cleanup")

        # 清除vlan 配置
        ret, output = self.vlan_config_remove(self.vlan_id)
        if ret != RST_SUCCESS:
            ret_info = "vlan_config_clear fail, output:%s" % output
            log.error(ret_info)
            ret_v = ret

        if ret_v == RST_SUCCESS:
            ret_info = "test cleanup success"
        log.debug(ret_info)
        return ret_v, ret_info

    def test_run(self, port_list=None, redirect=True, mac_lb=False):
        u'''执行端口收发帧测试'''

        result_dict = {
                        "test_type" : "frame",
                        "port_info_dict": {}, 
                        "other_info": "", 
                        "test_result": False,
                        "updownerrorports": [], 
                        "errorports": [], 
                        "successports": []
                    }
        other_info = ""
        log.debug("开始端口收发帧测试")
        # 传入port_list=[] 或 不传入port_list或者入参异常，测试全部端口
        # 传入port_list=[7,8,9,10], 测试面板口7, 8, 9, 10
        if port_list is None or len(port_list) == 0:
            port_list = []
        elif not isinstance(port_list[0], int):
            log_info = "not find port in port_list:%s\n" % port_list
            result_dict["other_info"] += log_info
            log.error(log_info)
            port_list = []
        self.port_list = copy.deepcopy(port_list)  # 防止传入为对象时，对传入对象进行操作
        if len(self.port_list) == 0:
            for i in range(len(self.device_port_list)):
                self.port_list.append(i + 1)
        log.info("port_list %s" % self.port_list)

        run_step = [
            # 拓扑配置
            self.test_setup,
            # 发包
            self.test_start,
            # 报文校验
            self.test_verify,
        ]
        for _ in range(self.retry_times):
            try:
                # 获取端口up状态
                ret_t, other_info = self.get_port_test_info()
                log.debug("up_port: %s" % self.upports)
                if (ret_t < 0):
                    log_info = "get_port_status_f ret_t < 0\n"
                    other_info += log_info
                    log.error(log_info)
                    continue
                elif len(self.upports) == 0:
                    log_info = "get_port_status_f len(upports) == 0\n"
                    other_info += log_info
                    log.error(log_info)
                    continue
                
                for step_fun in run_step:
                    ret, log_info = step_fun()
                    # print(f"Step {step_fun.__name__} completed with result: {ret}, log_info: {log_info}")
                    if ret != RST_SUCCESS:
                        ret_t = ret
                        other_info += log_info
                        log.error(log_info)
                        break  # 开始整个测试项重试

            except Exception:
                msg = traceback.format_exc()
                print("Exception_info:\n%s" % msg)
                ret_t = -999
                continue
            # 即使在 try 块中调用了 return、break 或者 continue，finally 块仍然会执行
            finally:
                ret, _ = self.test_cleanup()
                if ret != RST_SUCCESS:
                    ret_t = ret
                time.sleep(self.cleanup_time)
                # 测试成功才执行端口比较
                if ret_t == RST_SUCCESS:
                    # print("Testing successful, calling compare_start_end_ports")
                    ret_t, log_info = self.compare_start_end_ports()
                    # print(f"compare_start_end_ports result: {ret_t}, log_info: {log_info}")
            
            # 测试失败重测
            if ret_t != RST_SUCCESS:
                continue
            # 有端口不up 重测
            if len(self.updownerrorports) > 0:
                log.warning("Some ports are down, retrying test...")
                ret_t = RST_PORT_STATUS_ERR
                continue

            # 确认各个属性的值是否已经被更新
            # print(f"Final successports: {self.successports}")
            # print(f"Final updownerrorports: {self.updownerrorports}")
            # print(f"Final errorports: {self.errorports}")
            # print(f"Other info collected: {other_info}")
            
        result_dict["successports"]     = copy.deepcopy(self.successports)
        result_dict["updownerrorports"] = copy.deepcopy(self.updownerrorports)
        result_dict["errorports"]       = copy.deepcopy(self.errorports)
        result_dict["port_info_dict"]   = copy.deepcopy(self.port_info_dict)
        result_dict["other_info"] += other_info
        if ret_t == RST_SUCCESS:
            result_dict["test_result"] = True

        return result_dict

@singleton
class PortBrcstTestCTC(PortTestCTC):
    u'''
    # 端口广播
    # 将所有面板口加入一个vlan
    vlan create vlan 4048 default-entry
    vlan add port 0x0 vlan 4048
    port 0x0 default vlan 4048
    clear stats mac-all all
    # 发包让报文泛洪
    tx 10000 VLantag=4048 gport=0 Length=64 DestMac=ff:ff:ff:ff:ff:ff SourceMac=00:00:00:00:00:01
    # 停止端口去使能停流
    port 0x0 port-en disable
    # 校验报文
    show stats mac-all all
    '''
    def __init__(self):
        self.successports = []
        self.errorports = []
        self.updownerrorports = []

        # 显式调用父类的 __init__ 方法
        super().__init__()
        # self.base_config_init()
        # self.test_type = "brcst"
        self.cfg = self.mft_config.get("test_brcst", {})
        self.cfg_default = self.mft_config_default["test_brcst"]
        self.vlan_id = self.cfg.get("vlan_id", self.cfg_default["vlan_id"])
        self.pkt_count = self.cfg.get("pkt_count", self.cfg_default["pkt_count"])
        self.pkt_len = self.cfg.get("pkt_len", self.cfg_default["pkt_len"])
        self.pkt_smac = self.cfg.get("pkt_smac", self.cfg_default["pkt_smac"])
        self.pkt_dmac = self.cfg.get("pkt_dmac", self.cfg_default["pkt_dmac"])
        self.retry_times = self.cfg.get("retry_times", self.cfg_default["retry_times"])
        self.cleanup_time = self.cfg.get("cleanup_time", self.cfg_default["cleanup_time"])

    def test_setup(self):
        u'''端口广播初始化'''

        log.debug("test_setup")

        ret, output = self.vlan_config(self.vlan_id)
        if ret != RST_SUCCESS:
            log_info = "vlan_config fail, output:%s" % output
            log.error(log_info)
            return ret, log_info

        cmd = self.get_sdk_cmd("l2 fdb vlan-default-entry fid {vlan_id} learn-dis".format(vlan_id=self.vlan_id))
        ret, output = sdk_cmd(cmd)
        if ret != RST_SUCCESS:
            log_info = "vlan set learn-dis failed. output:%s" % output
            log.error(log_info)
            return ret, log_info

        ret, output = self.clear_port_packets()
        if ret != RST_SUCCESS:
            log_info = "clear_port_packets fail, output:%s" % output
            log.error(log_info)
            return ret, log_info

        log_info = "init_test success"
        log.debug(log_info)
        return ret, log_info

    def test_start(self, **kwargs):
        u'''端口广播测试开始'''

        log.debug("start_test")
        ret_t = RST_SUCCESS
        log_info = ""
        try:
            # 第一个up的口发包
            port = self.upports[0]
            gport = self.device_port_list[port - 1]["gport"]
            port_info = "port:%d %s" % (port, gport)
            ret, output = self.start_send_port_packets(port, self.pkt_count, self.pkt_len, self.pkt_dmac, self.pkt_smac)
            if ret != RST_SUCCESS:
                ret_t -= 1
                log_info = "sending packet fail, output:%s\n" % output
                self.port_info_dict[port]["log"] += log_info
                log.warning("%-18s:%s" %(port_info, log_info))

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_t = RST_EXCEPTION
            return ret_t, log_info
        return ret_t, log_info

    def test_verify(self, **kwargs):
        u'''端口广播结果检测'''

        time.sleep(5)

        self.successports = []
        self.errorports = []
        ret_t, log_info = RST_SUCCESS, ""

        for port in self.upports:
            # 查看报文收发统计数据
            port_info = "port:%d %s" % (port, self.device_port_list[port - 1]["gport"])
            result = self.get_port_counter(port, ["tx_gbc", "tx_fcs", "rx_gbc", "rx_fcs"])
            tpkt = result["tx_gbc"]["pkt"]
            tfcs = result["tx_fcs"]["pkt"]
            rpkt = result["rx_gbc"]["pkt"]
            rfcs = result["rx_fcs"]["pkt"]
            if tfcs == 0 and rfcs == 0 and tpkt > 0 and rpkt > 0:
                self.successports.append(port)
                log.debug("%s: test success" % (port_info))
            else:
                ret_t -= 1
                self.errorports.append(port)
                log_info = "check ports packets failed! tpkt:%d,tfcs:%d, rpkt:%d, rfcs:%d" % (
                    tpkt, tfcs, rpkt, rfcs)
                log_info += get_last_cmd_output()
                self.port_info_dict[port]["log"] += log_info
                log.error("%-18s: %s" % (port_info, log_info))

        if ret_t == RST_SUCCESS:
            log_info = "check ports packets succeed"
        else:
            log_info = "check ports packets failed!"
        return ret_t, log_info

    def test_cleanup(self):
        u'''端口广播测试环境恢复'''

        ret_v, ret_info = RST_SUCCESS, ""
        log.debug("test_cleanup")

        # 先停掉报文
        # 去使能的时候，先port en disable， 再mac disable
        # 25-01-21 更新顺序，原顺序连续跑广播会导致端口报文全fcs
        cmd = self.get_sdk_cmd(self.cmd_port_mac_set_disable.format(port="all"))
        ret, output = sdk_cmd(cmd)
        if ret != RST_SUCCESS:
            log_info = "port set disable failed. output:%s" % (output)
            log.error(log_info)
            ret_v = ret

        cmd = self.get_sdk_cmd(self.cmd_port_en_set_disable.format(port="all"))
        ret, output = sdk_cmd(cmd)
        if ret != RST_SUCCESS:
            log_info = "port set disable failed. output:%s" % (output)
            log.error(log_info)
            ret_v = ret

        # 使能的时候，先mac enable，再port en enable
        cmd = self.get_sdk_cmd(self.cmd_port_mac_set_enable.format(port="all"))
        ret, output = sdk_cmd(cmd)
        if ret != RST_SUCCESS:
            log_info = "port set enable failed. output:%s" % (output)
            log.error(log_info)
            ret_v = ret

        cmd = self.get_sdk_cmd(self.cmd_port_en_set_enable.format(port="all"))
        ret, output = sdk_cmd(cmd)
        if ret != RST_SUCCESS:
            log_info = "port set enable failed. output:%s" % (output)
            log.error(log_info)
            ret_v = ret

        # 清除vlan 配置
        ret, output = self.vlan_config_remove(self.vlan_id)
        if ret != RST_SUCCESS:
            ret_info = "vlan_config_clear fail, output:%s" % output
            log.error(ret_info)
            ret_v = ret

        if ret_v == RST_SUCCESS:
            ret_info = "test cleanup success"
        log.debug(ret_info)
        return ret_v, ret_info

    def test_run(self, port_list=None, redirect=True):
        u'''执行端口收发帧测试'''

        result_dict = {
                        "test_type" : "brcst",
                        "port_info_dict": {}, 
                        "other_info": "", 
                        "test_result": False,
                        "updownerrorports": [], 
                        "errorports": [], 
                        "successports": []
                    }
        other_info = ""

        log.debug("开始端口广播测试")
        # 传入port_list=[] 或 不传入port_list或者入参异常，测试全部端口
        # 传入port_list=[7,8,9,10], 测试面板口7, 8, 9, 10
        if port_list is None or len(port_list) == 0:
            port_list = []
        elif not isinstance(port_list[0], int):
            log_info = "not find port in port_list:%s\n" % port_list
            result_dict["other_info"] += log_info
            log.error(log_info)
            port_list = []
        self.port_list = copy.deepcopy(port_list)  # 防止传入为对象时，对传入对象进行操作
        if len(self.port_list) == 0:
            for i in range(len(self.device_port_list)):
                self.port_list.append(i + 1)
        log.info("port_list %s" % self.port_list)

        run_step = [
            # 拓扑配置
            self.test_setup,
            # 发包
            self.test_start,
            # 报文校验
            self.test_verify,
        ]
        for _ in range(self.retry_times):
            try:
                # 获取端口up状态
                ret_t, other_info = self.get_port_test_info()
                log.debug("up_port: %s" % self.upports)
                if (ret_t < 0):
                    log_info = "get_port_status_f ret_t < 0\n"
                    other_info += log_info
                    log.error(log_info)
                    continue
                elif len(self.upports) == 0:
                    log_info = "get_port_status_f len(upports) == 0\n"
                    other_info += log_info
                    log.error(log_info)
                    continue
                
                for step_fun in run_step:
                    ret, log_info = step_fun()
                    # print(f"Step {step_fun.__name__} completed with result: {ret}, log_info: {log_info}")
                    if ret != RST_SUCCESS:
                        ret_t = ret
                        other_info += log_info
                        log.error(log_info)
                        break  # 开始整个测试项重试

            except Exception:
                msg = traceback.format_exc()
                print("Exception_info:\n%s" % msg)
                ret_t = -999
                continue
            # 即使在 try 块中调用了 return、break 或者 continue，finally 块仍然会执行
            finally:
                ret, _ = self.test_cleanup()
                if ret != RST_SUCCESS:
                    ret_t = ret
                time.sleep(self.cleanup_time)
                # 测试成功才执行端口比较
                if ret_t == RST_SUCCESS:
                    # print("Testing successful, calling compare_start_end_ports")
                    ret_t, log_info = self.compare_start_end_ports()
                    # print(f"compare_start_end_ports result: {ret_t}, log_info: {log_info}")
            
            # 测试失败重测
            if ret_t != RST_SUCCESS:
                continue
            # 有端口不up 重测
            if len(self.updownerrorports) > 0:
                log.warning("Some ports are down, retrying test...")
                ret_t = RST_PORT_STATUS_ERR
                continue

            # 确认各个属性的值是否已经被更新
            # print(f"Final successports: {self.successports}")
            # print(f"Final updownerrorports: {self.updownerrorports}")
            # print(f"Final errorports: {self.errorports}")
            # print(f"Other info collected: {other_info}")
            
        result_dict["successports"]     = copy.deepcopy(self.successports)
        result_dict["updownerrorports"] = copy.deepcopy(self.updownerrorports)
        result_dict["errorports"]       = copy.deepcopy(self.errorports)
        result_dict["port_info_dict"]   = copy.deepcopy(self.port_info_dict)
        result_dict["other_info"] += other_info
        if ret_t == RST_SUCCESS:
            result_dict["test_result"] = True

        return result_dict

@singleton
class PortPrbsTestCTC(PortTestCTC):
    u'''
    # 端口prbs测试
    # 所有端口启动prbs
    # mac侧启动prbs
    in
    port link monitor disable
    dk
    serdes 43 prbs 16 enable
    serdes 42 prbs 16 enable
    # phy启动prbs
    sdk
    phy user_diag 40 prbs_set if_side=sys poly=p31
    phy user_diag 40 prbs_set if_side=line poly=p31
    dk
    # mac侧清0一次统计
    serdes 43 prbs 16 check keep
    serdes 42 prbs 16 check keep
    # phy清0一次统计，启动的时候会清零，但是以为启动和清零之间时间很短，重新启动来清0
    phy user_diag 40 prbs_get if_side=sys poly=p31
    phy user_diag 40 prbs_get if_side=line poly=p31
    # 等待指定时间
    sleep(time)
    # 获取port prbs统计
    serdes 43 prbs 16 check keep
    serdes 42 prbs 16 check keep
    # 获取phyprbs统计
    sdk
    phy user_diag 40 prbs_get if_side=sys
    phy user_diag 40 prbs_get if_side=line
    # 清除prbs
    phy user_diag 40 prbs_clear if_side=sys
    phy user_diag 40 prbs_clear if_side=line
    dk
    serdes 43 prbs 16 disable
    serdes 42 prbs 16 disable
    in
    port link monitor enable
    '''
    def __init__(self):
        self.successports = []
        self.errorports = []
        self.updownerrorports = []

        super().__init__()
        # self.base_config_init()
        self.prbs_cfg_init()
        self.serdes_info_init()
        self.prbs_cmd_init()

    def prbs_cfg_init(self):
        self.cfg = self.mft_config["test_prbs"]
        self.cfg_default = self.mft_config_default["test_prbs"]
        self.prbs_port_list = PortUtil.convert_to_list(self.cfg.get("port_list", ""))
        self.prbs_port_list_ext_phy = PortUtil.convert_to_list(self.cfg.get("port_list_ext_phy", ""))
        self.prbs_proc = self.cfg.get("proc", self.cfg_default["proc"])
        self.retry_times = self.cfg.get("retry_times", self.cfg_default["retry_times"])
        self.duration_time = self.cfg.get("duration_time", self.cfg_default["duration_time"])
        self.cleanup_time = self.cfg.get("cleanup_time", self.cfg_default["cleanup_time"])
        self.prbs_ber_init()

    def prbs_ber_init(self):
        self.cfg = self.mft_config["test_prbs"]
        self.cfg_default = self.mft_config_default["test_prbs"]
        prbs_ber = self.cfg.get("prbs_ber", self.cfg_default["prbs_ber"])
        prbs_ber_dict = self.cfg.get("prbs_ber_dict", None)
        prbs_ber_lane_dict = self.cfg.get("prbs_ber_lane_dict", None)

        self.prbs_ber = {
            "mac":{},
            "sys":{},
            "line":{},
        }
        for port in range(len(self.device_port_list)):
            self.prbs_ber["mac"][port+1] = {}
            self.prbs_ber["sys"][port+1] = {}
            self.prbs_ber["line"][port+1] = {}
            for lane in range (LANE_MAX):
                self.prbs_ber["mac"][port+1][lane] = prbs_ber
                self.prbs_ber["sys"][port+1][lane] = prbs_ber
                self.prbs_ber["line"][port+1][lane] = prbs_ber
        
        if prbs_ber_dict:
            side_cfg = False
            if "mac" in prbs_ber_dict:
                side_cfg = True
                for port, ber in prbs_ber_dict["mac"].items():
                    for lane in range (LANE_MAX):
                        self.prbs_ber["mac"][port][lane] = ber
            if "sys" in prbs_ber_dict:
                side_cfg = True
                for port, ber in prbs_ber_dict["sys"].items():
                    for lane in range (LANE_MAX):
                        self.prbs_ber["sys"][port][lane] = ber
            if "line" in prbs_ber_dict:
                side_cfg = True
                for port, ber in prbs_ber_dict["line"].items():
                    for lane in range (LANE_MAX):
                        self.prbs_ber["line"][port][lane] = ber
            if not side_cfg:
                for port, ber in prbs_ber_dict.items():
                    for lane in range (LANE_MAX):
                        self.prbs_ber["mac"][port][lane] = ber
                        self.prbs_ber["sys"][port][lane] = ber
                        self.prbs_ber["line"][port][lane] = ber

        if prbs_ber_lane_dict:
            side_cfg = False
            if "mac" in prbs_ber_dict:
                side_cfg = True
                for port, line_ber in prbs_ber_lane_dict["mac"].items():
                    for lane, ber in line_ber.items():
                        self.prbs_ber["mac"][port][lane] = ber
            if "sys" in prbs_ber_dict:
                side_cfg = True
                for port, line_ber in prbs_ber_lane_dict["sys"].items():
                    for lane, ber in line_ber.items():
                        self.prbs_ber["sys"][port][lane] = ber
            if "line" in prbs_ber_dict:
                side_cfg = True
                for port, line_ber in prbs_ber_lane_dict["line"].items():
                    for lane, ber in line_ber.items():
                        self.prbs_ber["line"][port][lane] = ber
            if not side_cfg:
                for port, line_ber in prbs_ber_lane_dict.items():
                    for lane, ber in line_ber.items():
                        self.prbs_ber["mac"][port][lane] = ber
                        self.prbs_ber["sys"][port][lane] = ber
                        self.prbs_ber["line"][port][lane] = ber
        # print("prbs:%s" % str(json.dumps(self.prbs_ber, indent=4, sort_keys=True)))

    def prbs_cmd_init(self):
        com_cmd = self.mft_config["test_prbs"]["test_cmd"]
        default_cmd = self.mft_config_default["test_prbs"]["test_cmd"]
        self.cmd_para_dict = com_cmd.get("para_dict", default_cmd["para_dict"])

        self.cmd_prbs_start = com_cmd.get("prbs_start", default_cmd["prbs_start"])
        self.cmd_prbs_get   = com_cmd.get("prbs_get", default_cmd["prbs_get"])
        self.cmd_prbs_clear = com_cmd.get("prbs_clear", default_cmd["prbs_clear"])

        self.cmd_prbs_mac_start = com_cmd.get("prbs_mac_start", default_cmd["prbs_mac_start"])
        self.cmd_prbs_mac_get   = com_cmd.get("prbs_mac_get", default_cmd["prbs_mac_get"])
        self.cmd_prbs_mac_pre_check   = com_cmd.get("prbs_mac_pre_check", default_cmd["prbs_mac_pre_check"])
        # self.cmd_prbs_mac_start_rx_check   = com_cmd.get("prbs_mac_start_rx_check", default_cmd["prbs_mac_start_rx_check"])

        # self.cmd_prbs_mac_get_clean = com_cmd.get("prbs_mac_get_clean", default_cmd["prbs_mac_get_clean"])
        self.cmd_prbs_mac_clear = com_cmd.get("prbs_mac_clear", default_cmd["prbs_mac_clear"])
        
        self.cmd_prbs_sys_start = com_cmd.get("prbs_sys_start", default_cmd["prbs_sys_start"])
        self.cmd_prbs_sys_get   = com_cmd.get("prbs_sys_get", default_cmd["prbs_sys_get"])
        self.cmd_prbs_sys_clear = com_cmd.get("prbs_sys_clear", default_cmd["prbs_sys_clear"])

        self.cmd_prbs_line_start = com_cmd.get("prbs_line_start", default_cmd["prbs_line_start"])
        self.cmd_prbs_line_get   = com_cmd.get("prbs_line_get", default_cmd["prbs_line_get"])
        self.cmd_prbs_line_clear = com_cmd.get("prbs_line_clear", default_cmd["prbs_line_clear"])

    def serdes_info_init(self):
        u'''
        获取面板口对应的serdes
        命令输出样例：
        CTC_CLI(ctc-sdk)#  show datapath info serdes 
        INDEX SERDESID SLICE    HSSID    MODE      BW       RS       PLLSEL   CHAN  MAC   LPORT DYN   OVERCLOCK
        -------------------------------------------------------------------------------------------------------
        0     0        0        0        Cg        32BITS   FULL     NONE     0     0     0     1     0        
        1     1        0        0        Cg        32BITS   FULL     NONE     0     0     0     1     0        
        2     2        0        0        Cg        32BITS   FULL     NONE     0     0     0     1     0        
        3     3        0        0        Cg        32BITS   FULL     NONE     0     0     0     1     0        
        '''
        try:
            cmd = self.get_sdk_cmd(self.cmd_show_serdes_info)
            ret, output = sdk_cmd(cmd)
            if ret == RST_SUCCESS:
                # log.info("output:%s" % output)
                # 按行分割输入文本
                lines = output.strip().splitlines()
                # 找到包含 "GPort" 的行，确定列的索引
                for idx, line in enumerate(lines):
                    if "SERDESID" in line:
                        header = line.strip().split()
                        serdes_idx = header.index('SERDESID')
                        lport_idx = header.index('LPORT')
                        data_start_idx = idx + 2  # 数据行从 GPort 行的两行之后开始
                        break
                # 遍历数据行
                for line in lines[data_start_idx:]:
                    parts = line.strip().split()
                    if len(parts) <= lport_idx:
                        break
                    lport = int(parts[lport_idx])
                    gport_str = f"0x{lport:04x}"
                    serdes_list = self.gport_dict[gport_str].get("serdes", [])
                    serdes_list.append(int(parts[serdes_idx]))
                    self.gport_dict[gport_str]["serdes"] = serdes_list
            else:
                log.error("show serdes info failed")
                log.error("ret:%d, cmd:%s" % (ret, get_last_cmd()))
                log.error("output:%s" % get_last_cmd_output())
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))

    def run_cmd_list(self, cmd_list, static_val, dny_val):
        ret = -1
        output = "cmd list is None"
        if cmd_list is None or len(cmd_list) == 0:
            log.error("cmd list is None")
            return ret, output
        try:
            for command in cmd_list:
                cmd = command["cmd"]
                para_list = command.get("para", None)
                final_para_list = []
                if not isinstance(para_list, list):
                    para_list = [para_list]
                for para in para_list:
                    if para.startswith('static_'):
                        final_para_list.append(static_val.get(para,""))
                    elif para.startswith('dyn_'):
                        final_para_list.append(dny_val.get(para,""))
                    else:
                        final_para_list.append(para)

                sleep = command.get("sleep", 0)
                if len(final_para_list):
                    # 如果 para 是一个列表，则解包 para；否则将 para 当作一个参数
                    #log.debug("Command:%s, para:%s" % (cmd, str(final_para_list)))
                    cmd = cmd.format(*final_para_list)
                #log.debug("Command:%s, Sleep:%d" % (cmd, sleep))
                ret, output = sdk_cmd(cmd)
                if ret != RST_SUCCESS:
                    return ret, output
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd_list:%s" % cmd_list)
            log.error("static_val:%s" % static_val)
            log.error("dny_val:%s" % dny_val)
            ret_t = RST_EXCEPTION
            return ret_t, "Command parsing error"
        return ret, output

    def test_setup(self, **kwargs):
        u'''端口prbs测试初始化'''

        log.debug("test setup")
        test_type = kwargs["test_type"]
        ret = RST_SUCCESS

        if test_type != "prbs_line":
            cmd = self.get_sdk_cmd(["in","port link monitor disable"])
            ret, output = sdk_cmd(cmd)
            if ret != RST_SUCCESS:
                log_info = "port link monitor disable fail, output:%s" % output
                log.error(log_info)
                return ret, log_info

        log_info = "test setup end"
        log.debug(log_info)
        return ret, log_info

    # def prbs_start(self, port)

    def test_start(self, **kwargs):
        u'''端口prbs测试测试开始'''

        log.debug("test start")
        ret_t = RST_SUCCESS
        log_info = ""
        test_type = kwargs["test_type"]
        local_para = {}
        self.start_time = time.time()
        try:
            # 外部phy一起移动测试
            if test_type == "prbs_all":
                for port in self.upports:
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    # phy 侧启动
                    if port in self.prbs_port_list_ext_phy:
                        ret, output = self.run_cmd_list(self.cmd_prbs_sys_start, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                            log.error(log_info)
                        ret, output = self.run_cmd_list(self.cmd_prbs_line_start, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                            log.error(log_info)
                        # mac 侧启动
                        for serdes in serdes_list:
                            local_para["dyn_serdes"] = serdes
                            ret, output = self.run_cmd_list(self.cmd_prbs_mac_start, self.cmd_para_dict, local_para)
                            if ret != RST_SUCCESS:
                                log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                                log.error(log_info)

                    elif port in self.prbs_port_list:
                        # mac 侧启动
                        for serdes in serdes_list:
                            local_para["dyn_serdes"] = serdes
                            ret, output = self.run_cmd_list(self.cmd_prbs_start, self.cmd_para_dict, local_para)
                            if ret != RST_SUCCESS:
                                log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                                log.error(log_info)
                    else:
                        log.error("out of range port:%s" % gport)

                # 所有都启动后，再清统计
                for port in self.upports:
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    if port in self.prbs_port_list_ext_phy:
                        # mac 侧获取一次，启动统计
                        for serdes in serdes_list:
                            local_para["dyn_serdes"] = serdes
                            ret, output = self.run_cmd_list(self.cmd_prbs_mac_get, self.cmd_para_dict, local_para)
                            if ret != RST_SUCCESS:
                                log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                                log.error(log_info)
                        # phy侧获取一次统计，用于清除统计
                        ret, output = self.run_cmd_list(self.cmd_prbs_sys_get, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                            log.error(log_info)
                        ret, output = self.run_cmd_list(self.cmd_prbs_line_get, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                            log.error(log_info)

            # 非外部phy口
            elif test_type == "prbs":
                for port in self.upports:
                    if port not in self.prbs_port_list:
                        continue
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    for serdes in serdes_list:
                        local_para["dyn_serdes"] = serdes
                        ret, output = self.run_cmd_list(self.cmd_prbs_start, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                            log.error(log_info)
            
            # 外部phy口，mac侧
            elif test_type == "prbs_mac":
                for port in self.upports:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    # phy 侧启动
                    ret, output = self.run_cmd_list(self.cmd_prbs_sys_start, self.cmd_para_dict, local_para)
                    if ret != RST_SUCCESS:
                        log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                        log.error(log_info)
                    # mac 侧启动
                    for serdes in serdes_list:
                        local_para["dyn_serdes"] = serdes
                        ret, output = self.run_cmd_list(self.cmd_prbs_mac_start, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                            log.error(log_info)

                for port in self.upports:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    for serdes in serdes_list:
                        # mac 侧获取一次，启动统计
                        local_para["dyn_serdes"] = serdes
                        ret, output = self.run_cmd_list(self.cmd_prbs_mac_get, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                            log.error(log_info)
            
            # 外部phy口，sys侧
            elif test_type == "prbs_sys":
                for port in self.upports:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    # mac 侧启动
                    for serdes in serdes_list:
                        local_para["dyn_serdes"] = serdes
                        ret, output = self.run_cmd_list(self.cmd_prbs_mac_start, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                            log.error(log_info)
                    ret, output = self.run_cmd_list(self.cmd_prbs_sys_start, self.cmd_para_dict, local_para)
                    if ret != RST_SUCCESS:
                        log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                        log.error(log_info)

                for port in self.upports:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    # phy侧获取一次统计，用于清除统计
                    ret, output = self.run_cmd_list(self.cmd_prbs_sys_get, self.cmd_para_dict, local_para)
                    if ret != RST_SUCCESS:
                        log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                        log.error(log_info)

            # 外部phy口，line侧
            elif test_type == "prbs_line":
                for port in self.upports:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    ret, output = self.run_cmd_list(self.cmd_prbs_line_start, self.cmd_para_dict, local_para)
                    if ret != RST_SUCCESS:
                        log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                        log.error(log_info)

                for port in self.upports:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    # phy侧获取一次统计，用于清除统计
                    ret, output = self.run_cmd_list(self.cmd_prbs_line_get, self.cmd_para_dict, local_para)
                    if ret != RST_SUCCESS:
                        log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                        log.error(log_info)
            self.start_finished_time = time.time()
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_t = RST_EXCEPTION
            return ret_t, log_info
        return ret_t, log_info

    def analyze_prbs_rst_mac(self, port, ber_th, side=""):
        u'''
        端口prbs测试mac侧结果解析
        输出样例：
        CTC_CLI(ctc-dkits)# serdes 8 prbs 6 check delay 1000  
        Serdes_ID   Dir   Pattern   Enable   Result   Err_cnt  
        -----------------------------------------------------
        8           RX    PRBS31      -      Pass     0  

        CTC_CLI(ctc-dkits)# serdes 42 prbs 16 check delay 1000
        Serdes_ID   Dir   Pattern   Enable   Result   BER      
        -----------------------------------------------------
        42          RX    PRBS31Q     -      Pass     0.0E0
        解析结果：
        port:  9 0x0028    Lane[0]: 43 prbs_ber: 0.000e+00 < 1.000e-09, test success
        '''
        ret_v = RST_SUCCESS
        prbs_info = ""
        log_info = ""
        gport = self.device_port_list[port - 1]["gport"]
        serdes_list = self.device_port_list[port - 1]["serdes"]
        port_info = "port:%3d %s" % (port, gport)
        local_para = {}
        local_para["dyn_port"] = gport
        if side == "mac":
            result_dict = self.result_dict["prbs_mac_result_dict"]
            cmd_prbs_get = self.cmd_prbs_mac_get
        else:
            result_dict = self.result_dict["prbs_result_dict"]
            cmd_prbs_get = self.cmd_prbs_get

        try:
            for lane in range(len(serdes_list)):
                local_para["dyn_serdes"] = serdes_list[lane]
                ret, output = self.run_cmd_list(cmd_prbs_get, self.cmd_para_dict, local_para)
                get_lane_ber = False
                lane_check = RST_SUCCESS
                if ret == RST_SUCCESS:
                    lines = output.lower().strip().splitlines()
                    for idx, line in enumerate(lines):
                        header = line.strip().split()
                        if "ber" in line:
                            ber_idx = header.index('ber')
                            data_start_idx = idx + 2  # 数据行从 GPort 行的两行之后开始
                            parts = lines[data_start_idx].strip().split()
                            ber = float(parts[ber_idx])
                            if ber > float(ber_th[lane]):
                                prbs_info += "%-20s Lane[%d]:%3d prbs_ber: %.3e > %.3e, test fail\n" % (port_info, lane, serdes_list[lane], ber, ber_th[lane])
                                lane_check = ret_v = RST_CHECK_FAILED
                            else:
                                prbs_info+= "%-20s Lane[%d]:%3d prbs_ber: %.3e < %.3e, test success\n" % (port_info, lane, serdes_list[lane], ber, ber_th[lane])
                            get_lane_ber = True
                            break
                        elif "err_cnt" in line:
                            err_cnt_idx = header.index('err_cnt')
                            data_start_idx = idx + 2  # 数据行从 GPort 行的两行之后开始
                            parts = lines[data_start_idx].strip().split()
                            err_cnt = int(parts[err_cnt_idx])
                            if err_cnt > 0:
                                prbs_info+= "%-20s Lane[%d]:%3d prbs_err_cnt: %d > 0, test fail\n" % (port_info, lane, serdes_list[lane], err_cnt)
                                lane_check = ret_v = RST_CHECK_FAILED
                            else:
                                prbs_info+= "%-20s Lane[%d]:%3d prbs_err_cnt: %d, test success\n" % (port_info, lane, serdes_list[lane], err_cnt)
                            get_lane_ber = True
                            break

                if not get_lane_ber:
                    prbs_info += "%-20s Lane[%d]:%3d get prbs ber failed, test fail\n" % (port_info, lane, serdes_list[lane])
                    lane_check = ret_v = RST_CHECK_FAILED

                if lane_check != RST_SUCCESS:
                    log_info += "%s Lane[%d]:%3d prbs check failed!ret=%d\n%s\n%s\n" % (port_info, lane, serdes_list[lane], ret, get_last_cmd(), output)

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_v = RST_EXCEPTION
            prbs_info += "%-20s Lane[x] get prbs ber failed, test fail\n" % (port, gport)
            log_info += "Exception_info:\n%s" % msg

        result_dict["prbs_info"] += "\n"+prbs_info
        if len(log_info):
            result_dict["other_info"] += "\n"+log_info
        if ret_v != RST_SUCCESS:
            result_dict["test_result"] = False
            result_dict["errorports"].append(port)
        else:
            result_dict["successports"].append(port)
        return ret_v, log_info

    def analyze_prbs_rst_phy(self, port, ber_th, side):
        u'''
        端口prbs测试phy侧结果解析
        输出样例：
        CTC_CLI(ctc-sdk)# phy user_diag 40 prbs_get if_side=all
        port_lane  phy_lane  tx_gen  rx_chk  prbs_lock  lock_loss  err_dur err_cnt      ber
        0(sys)     0         PRBS31  PRBS31  YES        -               7s 0            0.000e+00
        1(sys)     1         PRBS31  PRBS31  YES        -               7s 12           3.061e-11
        0(line)    8         PRBS31  PRBS31  -          -               7s 0            0.000e+00
        1(line)    9         PRBS31  PRBS31  -          -               7s 0            0.000e+00
        2(line)    10        PRBS31  PRBS31  -          -               7s 0            0.000e+00
        3(line)    11        PRBS31  PRBS31  -          -               7s 0            0.000e+00
        解析结果：
        port: 1  0x0008     Lane[0]:2 prbs_ber: 3.23e-13 <= 1.00E-09, test success
        '''
        ret_v = RST_SUCCESS
        prbs_info = ""
        log_info = ""
        gport = self.device_port_list[port - 1]["gport"]
        port_info = "port:%3d %s" % (port, gport)
        local_para = {}
        local_para["dyn_port"] = gport
        if side == "sys":
            cmd_prbs_get = self.cmd_prbs_sys_get
            result_dict = self.result_dict["prbs_sys_result_dict"]
        else:
            cmd_prbs_get = self.cmd_prbs_line_get
            result_dict = self.result_dict["prbs_line_result_dict"]

        try:
            ret, output = self.run_cmd_list(cmd_prbs_get, self.cmd_para_dict, local_para)
            get_lane_ber = False
            if ret == RST_SUCCESS:
                lines = output.lower().strip().splitlines()
                data_start_idx = 0
                for idx, line in enumerate(lines):
                    header = line.strip().split()
                    if "port_lane" in line and "ber" in line:
                        lane_idx = header.index('port_lane')
                        ber_idx = header.index('ber')
                        phy_lane_idx = header.index('phy_lane')
                        data_start_idx = idx + 1
                        break
                if data_start_idx != 0:
                    # 遍历数据行
                    for line in lines[data_start_idx:]:
                        parts = line.strip().split()
                        if len(parts) <= ber_idx:
                            break
                        match = re.match(r"(\d+)\((\w+)\)", parts[lane_idx])
                        lane = int(match.group(1))
                        phy_lane = parts[phy_lane_idx]
                        ber = float(parts[ber_idx])
                        if ber > float(ber_th[lane]):
                            prbs_info += "%-20s Lane[%d]:%2s prbs_ber: %.3e > %.3e, test fail\n" % (port_info, lane, phy_lane, ber, ber_th[lane])
                            ret_v = RST_CHECK_FAILED
                        else:
                            prbs_info += "%-20s Lane[%d]:%2s prbs_ber: %.3e < %.3e, test success\n" % (port_info, lane, phy_lane, ber, ber_th[lane])
                        get_lane_ber = True

            if not get_lane_ber:
                prbs_info+= "%-20s Lane[x] get prbs ber failed, test fail\n" % (port_info)
                ret_v = RST_CHECK_FAILED
            if ret_v != RST_SUCCESS:
                log_info += "%s prbs check failed! ret=%d\n%s\n%s\n" % (port_info, ret, get_last_cmd(), output)

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_v = RST_EXCEPTION
            prbs_info += "port:%3d %2s    Lane[x] get prbs ber failed, test fail\n" % (port, gport)
            log_info += "Exception_info:\n%s" % msg

        result_dict["prbs_info"] += "\n"+prbs_info
        if len(log_info):
            result_dict["other_info"] += "\n"+log_info
        if ret_v != RST_SUCCESS:
            result_dict["test_result"] = False
            result_dict["errorports"].append(port)
        else:
            result_dict["successports"].append(port)
        return ret_v

    def test_verify(self, **kwargs):
        u'''端口prbs结果检测'''

        # 每侧prbs启动要2条命令，校验一条命令。所以启动流程花费的时间大概是校验的2倍
        
        # start   start_finished   (sleep_time)     check_start     check_end
        #  |------------|---------------------------------|-------------|
        #         |---------------------------------------|
        #                       duration_time
        time_span = (self.start_finished_time - self.start_time)/2
        if time_span < self.duration_time:
            time.sleep(self.duration_time-time_span + 1)
        test_type = kwargs["test_type"]
        log.debug("test verify type:%s" % test_type)
        self.successports = []
        self.errorports = []
        ret_t, log_info = RST_SUCCESS, ""
        self.result_dict["prbs_result_dict"]["prbs_info"] = "prbs_mac_test"
        self.result_dict["prbs_mac_result_dict"]["prbs_info"] = "prbs_mac_test"
        self.result_dict["prbs_sys_result_dict"]["prbs_info"] = "prbs_sys_test"
        self.result_dict["prbs_line_result_dict"]["prbs_info"] = "prbs_line_test"
        
        self.result_dict["prbs_result_dict"]["successports"] = []
        self.result_dict["prbs_mac_result_dict"]["successports"] = []
        self.result_dict["prbs_sys_result_dict"]["successports"] = []
        self.result_dict["prbs_line_result_dict"]["successports"] = []

        self.result_dict["prbs_result_dict"]["errorports"] = []
        self.result_dict["prbs_mac_result_dict"]["errorports"] = []
        self.result_dict["prbs_sys_result_dict"]["errorports"] = []
        self.result_dict["prbs_line_result_dict"]["errorports"] = []

        # 外部phy一起启动测试
        if test_type == "prbs_all":
            for port in self.upports:
                if port in self.prbs_port_list_ext_phy:
                    # extphy port mac
                    self.analyze_prbs_rst_mac(port, self.prbs_ber["mac"][port], "mac")
                    # extphy port sys
                    self.analyze_prbs_rst_phy(port, self.prbs_ber["sys"][port], "sys")
                    # extphy port line
                    self.analyze_prbs_rst_phy(port, self.prbs_ber["line"][port], "line")
                elif port in self.prbs_port_list:
                    self.analyze_prbs_rst_mac(port, self.prbs_ber["mac"][port])
                else:
                    log.error("out of range port:%s" % port)
        # 非外部phy口
        elif test_type == "prbs":
            for port in self.upports:
                if port not in self.prbs_port_list:
                    continue
                self.analyze_prbs_rst_mac(port, self.prbs_ber["mac"][port])
        
        # 外部phy口，mac侧
        elif test_type == "prbs_mac":
            for port in self.upports:
                if port not in self.prbs_port_list_ext_phy:
                    continue
                self.analyze_prbs_rst_mac(port, self.prbs_ber["mac"][port], "mac")

        # 外部phy口，sys侧
        elif test_type == "prbs_sys":
            for port in self.upports:
                if port not in self.prbs_port_list_ext_phy:
                    continue
                self.analyze_prbs_rst_phy(port, self.prbs_ber["sys"][port], "sys")

        # 外部phy口，line侧
        elif test_type == "prbs_line":
            for port in self.upports:
                if port not in self.prbs_port_list_ext_phy:
                    continue
                self.analyze_prbs_rst_phy(port, self.prbs_ber["line"][port], "line")

        return ret_t, log_info

    def test_cleanup(self, **kwargs):
        u'''端口prbs测试环境恢复'''

        ret_v = RST_SUCCESS
        log_info = ""
        test_type = kwargs["test_type"]
        log.debug("test cleanup type:%s" % test_type)
        local_para = {}
        try:
            # 外部phy一起移动测试
            if test_type == "prbs_all":
                for port in self.port_list:
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    if port in self.prbs_port_list_ext_phy:
                        ret, output = self.run_cmd_list(self.cmd_prbs_sys_clear, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                            log.error(log_info)
                        ret, output = self.run_cmd_list(self.cmd_prbs_line_clear, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                            log.error(log_info)
                        for serdes in serdes_list:
                            local_para["dyn_serdes"] = serdes
                            ret, output = self.run_cmd_list(self.cmd_prbs_mac_clear, self.cmd_para_dict, local_para)
                            if ret != RST_SUCCESS:
                                log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                                log.error(log_info)
                    elif port in self.prbs_port_list:
                        for serdes in serdes_list:
                            local_para["dyn_serdes"] = serdes
                            ret, output = self.run_cmd_list(self.cmd_prbs_clear, self.cmd_para_dict, local_para)
                            if ret != RST_SUCCESS:
                                log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                                log.error(log_info)
                    else:
                        log.error("out of range port:%s" % gport)
            # 非外部phy口
            elif test_type == "prbs":
                for port in self.port_list:
                    if port not in self.prbs_port_list:
                        continue
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    for serdes in serdes_list:
                        local_para["dyn_serdes"] = serdes
                        ret, output = self.run_cmd_list(self.cmd_prbs_clear, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                            log.error(log_info)
                        
            
            # 外部phy口，mac侧
            elif test_type == "prbs_mac":
                for port in self.port_list:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    ret, output = self.run_cmd_list(self.cmd_prbs_sys_clear, self.cmd_para_dict, local_para)
                    if ret != RST_SUCCESS:
                        log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                        log.error(log_info)
                    for serdes in serdes_list:
                        local_para["dyn_serdes"] = serdes
                        ret, output = self.run_cmd_list(self.cmd_prbs_mac_clear, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                            log.error(log_info)
            
            # 外部phy口，sys侧
            elif test_type == "prbs_sys":
                for port in self.port_list:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    gport = self.device_port_list[port - 1]["gport"]
                    serdes_list = self.device_port_list[port - 1]["serdes"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    for serdes in serdes_list:
                        local_para["dyn_serdes"] = serdes
                        ret, output = self.run_cmd_list(self.cmd_prbs_mac_start, self.cmd_para_dict, local_para)
                        if ret != RST_SUCCESS:
                            log_info =  "%s %s %d cmd exec failed\n%s\n%s\n" % (test_type, port_info, serdes, get_last_cmd(), output)
                            log.error(log_info)
                    ret, output = self.run_cmd_list(self.cmd_prbs_sys_clear, self.cmd_para_dict, local_para)
                    if ret != RST_SUCCESS:
                        log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                        log.error(log_info)

            # 外部phy口，line侧
            elif test_type == "prbs_line":
                for port in self.port_list:
                    if port not in self.prbs_port_list_ext_phy:
                        continue
                    gport = self.device_port_list[port - 1]["gport"]
                    local_para["dyn_port"] = gport
                    port_info = "port:%3d %s" % (port, gport)
                    ret, output = self.run_cmd_list(self.cmd_prbs_line_clear, self.cmd_para_dict, local_para)
                    if ret != RST_SUCCESS:
                        log_info =  "%s %s cmd exec failed\n%s\n%s\n" % (test_type, port_info, get_last_cmd(), output)
                        log.error(log_info)

            if test_type != "prbs_line":
                cmd = self.get_sdk_cmd(["in","port link monitor disable"])
                ret, output = sdk_cmd(cmd)
                if ret != RST_SUCCESS:
                    log_info = "port link monitor disable fail, output:%s" % output
                    log.error(log_info)
                    ret_v = ret

        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log.error("cmd:%s" % get_last_cmd())
            log.error("status:%s, output:%s" % (get_last_cmd_status(), get_last_cmd_output()))
            ret_v = RST_EXCEPTION
        
        if ret_v == RST_SUCCESS:
            ret_info = "test cleanup success"
        log.debug(ret_info)
        return ret_v, ret_info

    def test_run(self, port_list=None, test_type="", redirect=True):
        u'''执行端口prbs测试'''

        log.debug("开始端口prbs测试")
        if (test_type != ""):
            log.debug("test_type: %s" % test_type)
            self.test_type = test_type
        else:
            log.debug("test_type: prbs_mac、prbs_sys、prbs_line")
            self.test_type = "prbs_all"

        result_dist = {
                # 端口updown状态
                'port_info_dict':{},
                # 调试信息
                'other_info': '', 
                #  测试结果
                'test_result': True, 
                # updown异常的口，只有up的口会测试
                'updownerrorports': [], 
                'errorports': [], 
                # 测试成功的口
                'successports': [], 
                'test_type': 'prbs_mac', 
                # 测试结束后前台打印的信息
                'prbs_info':""
                # prbs_mac_test\n
                # port: 65  xe0(76)     Lane[0] prbs_ber: 3.23e-13 <= 1.00E-09, test success\n
        }
        self.result_dict = {
            "prbs_result_dict":{},
            "prbs_mac_result_dict": {},
            "prbs_sys_result_dict": {},
            "prbs_line_result_dict": {},
        }
        self.result_dict["prbs_result_dict"] = copy.deepcopy(result_dist)
        self.result_dict["prbs_mac_result_dict"] = copy.deepcopy(result_dist)
        self.result_dict["prbs_sys_result_dict"] = copy.deepcopy(result_dist)
        self.result_dict["prbs_sys_result_dict"]["test_type"] = "prbs_sys"
        self.result_dict["prbs_line_result_dict"] = copy.deepcopy(result_dist)
        self.result_dict["prbs_line_result_dict"]["test_type"] = "prbs_line"

        other_info = ""

        # 传入port_list=[] 或 不传入port_list或者入参异常，测试全部端口
        # 传入port_list=[7,8,9,10], 测试面板口7, 8, 9, 10
        if port_list is None or len(port_list) == 0:
            port_list = []
        elif not isinstance(port_list[0], int):
            log_info = "not find port in port_list:%s\n" % port_list
            # result_dict["other_info"] += log_info
            log.error(log_info)
            port_list = []
        self.port_list = copy.deepcopy(port_list)  # 防止传入为对象时，对传入对象进行操作
        if len(self.port_list) == 0:
            for i in range(len(self.device_port_list)):
                self.port_list.append(i + 1)
        log.info("port_list %s" % self.port_list)

        run_step = [
            # 拓扑配置
            self.test_setup,
            # 发包
            self.test_start,
            # 报文校验
            self.test_verify,
        ]
        for _ in range(self.retry_times):
            try:
                # 获取端口up状态
                ret_t, other_info = self.get_port_test_info()
                log.debug("up_port: %s" % self.upports)
                if (ret_t < 0):
                    log_info = "get_port_status_f ret_t < 0\n"
                    other_info += log_info
                    log.error(log_info)
                    continue
                elif len(self.upports) == 0:
                    log_info = "get_port_status_f len(upports) == 0\n"
                    other_info += log_info
                    log.error(log_info)
                    continue

                # 串行测试
                if self.prbs_proc == PRBS_PROC_SERIAL and self.test_type == "prbs_all":
                    prbs_step = ["prbs", "prbs_mac", "prbs_sys", "prbs_line"]
                    for prbs_type in prbs_step:
                        for step_fun in run_step:
                            ret, log_info = step_fun(test_type=prbs_type)
                            # print(f"Step {step_fun.__name__} completed with result: {ret}, log_info: {log_info}")
                            if ret != RST_SUCCESS:
                                ret_t = ret
                                other_info += log_info
                                log.error(log_info)
                                break  # 开始整个测试项重试
                        if ret_t != RST_SUCCESS:
                            break
                else:
                    for step_fun in run_step:
                        ret, log_info = step_fun(test_type=self.test_type)
                        # print(f"Step {step_fun.__name__} completed with result: {ret}, log_info: {log_info}")
                        if ret != RST_SUCCESS:
                            ret_t = ret
                            other_info += log_info
                            log.error(log_info)
                            break  # 开始整个测试项重试

            except Exception:
                msg = traceback.format_exc()
                print("Exception_info:\n%s" % msg)
                ret_t = -999
                continue
            # 即使在 try 块中调用了 return、break 或者 continue，finally 块仍然会执行
            finally:
                ret, _ = self.test_cleanup(test_type=self.test_type)
                if ret != RST_SUCCESS:
                    ret_t = ret
                time.sleep(self.cleanup_time)
                # 测试成功才执行端口比较
                if ret_t == RST_SUCCESS:
                    ret_t, log_info = self.compare_start_end_ports()

            # 测试失败重测
            if ret_t != RST_SUCCESS:
                continue
            # 有端口不up 重测
            if len(self.upports) != len(self.port_list):
                log.warning("Some ports are down, retrying test...")
                ret_t = RST_PORT_STATUS_ERR
            else :
                break
        
        for port in range(len(self.port_list)):
            if port in self.prbs_port_list:
                self.result_dict["prbs_result_dict"]["port_info_dict"][port] = self.port_info_dict[port]
                if port not in self.upports:
                    self.result_dict["prbs_result_dict"]["updownerrorports"].append(port)
                    self.result_dict["prbs_result_dict"]["test_result"] = False
            if port in self.prbs_port_list_ext_phy:
                self.result_dict["prbs_mac_result_dict"]["port_info_dict"][port] = self.port_info_dict[port]
                self.result_dict["prbs_sys_result_dict"]["port_info_dict"][port] = self.port_info_dict[port]
                self.result_dict["prbs_line_result_dict"]["port_info_dict"][port] = self.port_info_dict[port]
                if port not in self.upports:
                    self.result_dict["prbs_mac_result_dict"]["updownerrorports"].append(port)
                    self.result_dict["prbs_mac_result_dict"]["test_result"] = False
                    self.result_dict["prbs_sys_result_dict"]["updownerrorports"].append(port)
                    self.result_dict["prbs_sys_result_dict"]["test_result"] = False
                    self.result_dict["prbs_line_result_dict"]["updownerrorports"].append(port)
                    self.result_dict["prbs_line_result_dict"]["test_result"] = False

        return self.result_dict

class CTCPortTestEntry():
    u'''测试入口'''
    @staticmethod
    def port_frame_test(port_list=[], redirect=True, mac_lb=False):
        test = PortFrameTestCTC()
        return test.test_run(port_list, redirect, mac_lb)

    @staticmethod
    def port_brcst_test(port_list=[], redirect=True):
        test = PortBrcstTestCTC()
        return test.test_run(port_list, redirect)

    @staticmethod
    def port_prbs_test(port_list=[], test_type="", redirect=True):
        test = PortPrbsTestCTC()
        return test.test_run(port_list, test_type, redirect)

class AliasedGroup(click.Group):
    u'''命令别名'''

    def get_command(self, ctx, cmd_name):
        u'''获取命令'''
        get_rv = click.Group.get_command(self, ctx, cmd_name)
        if get_rv is not None:
            return get_rv
        matches = [x for x in self.list_commands(ctx)
                   if x.startswith(cmd_name)]
        if matches:
            if len(matches) == 1:
                return click.Group.get_command(self, ctx, matches[0])
            ctx.fail('Too many matches: %s' % ', '.join(sorted(matches)))
        return None

CONTEXT_SETTINGS = dict(help_option_names=['-h', '--help'])
@click.group(cls=AliasedGroup, context_settings=CONTEXT_SETTINGS)
def main():
    u'''ctc port test'''
    return None

@main.command()
def port_frame_test():
    """port frame test."""
    test = PortFrameTestCTC()
    # port_list = [1,2,3,4]
    port_list = []
    result_dict = test.test_run(port_list = port_list)
    # if result_dict.get("test_result", False) is False:
    #     test.show_config()
    # formatted_dict = json.dumps(result_dict, indent=4, sort_keys=True)
    # print("result_dict:%s" % formatted_dict)
    print("successports:%s" % result_dict["successports"])
    print("errorports:%s" % result_dict["errorports"])
    print("updownerrorports:%s" % result_dict["updownerrorports"])
    print("test_result:%s" % result_dict["test_result"])
    if result_dict["test_result"] is not True:
        print("other_info:%s" % result_dict["other_info"])

@main.command()
def port_brcst_test():
    """port brcst test."""
    test = PortBrcstTestCTC()
    # port_list = [1,2,3,4]
    port_list = []
    result_dict = test.test_run(port_list = port_list)
    # if result_dict.get("test_result", False) is False:
    #     test.show_config()
    # formatted_dict = json.dumps(result_dict, indent=4, sort_keys=True)
    # print("result_dict:%s" % formatted_dict)
    print("successports:%s" % result_dict["successports"])
    print("errorports:%s" % result_dict["errorports"])
    print("updownerrorports:%s" % result_dict["updownerrorports"])
    print("test_result:%s" % result_dict["test_result"])
    if result_dict["test_result"] is not True:
        print("other_info:%s" % result_dict["other_info"])

@main.command()
def port_prbs_test():
    """port prbs test."""
    test = PortPrbsTestCTC()
    port_list = []
    # test.show_config()
    result_dict = test.test_run(port_list = port_list)
    # if result_dict.get("test_result", False) is False:
    #     test.show_config()
    # formatted_dict = json.dumps(result_dict, indent=4, sort_keys=True)
    # print("result_dict:%s" % formatted_dict)
    if len(result_dict["prbs_result_dict"]["updownerrorports"]) :
        print("updownerrorports prbs:%s" % result_dict["prbs_result_dict"]["updownerrorports"])
    if len(result_dict["prbs_mac_result_dict"]["updownerrorports"]) :
        print("updownerrorports prbs mac:%s" % result_dict["prbs_mac_result_dict"]["updownerrorports"])
    # if len(result_dict["prbs_sys_result_dict"]["updownerrorports"]) :
    #     print("updownerrorports prbs sys:%s" % result_dict["prbs_sys_result_dict"]["updownerrorports"])
    # if len(result_dict["prbs_line_result_dict"]["updownerrorports"]) :
    #     print("updownerrorports prbs line:%s" % result_dict["prbs_line_result_dict"]["updownerrorports"])

    print("%s" % result_dict["prbs_result_dict"]["prbs_info"])
    print("%s" % result_dict["prbs_mac_result_dict"]["prbs_info"])
    print("%s" % result_dict["prbs_sys_result_dict"]["prbs_info"])
    print("%s" % result_dict["prbs_line_result_dict"]["prbs_info"])
    if result_dict["prbs_result_dict"]["test_result"] is not True:
        print(result_dict["prbs_result_dict"]["other_info"])
    if result_dict["prbs_mac_result_dict"]["test_result"] is not True:
        print(result_dict["prbs_mac_result_dict"]["other_info"])
    if result_dict["prbs_sys_result_dict"]["test_result"] is not True:
        print(result_dict["prbs_sys_result_dict"]["other_info"])
    if result_dict["prbs_line_result_dict"]["test_result"] is not True:
        print(result_dict["prbs_line_result_dict"]["other_info"])


if __name__ == '__main__':
    main()