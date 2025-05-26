#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
import time
import os
import re
import logging
import syslog
import subprocess
import traceback
import click
from wbutil.baseutil import get_machine_info
from wbutil.baseutil import get_platform_info
from faclib.config.facconfig import TESTCASE
from faclib.config.facconfig import sdk_cmd_lock
from faclib.port.portutil import PortUtil


MACHINE_FILE = "/host/machine.conf"
MACHINE_PLATFORM_PREDIX = "onie_platform="
SYSLOG_IDENTIFIER = "PORT"
global_onie_platform = ""

# True: 同时将cmd打印出来(默认False)
cmd_also_print_to_console = False
# True: 同时将cmd执行结果打印出来(默认False)
cmd_output_also_print_to_console = False
# True: 开启输入重定向  cmd + < /dev/null(默认True)
sdk_cmd_redirect_console = True

global_mgmt_kr_ports = {
    "x86_64-ruijie_b6510-48vs8cq-r0": {"eth1": 66, "eth2": 130},
    "x86_64-ruijie_b6510-32cq-r0": {"eth1": 66, "eth2": 130},
    "x86_64-ruijie_b6520-64cq-r0": {"eth1": 66, "eth2": 100},
    "x86_64-ruijie_b6920-4c-r0": {"eth1": 38, "eth2": 118},
    "x86_64-ruijie_b6920-4c-v3-r0": {"eth1": 118, "eth2": 38},
    "x86_64-ruijie_bt2575-r0": {"eth1": 66, "eth2": 130},
    "x86_64-tencent_tcs81-100f-r0": {"eth1": 66, "eth2": 130},
    "x86_64-tencent_tcs82-100f-r0": {"eth1": 66, "eth2": 130},
    "x86_64-tencent_tcs83-100f-r0": {"eth1": 38, "eth2": 118}
}

global_extphy_newcmd_list = {
    "x86_64-ruijie_b6920-4c-v3-r0" : True,
}

global_special_kr_device = {
    "x86_64-ragile_ra-b6920-32qc2x-r0" : True,
    "x86_64-ruijie_ws9851-32dq-r0" : True,
    "x86_64-micas_m2-w6920-32qc2x-r0" : True,
}

global_special_kr_device_prbs_ber = {
    "x86_64-ragile_ra-b6920-32qc2x-r0" : 1.0e-7
}

global_show_prefix_config = {
    "x86_64-micas_m2-w6510-48gt4v-r0" : {
        "ge":"G",
        "xe":"CLMIB_",
    },
    "x86_64-ruijie_b6920-4c-v3-r0" : {
        "ce":"CDMIB_",
        "xe":"CLMIB_",
    }
}

global_prepare_before_tx_broadcast = {
    "x86_64-micas_m2-w6510-48gt4v-r0": True
}

global_esw_sdkcmd_prbs_support = [
    "x86_64-micas_m2-w6510-48gt4v-r0",
    "x86_64-ruijie_ws9851-32dq-r0",
    "x86_64-micas_m2-w6920-32qc2x-r0"
]

CONTEXT_SETTINGS = dict(help_option_names=['-h', '--help'])


class AliasedGroup(click.Group):
    def get_command(self, ctx, cmd_name):
        rv = click.Group.get_command(self, ctx, cmd_name)
        if rv is not None:
            return rv
        matches = [x for x in self.list_commands(ctx)
                   if x.startswith(cmd_name)]
        if not matches:
            return None
        elif len(matches) == 1:
            return click.Group.get_command(self, ctx, matches[0])
        ctx.fail('Too many matches: %s' % ', '.join(sorted(matches)))


def Singleton(cls):
    _instance = {}

    def _singleton(*args, **kargs):
        if cls not in _instance:
            _instance[cls] = cls(*args, **kargs)
        return _instance[cls]

    return _singleton


def log_info(msg, also_print_to_console=False):
    syslog.openlog(SYSLOG_IDENTIFIER)
    syslog.syslog(syslog.LOG_INFO, msg)
    syslog.closelog()
    if also_print_to_console:
        click.echo(msg)


def log_debug(msg, also_print_to_console=False):
    try:
        syslog.openlog(SYSLOG_IDENTIFIER)
        syslog.syslog(syslog.LOG_DEBUG, msg)
        syslog.closelog()

        if also_print_to_console:
            click.echo(msg)
    except Exception as e:
        print(e)
        pass


def log_warning(msg, also_print_to_console=False):
    syslog.openlog(SYSLOG_IDENTIFIER)
    syslog.syslog(syslog.LOG_WARNING, msg)
    syslog.closelog()

    if also_print_to_console:
        click.echo(msg)


def log_error(msg, also_print_to_console=False):
    syslog.openlog(SYSLOG_IDENTIFIER)
    syslog.syslog(syslog.LOG_ERR, msg)
    syslog.closelog()


def baresdk_check():
    # 0:bcmcmd,td3/td4  1:bcmcmdb,td4/th4/th5  2:bcmcmdb,td3/th3
    version = TESTCASE.get("sdkcmdversion", 0) 
    if version == 0:
        return False
    else:
        return True


class CommandExecutor:
    last_command = None
    last_status = None
    last_output = None

    @classmethod
    def sys_cmd(cls, cmd):
        """Execute a system command and return the status and output."""
        try:
            cls.last_command = cmd  # 记录最后一次执行的命令
            log_debug(cmd)
            cls.last_status, cls.last_output = subprocess.getstatusoutput(cmd)
        except Exception:
            msg = traceback.format_exc()
            print("Exception_info:\n%s" % msg)
            log_error("cmd:%s" % cmd)
        return cls.last_status, cls.last_output

def sys_cmd(cmd):
    return CommandExecutor.sys_cmd(cmd)

def get_last_cmd():
    return CommandExecutor.last_command

def get_last_cmd_status():
    return CommandExecutor.last_status

def get_last_cmd_output():
    return CommandExecutor.last_output

def sdkcmdb_os_system(cmd):
    with sdk_cmd_lock:
        return sys_cmd(cmd)

def port_getstatusoutput_one_time(cmd, time_sleep=0):
    u'''获取命令的执行结果一次'''

    if baresdk_check():
        ret, output = sdkcmdb_os_system(cmd)
    else:
        ret, output = commands.getstatusoutput(cmd)

    t = int(time_sleep)
    if cmd_also_print_to_console:
        print(cmd)
        log_debug(cmd)
    if cmd_output_also_print_to_console:
        print(output)
        log_debug(output)
    if t != 0:
        log_debug("time sleep: %ds" % t)
        time.sleep(t)
    if ret == 0:
        if (re.search('hsdk msg connect fail', output.strip())):
            return -1, output
        return ret, output
    else:
        output_content = "cmd: %s execution fail, output:%s" % (cmd, output)
        return_output_content = output_content.replace("\"", "'")
        log_error("%s" % output_content)
        return ret, "%s" % return_output_content


def port_checkstatusoutput(output):
    if (re.search('cmicx_sbusdma_error', output.strip())):
        return False
    return True


def port_getstatusoutput(cmd, time_sleep=0):
    u'''获取命令的执行结果'''

    ret = -1
    output = ""
    retry_time = 0
    for i in range(3):
        retry_time = i + 1
        ret, output = port_getstatusoutput_one_time(cmd, time_sleep)
        if ret != 0:
            break
        if port_checkstatusoutput(output) is False:
            log_error("[%d] cmd:%s output:%s\n port_checkstatusoutput fail" % (retry_time, cmd, output))
            if "tx" in cmd and "DestMac" in cmd:
                log_debug("cmd: %s not need retry" % (cmd))
                break
            time.sleep(5)
            continue
        break
    return ret, output


def get_sdk_cmd(cmd, time_out=0, grep=""):
    u'''对传入的cmd进行组合, 返回sdk_cmd'''

    cmd_str = ""
    sdk_cmd = ""
    t = int(time_out)
    if (baresdk_check()):
        sdk_cmd = "bcmcmdb "
    else:
        sdk_cmd = "bcmcmd "
    if t != 0:
        cmd_str = sdk_cmd + "-t %d \"" % t
    else:
        cmd_str = sdk_cmd + "\""
    if sdk_cmd_redirect_console == True:
        cmd_str = cmd_str + str(cmd) + "\" < /dev/null"
    else:
        cmd_str = cmd_str + str(cmd) + "\""
    if grep != "":
        cmd_str += " | grep \"%s\"" % str(grep)
    return cmd_str


class PortKrTest(object):

    def __init__(self):
        self.__install_pktgen_mode()

    def get_onie_platform(self):
        with open(MACHINE_FILE, "r") as machine_f:
            for line in machine_f:
                if(re.search('%s(.*?)$' % MACHINE_PLATFORM_PREDIX, line)):
                    onie_platform = re.findall(r"%s(.*?)$" % MACHINE_PLATFORM_PREDIX, line)[0]
                    return onie_platform
        return None

    def __install_pktgen_mode(self):
        cmd = "lsmod | grep pktgen"
        ret, output = port_getstatusoutput(cmd)
        if ((not output) or (ret != 0)):
            cmd = "modprobe pktgen"
            ret, output = port_getstatusoutput(cmd)
        cmd = "ls /proc/net/pktgen/eth1"
        ret, output = port_getstatusoutput(cmd)
        if (("cannot" in output) or (ret != 0)) :
            cmd = "ifconfig eth1 up"
            ret, output = port_getstatusoutput(cmd)
            time.sleep(1)
            cmd = "echo \"add_device eth1\" > /proc/net/pktgen/kpktgend_0"
            ret, output = port_getstatusoutput(cmd)
        cmd = "ls /proc/net/pktgen/eth2"
        ret, output = port_getstatusoutput(cmd)
        if (("cannot" in output) or (ret != 0)) :
            cmd = "ifconfig eth2 up"
            ret, output = port_getstatusoutput(cmd)
            time.sleep(1)
            cmd = "echo \"add_device eth2\" > /proc/net/pktgen/kpktgend_0"
            ret, output = port_getstatusoutput(cmd)
        time.sleep(3)

    def get_mgmt_bcmport(self, port):
        cmd = get_sdk_cmd("ps")
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        logic_port = global_mgmt_kr_ports[self.get_onie_platform()][port]
        for line in lines:
            line.strip()
            if re.search(r"^.*?\(.*?\)", line) and int(re.findall(r"^.*?\((.*?)\)",
                                                                  line)[0].strip()) == logic_port:
                return re.findall(r"^(.*?)\(.*?\)", line)[0].strip()
        return None

    def start_send_port_packets(self, port, count=10000, size=64, dst_mac="ff:ff:ff:ff:ff:ff", vlan=4080):
        bcm_port = self.get_mgmt_bcmport(port)
        time.sleep(1)
        cmd = get_sdk_cmd("vlan destroy %d" % vlan)
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("vlan create %d PortBitMap=%s UntagBitMap=%s" % (vlan, bcm_port, bcm_port))
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("pvlan set %s %d" % (bcm_port, vlan))
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        cmd = "echo \"pkt_size %d\" > /proc/net/pktgen/%s" % (size, port)
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "echo \"count %d\" > /proc/net/pktgen/%s" % (count, port)
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "echo \"dst_mac %s\" > /proc/net/pktgen/%s" % (dst_mac, port)
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "echo \"vlan_id %s\" > /proc/net/pktgen/%s" % (vlan, port)
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("clear c %s" % bcm_port)
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        cmd = "echo \"start\" > /proc/net/pktgen/pgctrl"
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(2)
        cmd = "echo \"stop\" > /proc/net/pktgen/pgctrl"
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("vlan remove %d PortBitMap=%s" % (vlan, bcm_port))
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "cat /proc/net/pktgen/%s" % port
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        return True, output

    def clear_port_packets(self):
        for port in global_mgmt_kr_ports[self.get_onie_platform()]:
            cmd = get_sdk_cmd("clear c %s" % self.get_mgmt_bcmport(port))
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0):
                return False, output
        return True, output

    def check_port_packets(self, port, count=10000):
        cmd = get_sdk_cmd("show c XLMIB_RPOK.%s" % self.get_mgmt_bcmport(port))
        ret, output = port_getstatusoutput(cmd)
        # print output
        lines = output.split("\n")
        if(lines[1].strip()):
            if (count == int(re.sub('[,]', '', lines[1].split()[2].strip("+-")))):
                return True, lines[1].split()[2]
            else:
                return False, lines[1].split()[2]
        return False, "fail"


@Singleton
class PortTest(object):
    __mode = 0

    interfaces = []
    porttabfile = " "
    bcm_ports = []
    unit_ports = []
    logic_ports = []
    mac_output = []
    standard = 0
    exist_400G_flag = False
    showc_prefix = None

    def __init__(self, mode="sdk"):
        self.exist_400G_flag = False
        if global_onie_platform == "":
            self.get_onie_platform()
        self.showc_prefix = global_show_prefix_config.get(global_onie_platform, None)
        self.prepare_before_tx_broadcast = global_prepare_before_tx_broadcast.get(global_onie_platform, None)
        self.__install_pktgen_mode()
        self.__get_global_interfaces()
        self.__get_global_bcmports()
        self.__get_global_unitports()
        self.__get_prbs_ber()
        self.__check_400G_port_exist()
        if(mode == "sonic"):
            self.__mode = 1
        else:
            self.__mode = 0

    @staticmethod
    def get_onie_platform():
        u'''获取onie_platform'''

        log_debug("获取onie_platform")
        with open(MACHINE_FILE, "r") as machine_f:
            for line in machine_f:
                if(re.search('%s(.*?)$' % MACHINE_PLATFORM_PREDIX, line)):
                    onie_platform = re.findall(r"%s(.*?)$" % MACHINE_PLATFORM_PREDIX, line)[0]
                    global global_onie_platform
                    global_onie_platform = onie_platform
                    log_debug("onie_platform:%s" % global_onie_platform)
                    return onie_platform
        return None

    def __install_pktgen_mode(self):
        cmd = "lsmod | grep pktgen"
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        if(not lines):
            cmd = "modprobe pktgen"
            ret, output = port_getstatusoutput(cmd)

    def __get_global_interfaces(self):
        if(len(self.interfaces)):
            #print("inited interfaces")
            pass
        else:
            cmd = "show interfaces status"
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            for line in lines:
                line.strip()
                if(len(line.split()) and re.search("Ethernet[0-9]+", line.split()[0])):
                    self.interfaces.append(line.split()[0])

    def __get_global_bcmports(self):
        if(len(self.bcm_ports)):
            pass
        else:
            pu = PortUtil()
            cmd = get_sdk_cmd("ps")
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            for port in pu.device_port_list:
                for line in lines:
                    line.strip()
                    if re.search(r"^.*?\(.*?\)", line) and int(re.findall(r"^.*?\((.*?)\)", line)
                                                               [0].strip()) == port.logic_port:
                        self.bcm_ports.append(re.findall(r"^(.*?)\(.*?\)", line)[0].strip())
                        self.logic_ports.append(port.logic_port)
                        break
            log_debug(' '.join(str(i) for i in self.bcm_ports))

    def __get_global_unitports(self):
        if(len(self.unit_ports)):
            pass
        else:
            pu = PortUtil()
            cmd = get_sdk_cmd("ps")
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            for port in pu.device_port_list:
                for line in lines:
                    line.strip()
                    if re.search(r"^.*?\(.*?\)", line) and int(re.findall(r"^.*?\((.*?)\)", line)[0].strip()) == port.logic_port:
                        self.unit_ports.append(int((line.split(")")[0]).split("(")[1]))
                        break

    def __get_prbs_ber(self):
        self.standard = global_special_kr_device_prbs_ber.get(self.get_onie_platform(), 1.0e-7)

    def __check_400G_port_exist(self):
        cmd = get_sdk_cmd("ps")
        ret, output = port_getstatusoutput(cmd)
        if ret == 0:
            if "400G" in output:
                self.exist_400G_flag = True

    def __get_port_status_by_sonic(self, port):
        cmd = "show int status %s" % self.interfaces[port - 1]
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        return lines[-2].split()[5]

    def __get_port_status_by_sdk(self, port):
        cmd = get_sdk_cmd("ps %s" % self.bcm_ports[port - 1])
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        if(re.search("up", lines[3])):
            return "up"
        elif(re.search("!ena", lines[3])):
            return "!ena"
        else:
            return "down"

    def __get_unit_port_by_bcm(self, port):
        cmd = get_sdk_cmd("ps %s" % self.bcm_ports[port - 1])
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        unit_port = int(re.findall(r"%s\((.*?)\)" % self.bcm_ports[port - 1], lines[3])[0].strip())
        return unit_port

    def __start_send_port_packets_by_sonic(self, port, count, size=64, dst_mac="ff:ff:ff:ff:ff:ff"):
        cmd = "echo \"add_device %s\" > /proc/net/pktgen/kpktgend_0" % self.interfaces[port - 1]
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "echo \"pkt_size %d\" > /proc/net/pktgen/%s" % (size, self.interfaces[port - 1])
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "echo \"count %d\" > /proc/net/pktgen/%s" % (count, self.interfaces[port - 1])
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "echo \"dst_mac %s\" > /proc/net/pktgen/%s" % (dst_mac, self.interfaces[port - 1])
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "echo \"start\" > /proc/net/pktgen/pgctrl"
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = "cat /proc/net/pktgen/%s" % self.interfaces[port - 1]
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        return True, output

    def __start_send_port_packets_by_sdk(self, port, count, size=64, dst_mac="ff:ff:ff:ff:ff:ff"):
        cmd = get_sdk_cmd("pbmp %s" % self.bcm_ports[port - 1])
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        pbmp = re.findall("0x[0-9]+", lines[1].strip())[0]
        cmd = get_sdk_cmd("tx %d VLantag=1 TXUnit=0 PortBitMap=%s Length=%d DestMac=%s" % (count, pbmp, size, dst_mac))
        ret, output = port_getstatusoutput(cmd)
        log_debug(output)
        if(ret != 0):
            return False, output
        return True, output

    def __clear_port_packets_by_sonic(self):
        cmd = "show interfaces counters -c"
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        return True, output

    def __clear_port_packets_by_sdk(self):
        cmd = get_sdk_cmd("clear c")
        ret, output = port_getstatusoutput(cmd)
        log_debug(output)
        if(ret != 0):
            return False, output
        return True, output

    def __check_port_packets_by_sonic(self, port, count, direc="tx"):
        cmd = "show interfaces counters"
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        for line in lines:
            line.strip()
            if(len(line.split()) and re.search("Ethernet[0-9]+", line.split()[0])):
                if(self.interfaces[port - 1] == line.split()[0]):
                    if(direc == "tx"):
                        if(count == int(re.sub('[,]', '', line.split()[9]))):
                            return True, line.split()[9]
                        else:
                            #print("packets num = %s" % int(re.sub('[,]', '', line.split()[9])))
                            return False, line.split()[9]
                    elif(direc == "rx"):
                        if(count == int(re.sub('[,]', '', line.split()[2]))):
                            return True, line.split()[9]
                        else:
                            #print("packets num = %s" % int(re.sub('[,]', '', line.split()[2])))
                            return False, line.split()[9]
        return False, "fail"

    def __check_is_special_kr_device(self):
        special_kr_device = False
        if global_special_kr_device.get(self.get_onie_platform(), False) is True:
            special_kr_device = True
        return special_kr_device

    def __check_port_is_special_kr(self, port):
        if self.__check_is_special_kr_device() is False:
            return False
        bcm_port = self.bcm_ports[port - 1]
        if "xe" in bcm_port:
            return True
        return False

    def __check_port_packets_by_sdk(self, port, count, direc="tx"):
        if(direc == "tx"):
            if self.showc_prefix is not None:
                prefix = self.showc_prefix.get(re.search(r'[a-zA-Z]+', self.bcm_ports[port - 1]).group(0), None)
                if prefix is not None:
                    showpkt = prefix + "TPKT"
                    show1518 = prefix + "T1518"
                else:
                    log_error("get prefix failed. port %s[%s],dict %s" % (self.bcm_ports[port - 1], \
                            re.search(r'[a-zA-Z]+', self.bcm_ports[port - 1]).group(0), str(self.showc_prefix)))
                    return False, "prefix fail"
            else:
                cmd = get_sdk_cmd("show c CLMIB_TPOK")
                ret, output = port_getstatusoutput(cmd)
                if re.search(r"failed", output) or re.search(r"error", output):
                    showpkt = "CDMIB_TPKT"
                    show1518 = "CDMIB_T1518"
                else:
                    showpkt = "CLMIB_TPKT"
                    show1518 = "CLMIB_T1518"
                if self.__check_port_is_special_kr(port) is True:
                    showpkt = "XLMIB_TPKT"
                    show1518 = "XLMIB_T1518"
            cmd = get_sdk_cmd("show c %s.%s" % (showpkt, self.bcm_ports[port - 1]))
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            count_pkt = int(re.sub('[,]', '', lines[1].split()[2].strip("+-")))
            log_debug(' '.join(str(s) for s in lines))

            cmd = get_sdk_cmd("show c %s.%s" % (show1518, self.bcm_ports[port - 1]))
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            count_1518 = int(re.sub('[,]', '', lines[1].split()[2].strip("+-")))
            log_debug(' '.join(str(s) for s in lines))
            if(lines[1].strip()):
                if(count == count_pkt or count == count_1518):
                    return True, lines[1].split()[2]
                else:
                    return False, lines[1].split()[2]
        elif(direc == "rx"):
            if self.showc_prefix is not None:
                prefix = self.showc_prefix.get(re.search(r'[a-zA-Z]+', self.bcm_ports[port - 1]).group(0), None)
                if prefix is not None:
                    showpkt = prefix + "TPKT"
                    show1518 = prefix + "T1518"
                else:
                    log_error("get prefix failed. port %s[%s],dict %s" % (self.bcm_ports[port - 1], \
                            re.search(r'[a-zA-Z]+', self.bcm_ports[port - 1]).group(0), str(self.showc_prefix)))
                    return False, "prefix fail"
            else:
                cmd = get_sdk_cmd("show c CLMIB_RPOK")
                ret, output = port_getstatusoutput(cmd)
                if re.search(r"failed", output) or re.search(r"error", output):
                    showpkt = "CDMIB_RPKT"
                    show1518 = "CDMIB_R1518"
                else:
                    showpkt = "CLMIB_RPKT"
                    show1518 = "CLMIB_R1518"
                if self.__check_port_is_special_kr(port) is True:
                    showpkt = "XLMIB_RPKT"
                    show1518 = "XLMIB_R1518"
            cmd = get_sdk_cmd("show c %s.%s" % (showpkt, self.bcm_ports[port - 1]))
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            count_pkt = int(re.sub('[,]', '', lines[1].split()[2].strip("+-")))
            log_debug(' '.join(str(s) for s in lines))

            cmd = get_sdk_cmd("show c %s.%s" % (show1518, self.bcm_ports[port - 1]))
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            count_1518 = int(re.sub('[,]', '', lines[1].split()[2].strip("+-")))
            log_debug(' '.join(str(s) for s in lines))
            if(lines[1].strip()):
                if(count == count_pkt or count == count_1518):
                    return True, lines[1].split()[2]
                else:
                    return False, lines[1].split()[2]
        return False, "fail"

    ################################################################################################
    def get_fiber_bcmports(self):
        fiber_bcmports = list()
        for bcmport in self.bcm_ports:
            if ("ge" in bcmport):
                continue
            else:
                fiber_bcmports.append(bcmport)
        return fiber_bcmports

    def get_copper_bcmports(self):
        copper_bcmports = list()
        for bcmport in self.bcm_ports:
            if ("ge" in bcmport):
                copper_bcmports.append(bcmport)
        return copper_bcmports

    def get_port_status(self, port):
        if(self.__mode == 0):
            return self.__get_port_status_by_sdk(port)
        elif(self.__mode == 1):
            return self.__get_port_status_by_sonic(port)

    def get_port_fcs_status(self, port):
        tfcs = 0
        rfcs = 0
        if self.showc_prefix is not None:
            prefix = self.showc_prefix.get(re.search(r'[a-zA-Z]+', self.bcm_ports[port - 1]).group(0), None)
            if prefix is not None:
                showc = prefix + "TFCS"
            else:
                log_error("get prefix failed. port %s[%s],dict %s" % (self.bcm_ports[port - 1], \
                        re.search(r'[a-zA-Z]+', self.bcm_ports[port - 1]).group(0), str(self.showc_prefix)))
                return False, "prefix fail"
        else:
            cmd = get_sdk_cmd("show c CLMIB_TPOK")
            ret, output = port_getstatusoutput(cmd)
            if re.search(r"failed", output) or re.search(r"error", output) :
                showc = "CDMIB_TFCS"
            else:
                showc = "CLMIB_TFCS"
        cmd = get_sdk_cmd("show c %s.%s" % (showc, self.bcm_ports[port - 1]))
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        if lines[1].strip():
            tfcs = int(re.sub('[,]', '', lines[1].split()[2]))
        if self.showc_prefix is not None:
            prefix = self.showc_prefix.get(re.search(r'[a-zA-Z]+', self.bcm_ports[port - 1]).group(0), None)
            if prefix is not None:
                showc = prefix + "RFCS"
            else:
                log_error("get prefix failed. port %s[%s],dict %s" % (self.bcm_ports[port - 1], \
                        re.search(r'[a-zA-Z]+', self.bcm_ports[port - 1]).group(0), str(self.showc_prefix)))
                return False, "prefix fail"
        else:
            cmd = get_sdk_cmd("show c CLMIB_RPOK")
            ret, output = port_getstatusoutput(cmd)
            if re.search(r"failed", output) or re.search(r"error", output):
                showc = "CDMIB_RFCS"
            else:
                showc = "CLMIB_RFCS"
        cmd = get_sdk_cmd("show c %s.%s" % (showc, self.bcm_ports[port - 1]))
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        if lines[1].strip():
            rfcs = int(re.sub('[,]', '', lines[1].split()[2]))

        if(tfcs == 0 and rfcs == 0):
            return True, (tfcs, rfcs)
        # print "port = %d, tfcs = %d rfcs = %d" % (port, tfcs, tfcs)
        return False, (tfcs, rfcs)

    def __prepare_before_tx_broadcast(self):
        self.prepare_before_tx_broadcast = False
        cmd = get_sdk_cmd("stg stp 1 all forward")
        ret, output = port_getstatusoutput(cmd, time_sleep=1)
        cmd = get_sdk_cmd("pvlan set all 1")
        ret, output = port_getstatusoutput(cmd, time_sleep=1)
        cmd = get_sdk_cmd("vlan add 1 PortBitMap=all UntagBitMap=all")
        ret, output = port_getstatusoutput(cmd, time_sleep=1)

    def __get_split_index_dict(self, input, split_key):
        index_dict = dict()
        pre_space_index = -2
        for index in range(len(input)):
            if input[index] != split_key and pre_space_index == -1:
                pre_space_index = index
                continue

            if input[index] == split_key and pre_space_index == -2:
                continue

            if input[index] != split_key and pre_space_index == -2:
                pre_space_index = 0
                continue

            if input[index] == split_key and pre_space_index != -1:
                index_dict[pre_space_index] = index
                pre_space_index = -1
                continue

            if (index == len(input)-1) and (pre_space_index != -1):
                index_dict[pre_space_index] = index + 1

        return index_dict

    def __parse_bcmcmd_ps(self, input):
        result_dict = dict()
        input_lines = input.splitlines()

        head_line = [line for line in input_lines if line.find("port")>=0][0]
        lines = [line for line in input_lines if (line.find("(")>=0 and line.find(")")>=0)]

        head_index_dict = self.__get_split_index_dict(head_line, ' ')

        lines_split_list = list()
        for line in lines:
            line_split_index = self.__get_split_index_dict(line, ' ')
            line_split_list = list()
            head_index_list = list(head_index_dict.keys())
            head_index_list.sort()
            for head_first_index in head_index_list:
                head_last_index = head_index_dict.get(head_first_index)
                line_item_str = ""
                line_index_list = list(line_split_index.keys())
                line_index_list.sort()
                for line_first_index in line_index_list:
                    line_last_index = line_split_index.get(line_first_index)
                    for line_index in range(line_first_index, line_last_index):
                        if line_index in range(head_first_index, head_last_index):
                            line_item_str = "{} {}".format(line_item_str, line[line_first_index: line_last_index])
                            break
                line_split_list.append(line_item_str.strip())
            lines_split_list.append(line_split_list)

        key = ["lport", "port", "link", "lanes", "speed", "duplex", "linkscan", "autoneg",
                "stp", "pause", "discard", "lrn ops", "interface", "max frame", "cut thru", "loopback", "encap"]
        new_line_split_list = list()
        for line in lines_split_list:
            new_line_list = list(line)

            port = line[0] # type: str
            port.replace(' ','')
            lport = int(port[port.find('(')+1:port.find(')')])
            port = port[0:port.find('(')]
            new_line_list[0] = port
            new_line_list.insert(0, lport)

            speed_duplex = line[3].split()
            new_line_list[4] = speed_duplex[0]
            new_line_list.insert(5, speed_duplex[-1])
            
            new_line_split_list.append(new_line_list)

        for line in new_line_split_list:
            result_dict[line[0]] = dict(zip(key, line))

        return result_dict

    def __down_port_reset_admin(self):
        result = True
        cmd = get_sdk_cmd("ps")
        ret, bcmcmd_ps = port_getstatusoutput(cmd, time_sleep=2)

        bcmcmd_ps_status = self.__parse_bcmcmd_ps(bcmcmd_ps)
        
        for lport, info in bcmcmd_ps_status.items():
            if info.get("link") != "up":
                cmd = "port {} en=0".format(info.get("port"))
                cmd = get_sdk_cmd(cmd)
                ret, bcmcmd_ps = port_getstatusoutput(cmd, time_sleep=2)

                cmd = "port {} en=1".format(info.get("port"))
                cmd = get_sdk_cmd(cmd)
                ret, bcmcmd_ps = port_getstatusoutput(cmd, time_sleep=2)
                result = False
        return result

    def set_port_loopback(self, enable):
        global global_onie_platform
        if global_onie_platform != "x86_64-micas_m2-w6510-48gt4v-r0":
            return True
        enable_command_list = [
                "linkscan off",
                "phy ge0-ge23 0x09 0x1800",
                "phy ge0-ge23 0x00 0x0040",
                "phy ge0-ge23 0x17 0xf7e",
                "phy ge0-ge23 0x15 0x0",
                "phy ge0-ge23 0x1e 0x028",
                "phy ge0-ge23 0x1f 0x8400",
                "phy ge0-ge23 0x1e 0x02C",
                "phy ge0-ge23 0x1f 0x4014",
                "phy ge0-ge23 0x1e 0x87",
                "phy ge0-ge23 0x1f 0x8000",
                "port ge24-ge47 loopback=phy",
                "port xe loopback=phy",
                "linkscan on"
            ]
        disable_command_list = [
                "linkscan off",
                "port ge24-ge47 loopback=none",
                "port xe loopback=none",
                "phy ge0-ge23 0x00 0x9140",
                "phy ge1 0x17 0x0D19",
                "phy ge1 0x15 0x4801",
                "phy ge1 0x17 0x0D18",
                "phy ge1 0x15 0x0D07",
                "phy ge1 0x17 0x0D19",
                "phy ge1 0x15 0xC801",
                "init port",
                "phy ge9 0x17 0x0D19",
                "phy ge9 0x15 0x4801",
                "phy ge9 0x17 0x0D18",
                "phy ge9 0x15 0x0D07",
                "phy ge9 0x17 0x0D19",
                "phy ge9 0x15 0xC801",
                "init port",
                "phy ge17 0x17 0x0D19",
                "phy ge17 0x15 0x4801",
                "phy ge17 0x17 0x0D18",
                "phy ge17 0x15 0x0D07",
                "phy ge17 0x17 0x0D19",
                "phy ge17 0x15 0xC801",
                "init port",
                "linkscan on",
                "port all en=1"
            ]
        if enable:
            command_list = enable_command_list
        else:
            command_list = disable_command_list

        for command in command_list:
            cmd = get_sdk_cmd(command)
            ret, output = port_getstatusoutput(cmd, time_sleep=0)

        if enable == True:
            loopback_retry_count = 0
            while (loopback_retry_count < 5):
                admin_retry_count = 0
                while admin_retry_count < 3:
                    if (self.__down_port_reset_admin() == True):
                        break
                    admin_retry_count = admin_retry_count + 1
                    log_warning("port down, try to fix({}).".format(admin_retry_count))

                if (admin_retry_count < 3):
                    return True
                else:
                    loopback_retry_count = loopback_retry_count + 1
                    for command in disable_command_list:
                        cmd = get_sdk_cmd(command)
                        ret, output = port_getstatusoutput(cmd, time_sleep=0)
                    time.sleep(1)
                    for command in enable_command_list:
                        cmd = get_sdk_cmd(command)
                        ret, output = port_getstatusoutput(cmd, time_sleep=0)
            return False

        else:
            return True
                 
        return True

    def start_send_port_packets(self, port, count, size=64, dst_mac="ff:ff:ff:ff:ff:ff"):
        if self.prepare_before_tx_broadcast == True:
            self.__prepare_before_tx_broadcast()
        if(self.__mode == 0):
            return self.__start_send_port_packets_by_sdk(port, count, size, dst_mac)
        elif(self.__mode == 1):
            return self.__start_send_port_packets_by_sonic(port, count, size, dst_mac)
        return False, "fail"

    def stop_send_port_packets(self):
        cmd = get_sdk_cmd("port ge,xe,ce en=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(2)
        cmd = get_sdk_cmd("port ge,xe,ce en=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(30)
        return True, output

    def clear_port_packets(self):
        if(self.__mode == 0):
            return self.__clear_port_packets_by_sdk()
        elif(self.__mode == 1):
            return self.__clear_port_packets_by_sonic()
        return False, "fail"

    def check_port_packets(self, port, count, direc="tx"):
        if(self.__mode == 0):
            return self.__check_port_packets_by_sdk(port, count, direc)
        elif(self.__mode == 1):
            return self.__check_port_packets_by_sonic(port, count, direc)
        return False, "fail"

    def init_port_cpu(self):
        u'''调用cpu.cint'''

        cmd = ""
        if baresdk_check():
            cmd = get_sdk_cmd("cint /usr/share/sonic/device/%s/cpu.cint" % global_onie_platform)
        else:
            cmd = get_sdk_cmd("cint /usr/share/sonic/platform/cpu.cint")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        log_debug("cpu_create success")
        return True, output

    def reset_port_cpu(self):
        u'''调用cpu_destroy.cint 或者原始的直接执行'''

        cmd = ""
        if baresdk_check():
            cmd = get_sdk_cmd("cint /usr/share/sonic/device/%s/cpu_destroy.cint" % global_onie_platform)
        else:
            cmd = get_sdk_cmd("cint;\r\n bcm_field_entry_destroy(0, 2048);\r\n bcm_field_group_destroy(0, 5);\r\n exit;")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        log_debug("cpu_destroy success")
        return True, output

    def init_port_prbs_new(self):
        # 获取mac侧 prbs测试结果
        cmd = get_sdk_cmd("phy diag ce,cd prbs set p=3")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(5)

        cmd = get_sdk_cmd("phy diag ce,cd prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce,cd prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce,cd prbsstat start i=120")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        time.sleep(150)

        cmd = get_sdk_cmd("phy diag ce,cd prbsstat ber")
        ret, output = port_getstatusoutput(cmd)
        self.mac_output = output
        #print("cmd:%s,\nret:%d\noutput:%s\n" % (cmd, ret, output))
        if(ret != 0 or "unlock" in output or "fail" in output ):
            return False, output

        cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
        ret, output = port_getstatusoutput(cmd)
        time.sleep(5)
        cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
            ret, output = port_getstatusoutput(cmd)

        cmd = get_sdk_cmd("cint /usr/share/sonic/platform/prbs.cint")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        return True, output

    def set_port_prbs_new(self, port, enable):
        if self.__check_port_is_special_kr(port) is False:
            log_debug("port:%s __check_port_is_special_kr is False, not need to set port prbs" % port)
            return True, "port:%s __check_port_is_special_kr is False, not need to set port prbs" % port

        unit_port = self.__get_unit_port_by_bcm(port)
        # 可能会有问题
        cmd = get_sdk_cmd("cint;\r\n set_port_prbs(%d, %d);\r\n exit;" % (unit_port, enable))
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        return True, output

    def _get_mac_side_prbsstat_ber_new(self, port):
        u'''获取mac端prbsstat_ber'''
        lines = self.mac_output.split("\n")
        prbs_ber_flag = 0
        prbs_ber_test_fail = 0
        output_content = ""
        lport = port
        unit_port = self.unit_ports[port - 1]
        bcm_port = self.bcm_ports[port - 1]
        output_content_port_info = "port:%-3d %s(%d)" % (lport, bcm_port, unit_port)

        for line in lines:
            if (re.search('.*].*e', line.strip())):
                line_bcm_port = line.split('[')[0]
                if bcm_port == line_bcm_port:
                    prbs_ber_flag = 1
                    lane_num = int((line.split(']')[0]).split('[')[1])
                    prbs_ber = line.split(' ')[-1]
                    if (float(prbs_ber) > float(self.standard)):
                        prbs_ber_test_fail = 1
                        output_content = output_content + \
                                "%-20s Lane[%d] prbs_ber:%.2e > %s, test fail\n" % (output_content_port_info, lane_num, float(prbs_ber), self.standard)
                    else:
                        output_content = output_content + \
                                "%-20s Lane[%d] prbs_ber:%.2e <= %s, test success\n" % (output_content_port_info, lane_num, float(prbs_ber), self.standard)
            elif (re.search('Nolock', line.strip())
                or re.search('NoLock', line.strip())
                or re.search('LossOfLock', line.strip())):
                line_bcm_port = line.split('[')[0]
                if bcm_port == line_bcm_port:
                    prbs_ber_flag = 1
                    prbs_ber_test_fail = 1
                    lane_num = int((line.split(']')[0]).split('[')[1])
                    lock_info = line.split(' ')[-1]
                    output_content = output_content + \
                            "%-20s Lane[%d] prbs_ber: %s\n" % (output_content_port_info, lane_num, lock_info)
        output_content += "\n"
        if prbs_ber_flag == 1 and prbs_ber_test_fail == 0:
            return True, output_content
        elif prbs_ber_flag == 1 and prbs_ber_test_fail == 1:
            return False, output_content
        else:
            output_content = "%-20s get prbs_ber fail, output:%s" % (output_content_port_info, self.mac_output)
            return False, output_content

    def get_port_prbs_result_new(self, port):
        # 非特殊的kr面板端口
        if self.__check_port_is_special_kr(port) is False:
            return self._get_mac_side_prbsstat_ber_new(port)

        # 特殊的kr面板端口
        unit_port = self.unit_ports[port - 1]
        # 可能会有问题
        cmd = get_sdk_cmd("cint;\r\n print get_port_prbs_result(%d);\r\n exit;" % unit_port)
        ret, output = port_getstatusoutput(cmd)
        time.sleep(1)
        # 可能会有问题
        cmd = get_sdk_cmd("cint;\r\n print get_port_prbs_result(%d);\r\n exit;" % unit_port)
        ret, output = port_getstatusoutput(cmd)
        lines = output.split("\n")
        status = int(re.findall(" = (.*?) ",lines[-5])[0])
        if status == 0:
            return True, ""

        return False, output

    def init_port_prbs(self):
        if self.exist_400G_flag is True:
            return self.init_port_prbs_new()
        
        cmd = ""
        if baresdk_check():
            cmd = get_sdk_cmd("cint /usr/share/sonic/device/%s/cpu_destroy.cint" % global_onie_platform)
        else:
            cmd = get_sdk_cmd("cint /usr/share/sonic/platform/prbs.cint")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        return True, output

    def reset_port_prbs(self):
        time.sleep(1)

    def set_port_prbs(self, port, enable):
        if self.exist_400G_flag is True:
            return self.set_port_prbs_new(port, enable)

        if (global_onie_platform in global_esw_sdkcmd_prbs_support):
            port_alias = self.bcm_ports[port - 1]
            if ("ge" in port_alias):
                return True, None
            if (enable == 1):
                cmd = get_sdk_cmd("phy diag {} prbs set unit=0 p=3".format(port_alias))
            else:
                cmd = get_sdk_cmd("phy diag {} prbs clear".format(port_alias))

            ret, output = port_getstatusoutput(cmd)
            if(ret != 0):
                return False, output
            return True, output
        else:
            unit_port = self.__get_unit_port_by_bcm(port)
            # 可能会有问题，需要统一使用cint文件
            #cmd = "bcmcmd \"cint;\r\n set_port_prbs(%d, %d);\r\n exit;\"" % (unit_port, enable)
            cmd = get_sdk_cmd("cint;\r\n set_port_prbs(%d, %d);\r\n exit;" % (unit_port, enable))
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0):
                return False, output
            return True, output


    def get_port_prbs_result(self, port):
        if self.exist_400G_flag is True:
            return self.get_port_prbs_result_new(port)

        if (global_onie_platform in global_esw_sdkcmd_prbs_support):
            port_alias = self.bcm_ports[port - 1]
            if ("ge" in port_alias):
                return 0
            cmd = get_sdk_cmd("phy diag {} prbs get".format(port_alias))
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0):
                return 1
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0 or "PRBS OK" not in output):
                return 1
            return 0
        else:
            unit_port = self.__get_unit_port_by_bcm(port)
            # 可能会有问题，需要统一使用cint文件
            #cmd = "bcmcmd \"cint;\r\n print get_port_prbs_result(%d);\r\n exit;\"" % unit_port
            cmd = get_sdk_cmd("cint;\r\n print get_port_prbs_result(%d);\r\n exit;" % unit_port)
            ret, output = port_getstatusoutput(cmd)
            time.sleep(1)
            # 可能会有问题，需要统一使用cint文件
            # cmd = "bcmcmd \"cint;\r\n print get_port_prbs_result(%d);\r\n exit;\"" % unit_port
            cmd = get_sdk_cmd("cint;\r\n print get_port_prbs_result(%d);\r\n exit;" % unit_port)
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            status = int(re.findall(" = (.*?) ", lines[-5])[0])
            return status



@Singleton
class PortPrbsTest(object):
    mac_output = []
    sys_output = []
    line_output = []
    bcm_ports = []
    unit_ports = []
    standard = 0
    exist_400G_flag = False

    def __init__(self, standard=1.0e-10):
        self.standard = standard
        self.__get_global_bcmports()
        self.__get_global_unitports()
        self.__check_400G_port_exist()

    def get_onie_platform(self):
        with open(MACHINE_FILE, "r") as machine_f:
            for line in machine_f:
                if(re.search('%s(.*?)$' % MACHINE_PLATFORM_PREDIX, line)):
                    onie_platform = re.findall(r"%s(.*?)$" % MACHINE_PLATFORM_PREDIX, line)[0]
                    return onie_platform
        return None

    def __get_global_bcmports(self):
        if(len(self.bcm_ports)):
            pass
        else:
            pu = PortUtil()
            cmd = get_sdk_cmd("ps")
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            for port in pu.device_port_list:
                for line in lines:
                    line.strip()
                    if re.search(r"^.*?\(.*?\)", line) and int(re.findall(r"^.*?\((.*?)\)", line)
                                                               [0].strip()) == port.logic_port:
                        self.bcm_ports.append(re.findall(r"^(.*?)\(.*?\)", line)[0].strip())
                        break

    def __get_global_unitports(self):
        if(len(self.unit_ports)):
            pass
        else:
            pu = PortUtil()
            cmd = get_sdk_cmd("ps")
            ret, output = port_getstatusoutput(cmd)
            lines = output.split("\n")
            for port in pu.device_port_list:
                for line in lines:
                    line.strip()
                    if re.search(r"^.*?\(.*?\)", line) and int(re.findall(r"^.*?\((.*?)\)", line)[0].strip()) == port.logic_port:
                        self.unit_ports.append(int((line.split(")")[0]).split("(")[1]))
                        break

    def __check_400G_port_exist(self):
        cmd = get_sdk_cmd("ps")
        ret, output = port_getstatusoutput(cmd)
        if ret == 0:
            if "400G" in output:
                self.exist_400G_flag = True

    def _get_mac_side_prbsstat_ber(self, port):
        results = []
        i = 0
        for line in self.mac_output:
            if ((self.bcm_ports[port - 1] + "[0]") in line) or ((self.bcm_ports[port - 1] + "[1]") in line):
                results.append(line)
                i = i + 1
        for result in results:
            i = i - 1
            ex = "[0-9]{0,}[.][0-9]{0,}[e][+][0-9]{0,}|[0-9]{0,}[.][0-9]{0,}[e][-][0-9]{0,}"
            ber = (re.search(ex, result))
            if (ber is not None):
                if (float(ber.group()) > float(self.standard)):
                    return False, results
                elif(i == 0):
                    return True, results
            # Nolock NoLock 版本差异都有
            elif ("nolock" in result.lower()) or ("lossoflock" in result.lower()):
                return False, results
        return False, results

    def _get_sys_side_prbsstat_ber(self, port):
        results = []
        icount = 2
        ex = "[0-9]+$"
        num = (re.search(ex, self.bcm_ports[port - 1])).group()
        num = int(num) * 2 + 1
        results.append(self.sys_output[num])
        results.append(self.sys_output[num + 1])
        for result in results:
            icount = icount - 1
            ex = "[0-9]{0,}[.][0-9]{0,}[e][+][0-9]{0,}|[0-9]{0,}[.][0-9]{0,}[e][-][0-9]{0,}"
            ber = re.search(ex, result)
            if(ber is not None):
                if(float(ber.group()) > float(self.standard)):
                    return False, results
                elif(icount == 0):
                    return True, results
            # Nolock NoLock 版本差异都有
            elif ("nolock" in result.lower()) or ("lossoflock" in result.lower()):
                return False, results
        return False, results

    def _get_line_side_prbsstat_ber(self, port):
        results = []
        icount = 4
        ex = "[0-9]+$"
        num = (re.search(ex, self.bcm_ports[port - 1])).group()
        num = int(num) * 4 + 1
        results.append(self.line_output[num])
        results.append(self.line_output[num + 1])
        results.append(self.line_output[num + 2])
        results.append(self.line_output[num + 3])
        for result in results:
            icount = icount - 1
            ex = "[0-9]{0,}[.][0-9]{0,}[e][+][0-9]{0,}|[0-9]{0,}[.][0-9]{0,}[e][-][0-9]{0,}"
            ber = re.search(ex, result)
            if(ber is not None):
                if(float(ber.group()) > float(self.standard)):
                    return False, results
                elif(icount == 0):
                    return True, results
            # Nolock NoLock 版本差异都有
            elif ("nolock" in result.lower()) or ("lossoflock" in result.lower()):
                return False, results
        return False, results

    def _get_mac_side_prbsstat_ber_new(self, port):
        u'''获取mac端prbsstat_ber'''
        lines = self.mac_output.split("\n")
        prbs_ber_flag = 0
        prbs_ber_test_fail = 0
        output_content = ""
        lport = port
        unit_port = self.unit_ports[port - 1]
        bcm_port = self.bcm_ports[port - 1]
        output_content_port_info = "port:%-3d %s(%d)" % (lport, bcm_port, unit_port)
        stand_ber = self.standard

        for line in lines:
            if (re.search('.*].*e', line.strip())):
                line_bcm_port = line.split('[')[0]
                if bcm_port == line_bcm_port:
                    if "xe" in line_bcm_port:
                        stand_ber = 1.0e-12
                    prbs_ber_flag = 1
                    lane_num = int((line.split(']')[0]).split('[')[1])
                    prbs_ber = line.split(' ')[-1]
                    if (float(prbs_ber) > float(stand_ber)):
                        prbs_ber_test_fail = 1
                        output_content = output_content + "%-20s Lane[%d] prbs_ber:%.2e > %s, test fail\n" % (output_content_port_info, lane_num, float(prbs_ber), float(stand_ber))
                    else:
                        output_content = output_content + "%-20s Lane[%d] prbs_ber:%.2e <= %s, test success\n" % (output_content_port_info, lane_num, float(prbs_ber), float(stand_ber))
            elif (re.search('Nolock', line.strip())
                or re.search('NoLock', line.strip())
                or re.search('LossOfLock', line.strip())):
                line_bcm_port = line.split('[')[0]
                if bcm_port == line_bcm_port:
                    prbs_ber_flag = 1
                    prbs_ber_test_fail = 1
                    lane_num = int((line.split(']')[0]).split('[')[1])
                    lock_info = line.split(' ')[-1]

                    output_content = output_content + "%-20s Lane[%d] %s, test fail\n" % (output_content_port_info, lane_num, lock_info)

        # output_content += "\n"
        output_content = output_content[:-1]
        if prbs_ber_flag == 1 and prbs_ber_test_fail == 0:
            return True, output_content
        elif prbs_ber_flag == 1 and prbs_ber_test_fail == 1:
            return False, output_content
        else:
            output_content = "%-20s get prbs_ber fail, output:%s" % (output_content_port_info, self.mac_output)
            return False, output_content

    def _get_sys_side_prbsstat_ber_new(self, port):
        u'''获取sys端prbsstat_ber'''
        lines = self.sys_output.split("\n")
        prbs_ber_flag = 0
        prbs_ber_test_fail = 0
        output_content = ""
        lport = port
        unit_port = self.unit_ports[port - 1]
        bcm_port = self.bcm_ports[port - 1]
        output_content_port_info = "port:%-3d %s(%d)" % (lport, bcm_port, unit_port)

        for line in lines:
            if (re.search('.*] Ber', line.strip())):
                line_unit_port = int((line.split('Lane')[0]).split('Port')[1])
                if unit_port == line_unit_port:
                    prbs_ber_flag = 1
                    lane_num = int((line.split(']')[0]).split('[')[1])
                    prbs_ber = (line.split('Ber')[1]).split()[0]
                    if (float(prbs_ber) > float(self.standard)):
                        prbs_ber_test_fail = 1
                        output_content = output_content + "%-20s Lane[%d] prbs_ber:%s > %s, test fail\n" % (output_content_port_info, lane_num, prbs_ber, self.standard)
                    else:
                        output_content = output_content + "%-20s Lane[%d] prbs_ber:%s <= %s, test success\n" % (output_content_port_info, lane_num, prbs_ber, self.standard)
            elif (re.search('Nolock', line.strip())
                or re.search('NoLock', line.strip())
                or re.search('LossOfLock', line.strip())):
                line_unit_port = int((line.split('Lane')[0]).split('Port')[1])
                if unit_port == line_unit_port:
                    prbs_ber_flag = 1
                    prbs_ber_test_fail = 1
                    lane_num = int((line.split(']')[0]).split('[')[1])
                    lock_info = line.split(']')[1]

                    output_content = output_content + "%-20s Lane[%d] %s, test fail\n" % (output_content_port_info, lane_num, lock_info)

        # output_content += "\n"
        output_content = output_content[:-1]
        if prbs_ber_flag == 1 and prbs_ber_test_fail == 0:
            return True, output_content
        elif prbs_ber_flag == 1 and prbs_ber_test_fail == 1:
            return False, output_content
        else:
            output_content = "%-20s get prbs_ber fail, output:%s" % (output_content_port_info, self.sys_output)
            return False, output_content

    def _get_line_side_prbsstat_ber_new(self, port):
        u'''获取line端prbsstat_ber'''
        lines = self.line_output.split("\n")
        prbs_ber_flag = 0
        prbs_ber_test_fail = 0
        output_content = ""
        lport = port
        unit_port = self.unit_ports[port - 1]
        bcm_port = self.bcm_ports[port - 1]
        output_content_port_info = "port:%-3d %s(%d)" % (lport, bcm_port, unit_port)
        standard_ber_val = self.standard
        port_400g_list = [17,18,19,20,
                        37,38,39,40,
                        57,58,59,60,
                        77,78,79,80]
        if port in port_400g_list:
            standard_ber_val = 1.0e-5

        for line in lines:
            if (re.search('.*] Ber', line.strip())):
                line_unit_port = int((line.split('Lane')[0]).split('Port')[1])
                if unit_port == line_unit_port:
                    prbs_ber_flag = 1
                    lane_num = int((line.split(']')[0]).split('[')[1])
                    prbs_ber = (line.split('Ber')[1]).split()[0]
                    if (float(prbs_ber) > float(standard_ber_val)):
                        prbs_ber_test_fail = 1
                        output_content = output_content + "%-20s Lane[%d] prbs_ber:%s > %s, test fail\n" % (output_content_port_info, lane_num, prbs_ber, standard_ber_val)
                    else:
                        output_content = output_content + "%-20s Lane[%d] prbs_ber:%s <= %s, test success\n" % (output_content_port_info, lane_num, prbs_ber, standard_ber_val)
            elif (re.search('Nolock', line.strip())
                or re.search('NoLock', line.strip())
                or re.search('LossOfLock', line.strip())):
                prbs_ber_flag = 1
                prbs_ber_test_fail = 1
                lane_num = int((line.split(']')[0]).split('[')[1])
                lock_info = line.split(']')[1]

                output_content = output_content + "%-20s Lane[%d] %s, test fail\n" % (output_content_port_info, lane_num, lock_info)

        # output_content += "\n"
        output_content = output_content[:-1]
        if prbs_ber_flag == 1 and prbs_ber_test_fail == 0:
            return True, output_content
        elif prbs_ber_flag == 1 and prbs_ber_test_fail == 1:
            return False, output_content
        else:
            output_content = "%-20s get prbs_ber fail, output:%s" % (output_content_port_info, self.line_output)
            return False, output_content

    def init_port_prbs(self):
        if global_extphy_newcmd_list.get(self.get_onie_platform(), False) is True:
            return self.init_port_prbs_extphy_newcmd()

        if self.get_onie_platform() in global_esw_sdkcmd_prbs_support:
            return self.init_port_prbs_esw()

        if self.exist_400G_flag is True:
            return self.init_port_prbs_new()

        cmd = get_sdk_cmd("phy diag ce prbs set p=3")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy control ce prbs lnside=1 p=5")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy control ce prbs lnside=0 p=5")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy diag ce prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce prbsstat start i=120")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy control ce prbs lnside=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy control ce prbs lnside=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(120)
        cmd = get_sdk_cmd("phy control ce prbs lnside=1 time=120 cal=1")
        ret, output = port_getstatusoutput(cmd)
        self.sys_output = output.split("\r\n")
        if(ret != 0 or "fail" in output):
            return False, output
        cmd = get_sdk_cmd("phy control ce prbs lnside=0 time=120 cal=1")
        ret, output = port_getstatusoutput(cmd)
        self.line_output = output.split("\r\n")
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(30)
        cmd = get_sdk_cmd("phy diag ce prbsstat ber")
        ret, output = port_getstatusoutput(cmd)
        self.mac_output = output.split("\r\n")
        if(ret != 0 or "unlock" in output or "fail" in output):
            return False, output
        return True, output

    def init_port_prbs_extphy_newcmd(self):
        if self.exist_400G_flag is True:
            return self.init_port_prbs_new_extphy_newcmd()

        cmd = get_sdk_cmd("phy diag ce prbs set p=3")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy user_diag ce setprbs lnside=1 poly=5")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy user_diag ce setprbs lnside=0 poly=5")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy diag ce prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce prbsstat start i=120")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy user_diag ce getprbs lnside=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy user_diag ce getprbs lnside=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(120)
        cmd = get_sdk_cmd("phy user_diag ce calprbs lnside=1 time=120")
        ret, output = port_getstatusoutput(cmd)
        self.sys_output = output.split("\n")
        if(ret != 0 or "fail" in output):
            return False, output
        cmd = get_sdk_cmd("phy user_diag ce calprbs lnside=0 time=120")
        ret, output = port_getstatusoutput(cmd)
        self.line_output = output.split("\n")
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(30)
        cmd = get_sdk_cmd("phy diag ce prbsstat ber")
        ret, output = port_getstatusoutput(cmd)
        self.mac_output = output.split("\n")
        if(ret != 0 or "unlock" in output or "fail" in output):
            return False, output
        return True, output

    def init_port_prbs_new(self):
        # disable line侧
        cmd = get_sdk_cmd("phy control ce,cd txdis_set lnside=0 enable=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(5)

        # 获取mac侧 sys侧 prbs测试结果
        cmd = get_sdk_cmd("phy diag ce,cd prbs set p=3")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy control ce,cd prbs lnside=1 p=5")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(5)

        cmd = get_sdk_cmd("phy diag ce,cd prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce,cd prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce,cd prbsstat start i=120")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy control ce,cd prbs lnside=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        time.sleep(120)
        cmd = get_sdk_cmd("phy control ce,cd prbs lnside=1 time=120 cal=1")
        ret, output = port_getstatusoutput(cmd)
        self.sys_output = output
        #print("cmd:%s,\nret:%d\noutput:%s\n" % (cmd, ret, output))
        if(ret != 0 or "fail" in output):
            return False, output

        time.sleep(30)
        cmd = get_sdk_cmd("phy diag ce,cd prbsstat ber")
        ret, output = port_getstatusoutput(cmd)
        self.mac_output = output
        #print("cmd:%s,\nret:%d\noutput:%s\n" % (cmd, ret, output))
        if(ret != 0 or "unlock" in output or "fail" in output ):
            return False, output

        cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
            ret, output = port_getstatusoutput(cmd)
        cmd = get_sdk_cmd("phy control ce,cd prbs lnside=1 clear=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy control ce,cd prbs lnside=1 clear=1")
            ret, output = port_getstatusoutput(cmd)

        # enable line侧
        cmd = get_sdk_cmd("phy control ce,cd txdis_set lnside=0 enable=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(15)

        # 获取line侧 prbs测试结果
        cmd = get_sdk_cmd("phy control ce,cd prbs lnside=0 p=5")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(5)

        cmd = get_sdk_cmd("phy control ce,cd prbs lnside=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        time.sleep(120)
        cmd = get_sdk_cmd("phy control ce,cd prbs lnside=0 time=120 cal=1")
        ret, output = port_getstatusoutput(cmd)
        self.line_output = output
        #print("cmd:%s,\nret:%d\noutput:%s\n" % (cmd, ret, output))
        if(ret != 0 or "fail" in output):
            return False, output

        cmd = get_sdk_cmd("phy control ce,cd prbs lnside=0 clear=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy control ce,cd prbs lnside=0 clear=1")
            ret, output = port_getstatusoutput(cmd)

        return True, output

    def init_port_prbs_esw(self):
        cmd = get_sdk_cmd("phy diag cd,xe prbs set unit=0 p=3")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag cd,xe prbsstat start i=120")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy diag cd,xe prbsstat ber")
        ret, output = port_getstatusoutput(cmd)
        time.sleep(120)
        cmd = get_sdk_cmd("phy diag cd,xe prbsstat ber")
        ret, output = port_getstatusoutput(cmd)
        self.mac_output = output
        if(ret != 0 or "unlock" in output or "fail" in output):
            return False, output

        cmd = get_sdk_cmd("phy diag cd,xe prbs clear")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        return True, output

    def init_port_prbs_new_extphy_newcmd(self):
        # disable line侧
        cmd = get_sdk_cmd("phy user_diag ce,cd setsquelch lnside=0 txrx=1 enable=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(5)

        # 获取mac侧 sys侧 prbs测试结果
        cmd = get_sdk_cmd("phy diag ce,cd prbs set p=3")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(5)
        cmd = get_sdk_cmd("phy user_diag ce,cd setprbs lnside=1 poly=5")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(5)

        cmd = get_sdk_cmd("phy diag ce,cd prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce,cd prbs get")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy diag ce,cd prbsstat start i=120")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        cmd = get_sdk_cmd("phy user_diag ce,cd getprbs lnside=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        time.sleep(120)
        cmd = get_sdk_cmd("phy user_diag ce,cd calprbs lnside=1 time=120")
        ret, output = port_getstatusoutput(cmd)
        self.sys_output = output
        #print("cmd:%s,\nret:%d\noutput:%s\n" % (cmd, ret, output))
        if(ret != 0 or "fail" in output):
            return False, output

        time.sleep(30)
        cmd = get_sdk_cmd("phy diag ce,cd prbsstat ber")
        ret, output = port_getstatusoutput(cmd)
        self.mac_output = output
        #print("cmd:%s,\nret:%d\noutput:%s\n" % (cmd, ret, output))
        if(ret != 0 or "unlock" in output or "fail" in output ):
            return False, output

        cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
            ret, output = port_getstatusoutput(cmd)
        cmd = get_sdk_cmd("phy user_diag ce,cd clearprbs lnside=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy user_diag ce,cd clearprbs lnside=1")
            ret, output = port_getstatusoutput(cmd)

        # enable line侧
        cmd = get_sdk_cmd("phy user_diag ce,cd setsquelch lnside=0 txrx=1 enable=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output
        time.sleep(15)

        # 获取line侧 prbs测试结果
        cmd = get_sdk_cmd("phy user_diag ce,cd setprbs lnside=0 poly=5")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            return False, output
        time.sleep(5)

        cmd = get_sdk_cmd("phy user_diag ce,cd getprbs lnside=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0):
            return False, output

        time.sleep(120)
        cmd = get_sdk_cmd("phy user_diag ce,cd calprbs lnside=0 time=120")
        ret, output = port_getstatusoutput(cmd)
        self.line_output = output
        #print("cmd:%s,\nret:%d\noutput:%s\n" % (cmd, ret, output))
        if(ret != 0 or "fail" in output):
            return False, output

        cmd = get_sdk_cmd("phy user_diag ce,cd clearprbs lnside=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy user_diag ce,cd clearprbs lnside=0")
            ret, output = port_getstatusoutput(cmd)

        return True, output

    def clear_port_prbs(self):
        if global_extphy_newcmd_list.get(self.get_onie_platform(), False) is True:
            return self.clear_port_prbs_extphy_newcmd()

        if self.exist_400G_flag is True:
            return self.clear_port_prbs_new()
        cmd = get_sdk_cmd("phy diag ce prbs clear")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy diag ce prbs clear")
            ret, output = port_getstatusoutput(cmd)
        time.sleep(5)
        cmd = get_sdk_cmd("phy control ce prbs lnside=1 clear=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy control ce prbs lnside=1 clear=1")
            ret, output = port_getstatusoutput(cmd)
        time.sleep(5)
        cmd = get_sdk_cmd("phy control ce prbs lnside=0 clear=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy control ce prbs lnside=0 clear=1")
            ret, output = port_getstatusoutput(cmd)
        time.sleep(5)
        return True, output

    def clear_port_prbs_extphy_newcmd(self):
        if self.exist_400G_flag is True:
            return self.clear_port_prbs_new_extphy_newcmd()
        cmd = get_sdk_cmd("phy diag ce prbs clear")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy diag ce prbs clear")
            ret, output = port_getstatusoutput(cmd)
        time.sleep(5)
        cmd = get_sdk_cmd("phy user_diag ce clearprbs lnside=1")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy user_diag ce clearprbs lnside=1")
            ret, output = port_getstatusoutput(cmd)
        time.sleep(5)
        cmd = get_sdk_cmd("phy user_diag ce clearprbs lnside=0")
        ret, output = port_getstatusoutput(cmd)
        if(ret != 0 or "fail" in output):
            cmd = get_sdk_cmd("phy user_diag ce clearprbs lnside=0")
            ret, output = port_getstatusoutput(cmd)
        time.sleep(5)
        return True, output

    def clear_port_prbs_new(self):
        for i in range(2):
            cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0 or "fail" in output):
                cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
                ret, output = port_getstatusoutput(cmd)
            time.sleep(5)
            cmd = get_sdk_cmd("phy control ce,cd prbs lnside=1 clear=1")
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0 or "fail" in output):
                cmd = get_sdk_cmd("phy control ce,cd prbs lnside=1 clear=1")
                ret, output = port_getstatusoutput(cmd)
            time.sleep(5)
            cmd = get_sdk_cmd("phy control ce,cd prbs lnside=0 clear=1")
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0 or "fail" in output):
                cmd = get_sdk_cmd("phy control ce,cd prbs lnside=0 clear=1")
                ret, output = port_getstatusoutput(cmd)
            time.sleep(10)

        time.sleep(10)
        return True, output

    def clear_port_prbs_new_extphy_newcmd(self):
        for i in range(2):
            cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0 or "fail" in output):
                cmd = get_sdk_cmd("phy diag ce,cd prbs clear")
                ret, output = port_getstatusoutput(cmd)
            time.sleep(5)
            cmd = get_sdk_cmd("phy user_diag ce,cd clearprbs lnside=1")
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0 or "fail" in output):
                cmd = get_sdk_cmd("phy user_diag ce,cd clearprbs lnside=1")
                ret, output = port_getstatusoutput(cmd)
            time.sleep(5)
            cmd = get_sdk_cmd("phy user_diag ce,cd clearprbs lnside=0")
            ret, output = port_getstatusoutput(cmd)
            if(ret != 0 or "fail" in output):
                cmd = get_sdk_cmd("phy user_diag ce,cd clearprbs lnside=0")
                ret, output = port_getstatusoutput(cmd)
            time.sleep(10)

        time.sleep(10)
        return True, output

    def print_prbs(self, port, flag, info):
        print("===================================")
        lane = 0
        # print(info)
        for tmps in info:
            # print(len(tmps.split(',')))
            for tmp in tmps.split(','):
                tmp = tmp.lstrip('\'').rstrip('\'')
                # Nolock NoLock 版本差异都有
                if "nolock" in tmp.lower() or "lossoflock" in tmp.lower():
                    print("%4s Port:%3d  Lane:%d  Ber:NoLock" % (flag, port, lane))
                    continue
                ex = "[0-9]{0,}[.][0-9]{0,}[e][+][0-9]{0,}|[0-9]{0,}[.][0-9]{0,}[e][-][0-9]{0,}"
                ber = re.search(ex, tmp)
                if (ber != None):
                    print("%4s Port:%3d  Lane:%d  Ber:%s" % (flag, port, lane, float(ber.group())))
                else :
                    print("%4s Port:%3d  Lane:%d  Ber:null" % (flag, port, lane))
            lane = lane + 1
        print("===================================\n")

    def print_prbs_new(self, port, flag, info):
        # print('info:',info)
        print("===================================")
        for tmps in info.split('\n'):
            print("%4s %s" % (flag, tmps))
        print("===================================\n")

    def get_port_prbs_result(self, flag, port):
        if self.exist_400G_flag is False:
            if (flag == "mac"):
                ret, output = self._get_mac_side_prbsstat_ber(port)
                self.print_prbs(port, flag, output)
                return ret, output
            elif (flag == "sys"):
                ret, output = self._get_sys_side_prbsstat_ber(port)
                self.print_prbs(port, flag, output)
                return ret, output
            elif (flag == "line"):
                ret, output = self._get_line_side_prbsstat_ber(port)
                self.print_prbs(port, flag, output)
                return ret, output
            else:
                return False, "fail"
        else:
            if (flag == "mac"):
                ret, output = self._get_mac_side_prbsstat_ber_new(port)
                self.print_prbs_new(port, flag, output)
                output_trans = self.output_transform(port, output)
                return ret, output_trans
            elif (flag == "sys"):
                ret, output = self._get_sys_side_prbsstat_ber_new(port)
                self.print_prbs_new(port, flag, output)
                output_trans = self.output_transform(port, output)
                return ret, output_trans
            elif (flag == "line"):
                ret, output = self._get_line_side_prbsstat_ber_new(port)
                self.print_prbs_new(port, flag, output)
                output_trans = self.output_transform(port, output)
                return ret, output_trans
            else:
                return False, "flag check fail"

    def output_transform(self, port, output):
        alias = TESTCASE.get('port_alias_400g', None)
        output_trans = ""
        if alias:
            port_alias = alias.get(port, "unknown")
            output_trans = port_alias + "\n" + output
        else:
            output_trans = output

        return output_trans

@click.group(cls=AliasedGroup, context_settings=CONTEXT_SETTINGS)
def main():
    '''device operator'''
    pass


@main.command()
def test():
    pk = PortKrTest()
    pk.clear_port_packets()
    ret, result = pk.start_send_port_packets("eth1", vlan=2001)
    # print ret
    # print result
    ret, result = pk.check_port_packets("eth1")
    print(ret)
    print(result)
    pk.clear_port_packets()
    ret, result = pk.start_send_port_packets("eth2", vlan=2002)
    # print ret
    # print result
    ret, result = pk.check_port_packets("eth2")
    print(ret)
    print(result)
    pk.clear_port_packets()


if __name__ == '__main__':
    main()


'''
portts = PortTest()


portts.init_port_prbs()
for i in range(64):
    portts.set_port_prbs((i+1),1)
    #portts.set_port_prbs(2,1)

time.sleep(5)

for i in range(64):
    print portts.get_port_prbs_result((i+1))
    #print portts.get_port_prbs_result(2)

for i in range(64):
    portts.set_port_prbs((i+1),0)


#status = get_port_status(i + 1)
'''
