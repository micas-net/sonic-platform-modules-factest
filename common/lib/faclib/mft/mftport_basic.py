import sys, time, traceback, copy, json, os
import logging, re, subprocess, zipfile
from logging.handlers import RotatingFileHandler
from faclib.mft.mftport_const import *
class KrPortCmdBase():

    def __init__(self, redirect):
        self.redirect = redirect
        self.cmd_interval = 0.1
        self.packets_counters = dict()
        self.counter_keys_map = {
            TX_ALL_PKT_FRAME: "TX_packets",
            RX_ALL_PKT_FRAME: "RX_packets",
            TX_FCS_ERROR_FRAME: "TX_errors",
            RX_FCS_ERROR_FRAME: "RX_errors"
        }

    def exec_cmd(self, cmd, timeout=10) -> {int, str}:
        try:
            ret = subprocess.run(cmd, stdout=subprocess.PIPE, shell=True, check=True, timeout=timeout)
        except subprocess.CalledProcessError as e:
            result_str = "命令[%s]执行失败：%s" %(cmd, str(e))
            logging.warning(result_str)
            return False, result_str
        except subprocess.TimeoutExpired as e:
            result_str = "命令[%s]执行超时: %s" %(cmd, str(e))
            logging.warning(result_str)
            return False, result_str
        ret_stdout = ret.stdout.decode('utf-8')
        if self.redirect == False:
            print(ret_stdout)
        logging.debug("exec_cmd, cmd: {}, output: {}".format(cmd, ret_stdout))
        return True, ret_stdout

    def sleep(self, count):
        u'''方便实现sleep中断'''
        time.sleep(count)

    def update_packets_counters(self, kr_port: str = ""):
        u'''更新cpu侧端口收发包统计'''
        ret, output = self.exec_cmd("ifconfig {}".format(kr_port))
        if ret is False:
            logging.warning("update_packets_counters failed, output: %s" % output)
            return False, output
        ret, output = self.ifconfig_output_parse(output)
        if ret is False:
            logging.warning("update_packets_counters failed, output: %s" % output)
            return False, output
        # 将报文统计解析结果转化成特定格式
        new_packets_counters = dict()
        for port, output_counters in output.items():
            new_packets_counters[port] = dict()
            post_port_counters = self.packets_counters.get(port, dict())
            for counter_field, counter_key in self.counter_keys_map.items():
                post_counter = post_port_counters.get(counter_field, dict())
                counter_value = output_counters.get(counter_key, 0)
                new_packets_counters[port][counter_field] = {
                    COUNTER_VALUE: counter_value,
                    COUNTER_DIFF: counter_value-post_counter.get(COUNTER_VALUE, 0),
                    COUNTER_RATE: 0 # TODO
                }
        self.packets_counters.update(new_packets_counters)
        return True, ""

    def ifconfig_output_parse(self, input: str):
        u'''解析ifconfig eth命令的输出'''
        # TODO: 成功时output返回字典，失败时output返回失败原因
        lines = input.splitlines()
        ret = True
        ret_dict = dict()
        eth_port = None
        for line in lines:
            port_pattern = '(eth\d+): flags=(\d+)<(\S+)>  mtu (\d+)'
            matchs = re.match(pattern=port_pattern, string=line.strip())
            if matchs != None:
                eth_port = matchs.group(1)
                ret_dict[eth_port] = dict.fromkeys(["RX_packets", "RX_errors", "TX_packets", "TX_errors"], None)
            port_pattern = 'RX packets (\d+)  bytes (\d+)'
            matchs = re.match(pattern=port_pattern, string=line.strip())
            if matchs != None and eth_port != None:
                RX_packets = int(matchs.group(1))
                ret_dict[eth_port]["RX_packets"] = RX_packets
            port_pattern = 'RX errors (\d+)  dropped (\d+)'
            matchs = re.match(pattern=port_pattern, string=line.strip())
            if matchs != None and eth_port != None:
                RX_errors = int(matchs.group(1))
                ret_dict[eth_port]["RX_errors"] = RX_errors
            port_pattern = 'TX packets (\d+)  bytes (\d+)'
            matchs = re.match(pattern=port_pattern, string=line.strip())
            if matchs != None and eth_port != None:
                TX_packets = int(matchs.group(1))
                ret_dict[eth_port]["TX_packets"] = TX_packets
            port_pattern = 'TX errors (\d+)  dropped (\d+)'
            matchs = re.match(pattern=port_pattern, string=line.strip())
            if matchs != None and eth_port != None:
                TX_errors = int(matchs.group(1))
                ret_dict[eth_port]["TX_errors"] = TX_errors
                eth_port = None
        return ret, ret_dict

    def port_admin_set(self, kr_port: str, enable: bool) -> {bool, str}:
        if enable == True:
            ret, output = self.exec_cmd("ifconfig {} up".format(kr_port))
        else:
            ret, output = self.exec_cmd("ifconfig {} down".format(kr_port))
        if ret is False:
            logging.warning("port_admin_set failed, output: %s" % output)
            return False, output
        self.sleep(self.cmd_interval)
        return True, output

    def tx_send_packets(self, kr_port, count, pkt_size, dst_mac, vlan_id=None, **kwargs) -> {bool, str}:
        u'''调用pkt指令进行发包'''
        ret, output = self.exec_cmd("echo \"add_device {}\" > /proc/net/pktgen/kpktgend_0".format(kr_port))
        if ret is False:
            logging.warning("tx_send_packets failed, output: %s" % output)
            return False, output
        self.sleep(self.cmd_interval)
        ret, output = self.exec_cmd("echo \"pkt_size {}\" > /proc/net/pktgen/{}".format(pkt_size, kr_port))
        if ret is False:
            logging.warning("tx_send_packets failed, output: %s" % output)
            return False, output
        self.sleep(self.cmd_interval)
        ret, output = self.exec_cmd("echo \"count {}\" > /proc/net/pktgen/{}".format(count, kr_port))
        if ret is False:
            logging.warning("tx_send_packets failed, output: %s" % output)
            return False, output
        self.sleep(self.cmd_interval)
        ret, output = self.exec_cmd("echo \"dst_mac {}\" > /proc/net/pktgen/{}".format(dst_mac, kr_port))
        if ret is False:
            logging.warning("tx_send_packets failed, output: %s" % output)
            return False, output
        self.sleep(self.cmd_interval)
        if vlan_id != None:
            ret, output = self.exec_cmd("echo \"vlan_id {}\" > /proc/net/pktgen/{}".format(vlan_id, kr_port))
            if ret is False:
                logging.warning("tx_send_packets failed, output: %s" % output)
                return False, output
            self.sleep(self.cmd_interval)
        # 发送报文
        ret, output = self.exec_cmd("echo \"start\" > /proc/net/pktgen/pgctrl")
        if ret is False:
            logging.warning("tx_send_packets failed, output: %s" % output)
            return False, output
        self.sleep(self.cmd_interval)
        # pktgen移出eth
        ret, output = self.exec_cmd("echo \"rem_device_all\" > /proc/net/pktgen/kpktgend_0")
        if ret is False:
            logging.warning("tx_send_packets failed, output: %s" % output)
            return False, output
        self.sleep(self.cmd_interval)
        return True, output

class PortSdkCmdBase():
    u'''负责命令执行和结果解析的实现'''
    def __init__(self, redirect, dport_start_index = 1, cmd_util="bcmcmdb"):
        '''
        port_map = {
            '1': {
                'port': 'eth1(1)',
                'eth_port': 'eth1',
                'lport': '1'
            }
        }
        port_status = {
            '1' : {
                'Ena/Link': 'down', 
                'Configured Speed': '400G', 
                'Linkscan': 'HW', 
                'Autoneg': 'No', 
                'FEC': 'RS-544-2xN', 
                'Loopback': 'NONE'
                }
            }
        '''
        self.port_map = None
        self.port_status = None
        self.dport_start_index = dport_start_index
        self.redirect = redirect
        self.poly_modes_map = dict()
        self.counter_keys_map = dict()
        self.loopback_modes_map = dict()
        self.stp_state_map = dict()
        self.packets_counters = dict()
        self.default_vlan = 1
        self.cmd_util = cmd_util

    def exec_cmd(self, cmd, timeout=10) -> {int, str}:
        try:
            ret = subprocess.run(cmd, stdout=subprocess.PIPE, shell=True, check=True, timeout=timeout)
        except subprocess.CalledProcessError as e:
            result_str = "命令[%s]执行失败：%s" %(cmd, str(e))
            logging.warning(result_str)
            return False, result_str
        except subprocess.TimeoutExpired as e:
            result_str = "命令[%s]执行超时: %s" %(cmd, str(e))
            logging.warning(result_str)
            return False, result_str
        ret_stdout = ret.stdout.decode('utf-8')
        if self.redirect == False:
            print(ret_stdout)
        logging.debug("exec_cmd, cmd: {}, output: {}".format(cmd, ret_stdout))
        return True, ret_stdout

    def exec_bcmcmd(self, cmd:str, timeout=10) -> {bool, str}:
        cmd_string = "{} '{}'".format(self.cmd_util, cmd)
        ret, output = self.exec_cmd(cmd_string, timeout=timeout)
        if ret != True:
            return False, output
        if (output.find("hsdk msg connect fail")>=0) or (output.find("hsdk_msg_wait_respon timeout")>=0):
            logging.warning("命令[%s]执行失败：%s" %(cmd, output))
            return False, ""
        return True, output
    
    def exec_bcmcmd_cint(self, cint_code=None, cint_file_path=None, timeout=10):
        # 优先运行code, code为None时运行file
        logging.debug("exec_cint, cint_code: {}, cint_file_path: {}".format(cint_code, cint_file_path))
        if cint_code!= None:
            file_path = "/tmp/tmp.cint"
            with open(file_path, "w") as file:
                file.write("cint_reset();", file_path)
                file.write(cint_code, file_path)
            return self.exec_bcmcmd("cint {}".format(file_path), timeout=timeout)
        if cint_file_path != None:
            return self.exec_bcmcmd("cint {}".format(cint_file_path), timeout=timeout)
        return True, "have nothing to do"
    
    def get_dport_list(self):
        u'''获取sdk底层所有端口'''
        if self.port_map == None:
            logging.warning("get_dport_list failed, port_map is None")
            return None
        return sorted(list(self.port_map.keys()))

    def get_dport_by_eth(self, eth_port):
        if self.port_map == None:
            logging.warning("get_dport_by_eth failed, port_map is None")
            return None
        for dport, dport_info in self.port_map.items():
            if dport_info["eth_port"] == eth_port:
                return dport
        logging.warning("get_dport_by_eth failed, key error: {}".format(eth_port))
        return None

    def get_dport_by_lport(self, lport:str)->int:
        if self.port_map == None:
            logging.warning("get_dport_by_lport failed, port_map is None")
            return None
        for dport, dport_info in self.port_map.items():
            if dport_info["lport"] == lport:
                return dport
        logging.warning("get_dport_by_lport failed, key error: {}".format(lport))
        return None

    def get_lport_by_dport(self, dport:int)->str:
        if self.port_map == None:
            logging.warning("get_lport_by_dport failed, port_map is None")
            return None
        dport_info = self.port_map.get(dport, dict())
        lport = dport_info.get("lport", None)
        if lport == None:
            logging.warning("get_lport_by_dport failed, key error: {}".format(dport))
        return lport

    def get_eport_by_dport(self, dport:int):
        if self.port_map == None:
            logging.warning("get_eport_by_dport failed, port_map is None")
            return None
        dport_info = self.port_map.get(dport, dict())
        eport = dport_info.get("eth_port", None)
        if eport == None:
            logging.warning("get_eport_by_dport failed, key error: {}".format(dport))
        return eport

    def get_default_vlan(self)->int:
        return self.default_vlan

    def tx_send_packets(self, dport, count, pkt_size, dst_mac, src_mac=None, untagged=None, vlan_id=None, PatternRandom=None, **kwargs) -> {bool, str}:
        u'''调用SDK指令进行发包'''
        cmd_dict = {
            "PortBitMap": "PortBitMap={}".format(self.get_lport_by_dport(dport)),
            "count": count,
            "Length": "Length={}".format(pkt_size),
            "vlan": "",
            "Untagged": "",
            "SourceMac": "",
            "DestMac": "DestMac={}".format(dst_mac),
            "PatternRandom": "",
        }
        if vlan_id != None:
            cmd_dict["vlan"] = "vlan={}".format(vlan_id)
        if untagged != None:
            cmd_dict["Untagged"] = "Untagged={}".format("yes" if untagged else "no")
        if src_mac != None:
            cmd_dict["SourceMac"] = "SourceMac={}".format(src_mac)
        if PatternRandom != None:
            cmd_dict["PatternRandom"] = "PatternRandom={}".format("yes" if PatternRandom else "no")
        ret, output = self.exec_bcmcmd("tx {count} {Untagged} {PortBitMap} {Length} {DestMac} {SourceMac} {vlan} {PatternRandom} VISibility=no".format(**cmd_dict))
        if ret != True:
            logging.warning("tx_send_packets failed, output: %s" % output)
            return False, output
        return True, output

    def vlan_clear(self) -> {bool, str}:
        u'''端口测试vlan环境恢复'''
        ret, output = self.exec_bcmcmd("vlan clear")
        if ret != True:
            logging.warning("vlan_clear failed, output: %s" % output)
            return False, output
        return True, output

    def clear_counter(self) -> {bool, str}:
        u'''清除报文统计数据'''
        ret, output = self.exec_bcmcmd("clear counter")
        if ret != True:
            logging.warning("clear_counter failed, output: %s" % output)
            return False, output
        return True, output
    
    def port_stp_config(self, dports: list[int], state: str, stg: int=1) -> {bool, str}:
        u'''生成树协议配置,默认设置sdk的default group'''
        stg_cmd = {
            "stg": stg,
            "lports": ','.join([self.get_lport_by_dport(dport) for dport in dports])
        }
        cli_state = self.stp_state_map.get(state, None)
        if cli_state == None:
            logging.warning("port_stp_config failed, invalid stp state")
            return False, ""
        stg_cmd["state"] = cli_state
        ret, output = self.exec_bcmcmd("stg stp {stg} {lports} {state}".format(**stg_cmd))
        if ret != True:
            logging.warning("port_stp_config failed, output: %s" % output)
            return False, output
        return True, output

    def vlan_create(self, vlan_id: int, dports: list[int]=[], untag=False) -> {bool, str}:
        if dports == None:
            return False, "dports value error"
        lport_list = [str(self.get_lport_by_dport(dport)) for dport in dports]
        PortBitMap = ''
        UntagBitMap = ''
        if len(lport_list):
            PortBitMap = "PortBitMap={}".format(",".join(lport_list))
            if untag:
                UntagBitMap = "UntagBitMap={}".format(",".join(lport_list))
        ret, output = self.exec_bcmcmd("vlan create {} {} {}".format(vlan_id, PortBitMap, UntagBitMap))
        if ret != True:
            logging.warning("vlan_create failed, output: %s" % output)
            return False, output
        return True, output

    def vlan_add(self, vlan_id, dports: list[int], untag_dports: list[int]=[]) -> {bool, str}:
        u'''vlan端口添加'''
        if dports == None:
            return False, "dports value error"
        lport_list = [str(self.get_lport_by_dport(dport)) for dport in dports]
        untag_lport_list = [str(self.get_lport_by_dport(dport)) for dport in untag_dports]
        PortBitMap = ''
        UntagBitMap = ''
        if len(lport_list):
            PortBitMap = "PortBitMap={}".format(",".join(lport_list))
            if len(untag_lport_list):
                UntagBitMap = "UntagBitMap={}".format(",".join(untag_lport_list))
        else:
            return True, "No ports to add"
        ret, output = self.exec_bcmcmd("vlan add {} {} {}".format(vlan_id, PortBitMap, UntagBitMap))
        if ret != True:
            logging.warning("vlan_add failed, output: %s" % output)
            return False, output
        return True, output

    def vlan_remove(self, vlan_id, dports: list[int]) -> {bool, str}:
        u'''vlan端口删除'''
        if dports == None:
            return False, "dports value error"
        lport_list = [str(self.get_lport_by_dport(dport)) for dport in dports]
        ret, output = self.exec_bcmcmd("vlan remove {} PortBitMap={} ".format(vlan_id, ",".join(lport_list)))
        if ret != True:
            logging.warning("vlan_remove failed, output: %s" % output)
            return False, output
        return True, output
    
    def vlan_destory(self, vlan_id) -> {bool, str}:
        u'''vlan删除'''
        ret, output = self.exec_bcmcmd("vlan DeSTRoY {} ".format(vlan_id))
        if ret != True:
            logging.warning("vlan_destory failed, output: %s" % output)
            return False, output
        return True, output

    def vlan_clear(self) -> {bool, str}:
        u'''vlan全部删除'''
        ret, output = self.exec_bcmcmd("vlan clear")
        if ret != True:
            logging.warning("vlan_clear failed, output: %s" % output)
            return False, output
        return True, output

    def counter_on(self, enable:bool=True) -> {bool, str}:
        if enable:
            ret, output = self.exec_bcmcmd("counter on")
        else:
            ret, output = self.exec_bcmcmd("counter off")
        return ret, output

    def show_counter(self) -> {bool, str}:
        ret, output = self.exec_bcmcmd("show counter")
        if ret != True:
            logging.warning("show_counter failed, output: %s" % output)
            return False, output
        return True, output

    def prbs_poly_set(self, dports: list[int], poly:str) -> {bool, str}:
        if dports == None:
            return False, "dports value error"
        lport_list = [str(self.get_lport_by_dport(dport)) for dport in dports]
        mode = self.poly_modes_map.get(poly, None)
        if mode == None:
            output = "poly: {} is not supported".format(poly)
            logging.warning("prbs_poly_set failed, output: %s" % output)
            return False, output
        ret, output = self.exec_bcmcmd("phy prbs set p={} poly={}".format(",".join(lport_list), mode))
        if ret != True:
            logging.warning("prbs_poly_set failed, output: %s" % output)
            return False, output
        return True, output
        
    def prbs_poly_get(self, dports: list[int]) -> {bool, str}:
        if dports == None:
            return False, "dports value error"
        lport_list = [str(self.get_lport_by_dport(dport)) for dport in dports]
        ret, output = self.exec_bcmcmd("phy prbs get p={}".format(",".join(lport_list)))
        if ret != True:
            logging.warning("prbs_poly_get failed, output: %s" % output)
            return False, output
        return True, output

    def prbs_poly_clear(self, dports: list[int]) -> {bool, str}:
        if dports == None:
            return False, "dports value error"
        lport_list = [str(self.get_lport_by_dport(dport)) for dport in dports]
        ret, output = self.exec_bcmcmd("phy prbs clear p={}".format(",".join(lport_list)))
        if ret != True:
            logging.warning("prbs_poly_clear failed, output: %s" % output)
            return False, output
        return True, output

    def port_admin_set(self, dports: list[int], enable: bool) -> {bool, str}:
        if dports == None:
            return False, "dports value error"
        lport_list = [str(self.get_lport_by_dport(dport)) for dport in dports]
        ret, output = self.exec_bcmcmd("port ENA p={} {}".format(",".join(lport_list), "on" if enable else "off"))
        if ret != True:
            logging.warning("port_admin_set failed, output: %s" % output)
            return False, output
        return True, output

    def port_loopback_set(self, dports: list[int], loopback_mode: str) -> {bool, str}:
        if dports == None:
            return False, "dports value error"
        lport_list = [str(self.get_lport_by_dport(dport)) for dport in dports]
        mode = self.loopback_modes_map.get(loopback_mode, None)
        if mode == None:
            output = "loopback_mode: {} is not supported".format(loopback_mode)
            logging.warning("port_loopback_set failed, output: %s" % output)
            return False, output
        ret, output = self.exec_bcmcmd("port loopback {} mode={}".format(",".join(lport_list), mode))
        if ret != True:
            logging.warning("port_loopback_set failed, output: %s" % output)
            return False, output
        return True, output

    def port_pvid_set(self, dports: list[int], vlan_id: int):
        raise NotImplementedError

    def update_packets_counters(self):
        raise NotImplementedError

    def cint_output_parse(self, input: str):
        input_lines = input.splitlines()
        output_list = list()
        for line in input_lines:
            # "int $$ = 0 (0x0)"
            ret_pattern_1 = '(\S+) \$\$ = (\d+) \((\S+)\)'
            matchs = re.match(pattern=ret_pattern_1, string=line)
            if matchs == None or len(matchs.groups()) != 3:
                continue
            ret_type = matchs.group(1)
            ret_value = int(matchs.group(2))
            output_list.append({"type": ret_type, "value": ret_value})
        return True, output_list

class PortTestBase():
    u'''端口测试基类,负责测试流程的实现'''
    def __init__(self, port_scene):
        self.port_scene = port_scene

    def init_test(self) -> {bool, str}:
        u'''初始化测试环境'''
        return True, ""

    def start_test(self) -> {bool, str}:
        u'''开始测试'''
        return True, ""
    
    def stop_test(self) -> {bool, str}:
        u'''搜集测试结果后停止测试并恢复环境'''
        self.collect_result()
        return True, ""
    
    def deinit_test(self) -> {bool, str}:
        u'''恢复环境环境, 方便再次测试或执行其它测试'''
        return True, ""
    
    def collect_result(self) -> {bool, str}:
        u'''搜集测试数据,调用时机视测试方案而定'''
        return True, ""
    
    def analy_result(self) -> {bool, dict}:
        u'''分析测试结果并格式化成上层指定的格式返回'''
        raise NotImplementedError
    
    def sleep(self, count):
        u'''方便实现sleep中断'''
        time.sleep(count)

    def get_dport_list(self):
        u'''获取待测端口列表,如果生测未指定端口,则返回底层所有端口'''
        dport_list = self.port_scene.get_dport_list()
        if len(dport_list) == 0:
            dport_list = self.sdk_cmd_util.get_dport_list()
        return dport_list

    def stop_send_port_packets(self):
        u'''shutdown所有端口来实现断流'''
        # 使用sdk_cmd_util的dport_list，对底层所有端口操作
        ret, output = self.sdk_cmd_util.port_admin_set(self.sdk_cmd_util.get_dport_list(), enable=False)
        if ret is False:
            logging.warning("stop_send_port_packets failed")
            return False, output
        self.sleep(3)
        ret, output = self.sdk_cmd_util.port_admin_set(self.sdk_cmd_util.get_dport_list(), enable=True)
        if ret is False:
            logging.warning("stop_send_port_packets failed")
            return False, output
        return True, output

class PortPrbsTest(PortTestBase):
    u'''端口PRBS测试类'''
    def __init__(self, port_scene):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdBase(port_scene.redirect, self.port_scene.dport_start_index)
        '''
            prbs_get_output_parsed[dport][lane_index].update({
                "status": [PRBS_LOCKED|PRBS_NOT_LOCKED],
                "error": [int],
                "ber": [float],
            })
        '''
        self.prbs_get_output_parsed = dict()

    def init_test(self):
        u'''端口PRBS测试初始化'''
        logging.debug("端口PRBS测试初始化")
        self.prbs_poly = self.port_scene.get_mftport_config(PRBS_POLY_CFG, PRBS_POLY_P31)
        return True, ""

    def start_test(self, **kwargs):
        u'''端口PRBS测试开始'''
        logging.debug("端口PRBS测试开始")
        ret, output = self.sdk_cmd_util.prbs_poly_set(self.get_dport_list(), self.prbs_poly)
        if ret != True:
            logging.error("PortPrbsTest start_test failed")
            return False, output
        prbs_time = self.port_scene.get_mftport_config(PRBS_TIME_CFG, 10)
        self.sleep(prbs_time)
        return True, output
    
    def stop_test(self):
        u'''端口PRBS测试结束'''
        logging.debug("端口PRBS测试结束")
        ret, output = self.collect_result()
        if ret is False:
            logging.error("PortPrbsTest stop_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.prbs_poly_clear(self.get_dport_list())
        if ret != True:
            logging.error("PortPrbsTest stop_test failed")
            return False, output
        del_time = self.port_scene.get_mftport_config(PRBS_DEL_TIME_CFG, 10)
        self.sleep(del_time)
        return True, output
    
    def analy_result(self) -> {bool, dict}:
        ret_dict = {
            "test_info": "",
            "successports": [],
            "errorports": [],
            "updownerrorports": [],
            "prbs_info": "",
            "snake_info": "",
            "other_info": "",
            "port_info_dict": {}
        }
        test_result = True
        prbs_data = self.prbs_get_output_parsed
        prbs_ber_limit_dict = self.port_scene.get_mftport_config(PRBS_BER_DICT_CFG, dict())
        prbs_ber_limit = self.port_scene.get_mftport_config(PRBS_BER_CFG, 1.0e-9)
        successports = set()
        errorports = set()
        updownerrorports = set()
        port_info_dict = dict()
        prbs_info_list = list()
        for dport, dport_prbs_data in prbs_data.items():
            eport = self.sdk_cmd_util.get_eport_by_dport(dport)
            port_index = dport
            port_prbs_ber_limit = prbs_ber_limit_dict.get(port_index, prbs_ber_limit)
            for lane, lane_result in dport_prbs_data.items():
                if isinstance(port_prbs_ber_limit, dict):
                    lane_prbs_ber_limit = port_prbs_ber_limit.get(lane, prbs_ber_limit)
                else:
                    lane_prbs_ber_limit = port_prbs_ber_limit
                if lane_result.get("status") != PRBS_LOCKED:
                    prbs_info_list.append("%-20s Lane[%s] not locked, test failed" % (eport, lane))
                    errorports.add(dport)
                    continue
                if float(lane_result.get("ber")) > float(lane_prbs_ber_limit):
                    prbs_info_list.append("%-20s Lane[%s] prbs_ber:%s > %s, test failed" % (eport, lane, str(lane_result.get("ber")), str(lane_prbs_ber_limit)))
                    errorports.add(dport)
                    continue
                prbs_info_list.append("%-20s Lane[%s] prbs_ber:%s <= %s, test success" % (eport, lane, str(lane_result.get("ber")), str(lane_prbs_ber_limit)))
                successports.add(dport)
        successports = successports.difference(errorports)
        successports = successports.difference(updownerrorports)
        if len(updownerrorports) or len(errorports):
            test_result = False
        ret_dict = {
            "test_result": test_result,
            "test_info": "",
            "successports": sorted(list(successports)),
            "errorports": sorted(list(errorports)),
            "updownerrorports": sorted(list(updownerrorports)),
            "prbs_info": '\n'.join(prbs_info_list),
            "snake_info": "",
            "other_info": "",
            "port_info_dict": port_info_dict
        }
        return test_result, {"prbs_result_dict": ret_dict}

class PortBrcstTest(PortTestBase):
    u'''端口广播测试类'''
    def __init__(self, port_scene):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdBase(port_scene.redirect, self.port_scene.dport_start_index)
        '''
            packets_counters[dport] = {
                TX_FCS_ERROR_FRAME: {
                    COUNTER_VALUE: 0,
                    COUNTER_DIFF: 0,
                    COUNTER_RATE: 0,
                },
                RX_FCS_ERROR_FRAME: {
                    COUNTER_VALUE: 0,
                    COUNTER_DIFF: 0,
                    COUNTER_RATE: 0,
                },
            }
        '''
        self.packets_counters = dict()
        self.dst_mac = "ff:ff:ff:ff:ff:ff"
        self.pkt_count = 1000
        self.pkt_size = 64

    def init_test(self):
        u'''端口广播测试初始化'''
        logging.debug("端口广播测试初始化")
        self.vlan_id = self.sdk_cmd_util.get_default_vlan()
        ret, output = self.stop_send_port_packets()
        if ret is False:
            logging.error("PortBrcstTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.clear_counter()
        if ret is False:
            logging.error("PortBrcstTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.port_stp_config(self.get_dport_list(), STG_STP_FORWARD)
        if ret is False:
            logging.error("PortBrcstTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.vlan_add(self.vlan_id, self.get_dport_list())
        if ret is False:
            logging.error("PortBrcstTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.counter_on()
        if ret is False:
            logging.error("PortBrcstTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.update_port_status()
        if ret is False:
            logging.error("PortBrcstTest init_test failed")
            return False, output
        return True, ""
    
    def start_test(self, **kwargs):
        u'''端口广播测试开始'''
        logging.debug("端口广播测试开始")
        port = self.get_dport_list()[0]
        self.sdk_cmd_util.tx_send_packets(port, self.pkt_count, self.pkt_size, self.dst_mac)
        self.sleep(10)
        return True, ""
    
    def stop_test(self):
        u'''端口广播测试结束'''
        logging.debug("端口广播测试结束")
        ret, output = self.sdk_cmd_util.update_port_status()
        if ret is False:
            logging.error("PortBrcstTest stop_test failed")
            return False, output
        ret, output = self.stop_send_port_packets()
        if ret is False:
            logging.error("PortBrcstTest stop_test failed")
            return False, output
        ret, output = self.collect_result()
        if ret is False:
            logging.error("PortBrcstTest stop_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.vlan_clear()
        if ret is False:
            logging.error("PortBrcstTest stop_test failed")
            return False, output
        del_time = self.port_scene.get_mftport_config(BRCST_DEL_TIME_CFG, 10)
        self.sleep(del_time)
        return True, ""

    def analy_result(self):
        dport_list = self.get_dport_list()
        sdk_dport_list = self.sdk_cmd_util.get_dport_list()
        if dport_list == None or sdk_dport_list == None:
            dport_list = list()
        else:
            dport_list = [dport for dport in dport_list if dport in sdk_dport_list]
        successports = set()
        errorports = set()
        updownerrorports = set()
        port_info_dict = dict()
        prbs_info_list = list()
        test_result = True
        packets_counters = self.packets_counters
        for dport in dport_list:
            tfcs = packets_counters[dport][TX_FCS_ERROR_FRAME][COUNTER_VALUE]
            rfcs = packets_counters[dport][RX_FCS_ERROR_FRAME][COUNTER_VALUE]
            tpkt = packets_counters[dport][TX_ALL_PKT_FRAME][COUNTER_VALUE]
            rpkt = packets_counters[dport][RX_ALL_PKT_FRAME][COUNTER_VALUE]
            port_info_dict[dport] = {
                "port_info": "",
                "status": "",
                "log": ""
            }
            port_info_dict[dport]['status'] = self.sdk_cmd_util.port_status[dport]['Ena/Link']
            port_info_dict[dport]['port_info'] = self.sdk_cmd_util.port_map[dport]['port']
            if self.sdk_cmd_util.port_status[dport]['Ena/Link'] != PORT_LINK_UP:
                updownerrorports.add(dport)
                port_info_dict[dport]['log'] = "tfcs:{}, rfcs:{}, tpkt:{}, rpkt:{}".format(tfcs, rfcs, tpkt, rpkt)
                continue
            if tfcs or rfcs:
                errorports.add(dport)
            if tpkt > 0 and rpkt > 0:
                successports.add(dport)
            else:
                errorports.add(dport)
            port_info_dict[dport] = {
                'log': "tfcs:{}, rfcs:{}, tpkt:{}, rpkt:{}".format(tfcs, rfcs, tpkt, rpkt)
            }
        successports = successports.difference(errorports)
        successports = successports.difference(updownerrorports)
        if len(updownerrorports) or len(errorports):
            test_result = False
        ret_dict = {
            "test_result": test_result,
            "test_info": "",
            "successports": sorted(list(successports)),
            "errorports": sorted(list(errorports)),
            "updownerrorports": sorted(list(updownerrorports)),
            "prbs_info": '\n'.join(prbs_info_list),
            "snake_info": "",
            "other_info": "",
            "port_info_dict": port_info_dict
        }
        return test_result, ret_dict

class PortFrameTest(PortTestBase):
    u'''端口收发帧测试类'''
    def __init__(self, port_scene):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdBase(port_scene.redirect, self.port_scene.dport_start_index)
        self.packets_counters = dict()
        self.pkt_count = 10000
        self.pkt_size = 256
        self.dst_mac = "00:00:00:00:00:02"

    def init_test(self):
        u'''端口收发帧测试初始化'''
        logging.debug("端口收发帧测试初始化")
        self.pkt_vlan_id = self.sdk_cmd_util.get_default_vlan() + 1
        ret, output = self.stop_send_port_packets()
        if ret is False:
            logging.error("PortFrameTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.clear_counter()
        if ret is False:
            logging.error("PortFrameTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.vlan_clear()
        if ret is False:
            logging.error("PortFrameTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.port_stp_config(self.get_dport_list(), STG_STP_FORWARD)
        if ret is False:
            logging.error("PortFrameTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.counter_on()
        if ret is False:
            logging.error("PortFrameTest init_test failed")
            return False, output
        loopback_mode = self.port_scene.get_loopback_mode()
        if loopback_mode != None:
            ret, output = self.sdk_cmd_util.port_loopback_set(self.get_dport_list(), loopback_mode)
            if ret is False:
                logging.error("PortFrameTest init_test failed")
                return False, output
        ret, output = self.sdk_cmd_util.update_port_status()
        if ret is False:
            logging.error("PortFrameTest init_test failed")
            return False, output
        return True, ""
    
    def start_test(self, **kwargs):
        u'''端口收发帧测试开始'''
        logging.debug("端口收发帧测试开始")
        for port in self.get_dport_list():
            self.sdk_cmd_util.tx_send_packets(port, self.pkt_count, self.pkt_size, self.dst_mac, vlan_id=self.pkt_vlan_id)
        self.sleep(3)
        return True, ""
    
    def stop_test(self):
        u'''端口收发帧测试结束'''
        logging.debug("端口收发帧测试结束")
        ret, output = self.sdk_cmd_util.update_port_status()
        if ret is False:
            logging.error("PortFrameTest stop_test failed")
            return False, output
        ret, output = self.stop_send_port_packets()
        if ret is False:
            logging.error("PortFrameTest stop_test failed")
            return False, output
        ret, output = self.collect_result()
        if ret is False:
            logging.error("PortFrameTest stop_test failed")
            return False, output
        del_time = self.port_scene.get_mftport_config(FRAME_DEL_TIME_CFG, 10)
        self.sleep(del_time)
        return True, ""
    
    def analy_result(self):
        dport_list = self.get_dport_list()
        sdk_dport_list = self.sdk_cmd_util.get_dport_list()
        if dport_list == None or sdk_dport_list == None:
            dport_list = list()
        else:
            dport_list = [dport for dport in dport_list if dport in sdk_dport_list]
        successports = set()
        errorports = set()
        updownerrorports = set()
        port_info_dict = dict()
        prbs_info_list = list()
        test_result = True
        packets_counters = self.packets_counters
        for dport in dport_list:
            tpkt = packets_counters[dport][TX_ALL_PKT_FRAME][COUNTER_VALUE]
            rpkt = packets_counters[dport][RX_ALL_PKT_FRAME][COUNTER_VALUE]
            port_info_dict[dport] = {
                "port_info": "",
                "status": "",
                "log": ""
            }
            port_info_dict[dport]['status'] = self.sdk_cmd_util.port_status[dport]['Ena/Link']
            port_info_dict[dport]['port_info'] = self.sdk_cmd_util.port_map[dport]['port']
            port_info_dict[dport]['log'] = "tpkt:{}, rpkt:{}".format(tpkt, rpkt)
            if self.sdk_cmd_util.port_status[dport]['Ena/Link'] != PORT_LINK_UP:
                updownerrorports.add(dport)
                continue
            if (tpkt != self.pkt_count) or (rpkt != self.pkt_count):
                errorports.add(dport)
                continue
            successports.add(dport)
        successports = successports.difference(errorports)
        successports = successports.difference(updownerrorports)
        if len(updownerrorports) or len(errorports):
            test_result = False
        ret_dict = {
            "test_result": test_result,
            "test_info": "",
            "successports": sorted(list(successports)),
            "errorports": sorted(list(errorports)),
            "updownerrorports": sorted(list(updownerrorports)),
            "prbs_info": '\n'.join(prbs_info_list),
            "snake_info": "",
            "other_info": "",
            "port_info_dict": port_info_dict
        }
        return test_result, ret_dict

class PortKrTest(PortTestBase):
    u'''端口KR测试类'''
    def __init__(self, port_scene):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdBase(port_scene.redirect, self.port_scene.dport_start_index)
        self.kr_cmd_util = KrPortCmdBase(port_scene.redirect)
        self.pkt_count = 10000
        self.pkt_size = 256
        self.vlan_id = 100
        self.dst_mac = "ff:ff:ff:ff:ff:ff"
        self.untagged = True
        self.cpu_mac_portmap = self.port_scene.get_mftport_config(CPU_MAC_PORTMAP_CFG, None)

    def init_test(self):
        if self.cpu_mac_portmap == None:
            logging.error("PortKrTest init_test failed")
            return False, "cpu_mac_portmap is None"
        # MAC端口设置pvid(对底层所有端口设置)，避免产生广播
        ret, output = self.sdk_cmd_util.port_pvid_set(self.sdk_cmd_util.get_dport_list(), self.vlan_id+1)
        if ret is False:
            logging.error("PortKrTest init_test failed")
            return False, output
        # 停止所有流量转发
        ret, output = self.stop_send_port_packets()
        if ret is False:
            logging.error("PortKrTest init_test failed")
            return False, output
        # 使能cpu口
        for cpu_port, mac_port in self.cpu_mac_portmap.items():
            ret, output = self.kr_cmd_util.port_admin_set(cpu_port, enable=True)
            if ret is False:
                logging.error("PortKrTest init_test failed")
                return False, output
        # 等待一段时间，cpu口Link之后会产生报文
        self.sleep(10)
        # 更新cpu口报文统计
        ret, output = self.kr_cmd_util.update_packets_counters()
        if ret is False:
            logging.error("PortKrTest init_test failed")
            return False, output
        # 清除mac口报文统计
        ret, output = self.sdk_cmd_util.clear_counter()
        if ret is False:
            logging.error("PortKrTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.counter_on()
        if ret is False:
            logging.error("PortKrTest init_test failed")
            return False, output
        return True, ""
    
    def start_test(self, **kwargs):
        # 加载pktgen模块
        ret, output = self.kr_cmd_util.exec_cmd("modprobe pktgen")
        if ret is False:
            logging.error("PortKrTest start_test failed")
            return False, output
        # mac和cpu互相发送报文
        for cpu_port, mac_port in self.cpu_mac_portmap.items():
            # cpu端口发包
            ret, output = self.kr_cmd_util.tx_send_packets(cpu_port, self.pkt_count, self.pkt_size, self.dst_mac, self.vlan_id)
            if ret is False:
                logging.error("PortKrTest start_test failed")
                return False, output
            # mac端口发包
            dport = self.sdk_cmd_util.get_dport_by_eth(mac_port)
            if dport == None:
                logging.error("PortKrTest start_test failed")
                return False, "mac port invalid"
            ret, output = self.sdk_cmd_util.tx_send_packets(dport, self.pkt_count, self.pkt_size, self.dst_mac, vlan_id=self.vlan_id, untagged=self.untagged)
            if ret is False:
                logging.error("PortKrTest start_test failed")
                return False, output
        return True, ""

    def collect_result(self) -> {bool, str}:
        # 更新mac端口状态
        ret, output = self.sdk_cmd_util.update_port_status()
        if ret is False:
            logging.error("PortKrTest start_test failed")
            return False, output
        # 更新cpu口报文统计
        ret, output = self.kr_cmd_util.update_packets_counters()
        if ret is False:
            logging.error("PortKrTest collect_result failed")
            return False, output
        # 收集mac口报文统计
        ret, output = self.sdk_cmd_util.update_packets_counters()
        if ret is False:
            logging.error("PortKrTest collect_result failed")
            return False, output
        return True, ""

    def stop_test(self):
        ret, output = self.collect_result()
        if ret is False:
            logging.error("PortKrTest stop_test failed")
            return False, output
        return True, ""

    def analy_result(self):
        successports = set()
        errorports = set()
        updownerrorports = set()
        port_info_dict = dict()
        prbs_info_list = list()
        test_result = True
        cpu_mac_portmap = self.cpu_mac_portmap
        mac_port_counters = self.sdk_cmd_util.packets_counters
        kr_port_counters = self.kr_cmd_util.packets_counters
        for cpu_port, mac_port in cpu_mac_portmap.items():
            cpu_port_tx = kr_port_counters[cpu_port][TX_ALL_PKT_FRAME][COUNTER_DIFF]
            cpu_port_rx = kr_port_counters[cpu_port][RX_ALL_PKT_FRAME][COUNTER_DIFF]
            dport = self.sdk_cmd_util.get_dport_by_eth(mac_port)
            mac_port_tx = mac_port_counters[dport][TX_ALL_PKT_FRAME][COUNTER_VALUE]
            mac_port_rx = mac_port_counters[dport][RX_ALL_PKT_FRAME][COUNTER_VALUE]
            port_info_dict[dport] = {
                'log': "tpkt:{}, rpkt:{}, cpu_tpkt:{}, cpu_rpkt:{}".format(mac_port_tx, mac_port_rx, cpu_port_tx, cpu_port_rx)
            }
            if self.sdk_cmd_util.port_status[dport]['Ena/Link'] != PORT_LINK_UP:
                updownerrorports.add(dport)
                continue
            if mac_port_rx != cpu_port_tx:
                errorports.add(dport)
                continue
            if mac_port_tx != cpu_port_rx:
                errorports.add(dport)
                continue
            if mac_port_rx == 0 or mac_port_tx == 0:
                errorports.add(dport)
                continue
            successports.add(dport)
        successports = successports.difference(errorports)
        successports = successports.difference(updownerrorports)
        if len(updownerrorports) or len(errorports):
            test_result = False
        ret_dict = {
            "test_result": test_result,
            "test_info": "",
            "successports": sorted(list(successports)),
            "errorports": sorted(list(errorports)),
            "updownerrorports": sorted(list(updownerrorports)),
            "prbs_info": '\n'.join(prbs_info_list),
            "snake_info": "",
            "other_info": "",
            "port_info_dict": port_info_dict
        }
        return test_result, ret_dict

class PortSnakeTest(PortTestBase):
    u'''端口自发包蛇形测试类'''
    def __init__(self, port_scene):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdBase(port_scene.redirect, self.port_scene.dport_start_index)
        self.packets_counters = dict()
        self.pkt_count = 10000
        self.pkt_size = 256
        self.dst_mac = "00:00:00:00:00:02"
        self.vlan_base_id = 100

    def init_test(self):
        u'''端口自发包蛇形测试初始化'''
        logging.debug("端口自发包蛇形测试初始化")
        ret, output = self.stop_send_port_packets()
        if ret is False:
            logging.error("PortSnakeTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.clear_counter()
        if ret is False:
            logging.error("PortSnakeTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.vlan_clear()
        if ret is False:
            logging.error("PortSnakeTest init_test failed")
            return False, output
        ret, output = self.sdk_cmd_util.counter_on()
        if ret is False:
            logging.error("PortSnakeTest init_test failed")
            return False, output
        loopback_mode = self.port_scene.get_loopback_mode()
        if loopback_mode != None:
            ret, output = self.sdk_cmd_util.port_loopback_set(self.get_dport_list(), loopback_mode)
            if ret is False:
                logging.error("PortSnakeTest init_test failed")
                return False, output
        ret, output = self.sdk_cmd_util.update_port_status()
        if ret is False:
            logging.error("PortSnakeTest init_test failed")
            return False, output
        # 创建vlan
        dport_list = self.get_dport_list()
        for vlan_id_offset in range(len(dport_list)):
            ret, output = self.sdk_cmd_util.vlan_create(vlan_id=self.vlan_base_id+vlan_id_offset)
            if ret is False:
                logging.error("PortSnakeTest init_test failed")
                return False, output
        # 添加vlan成员
        for port_index in range(len(dport_list)):
            vlan_id = self.vlan_base_id + port_index
            first_port = dport_list[port_index]
            if port_index%2 == 0:
                sec_port = dport_list[port_index+1]
            else:
                sec_port = dport_list[port_index-1]
            port_list = [first_port, sec_port]
            untag_port_list = [sec_port]
            ret, output = self.sdk_cmd_util.vlan_add(vlan_id=vlan_id, dports=port_list, untag_dports=untag_port_list)
            if ret is False:
                logging.error("PortSnakeTest init_test failed")
                return False, output
        # 添加l2表项
        for port_index in range(len(dport_list)):
            vlan_id = self.vlan_base_id + port_index
            first_port = dport_list[port_index]
            if port_index%2 == 0:
                sec_port = dport_list[port_index+1]
            else:
                sec_port = dport_list[port_index-1]
            ret, output = self.sdk_cmd_util.l2_add(dport=sec_port, vlan_id=vlan_id, mac=self.dst_mac, fwd=1)
            if ret is False:
                logging.error("PortSnakeTest init_test failed")
                return False, output
        # pvid设置
        for port_index in range(len(dport_list)):
            port_vlan_id = self.vlan_base_id + port_index
            dport = dport_list[port_index]
            ret, output = self.sdk_cmd_util.port_pvid_set(dports=[dport], vlan_id=port_vlan_id)
            if ret is False:
                logging.error("PortSnakeTest init_test failed")
                return False, output
        return True, ""
    
    def start_test(self, **kwargs):
        u'''端口自发包蛇形测试开始'''
        logging.debug("端口自发包蛇形测试开始")
        dport_list = self.get_dport_list()
        for dport_index in range(0, len(dport_list), 2):
            dport = dport_list[dport_index]
            pkt_vlan_id = self.vlan_base_id + dport_index
            ret, output = self.sdk_cmd_util.tx_send_packets(dport, self.pkt_count, self.pkt_size, self.dst_mac, vlan_id=pkt_vlan_id, PatternRandom=True)
            if ret is False:
                logging.error("PortSnakeTest start_test failed")
                return False, output
        self.sleep(3)
        return True, ""
    
    def stop_test(self):
        u'''端口自发包蛇形测试结束'''
        logging.debug("端口自发包蛇形测试结束")
        ret, output = self.sdk_cmd_util.update_port_status()
        if ret is False:
            logging.error("PortSnakeTest stop_test failed")
            return False, output
        # shutdown每个vlan的第一个端口来停止流量
        dport_list = self.get_dport_list()
        first_dport_list = list()
        for dport_index in range(0, len(dport_list), 2):
            first_dport_list.append(dport_list[dport_index])
        ret, output = self.sdk_cmd_util.port_admin_set(dports=first_dport_list, enable=False)
        if ret is False:
            logging.warning("PortSnakeTest stop_test failed")
            return False, output
        self.sleep(3)
        ret, output = self.sdk_cmd_util.port_admin_set(dports=first_dport_list, enable=True)
        if ret is False:
            logging.warning("PortSnakeTest stop_test failed")
            return False, output
        # 收集测试结果
        ret, output = self.collect_result()
        if ret is False:
            logging.error("PortSnakeTest stop_test failed")
            return False, output
        del_time = self.port_scene.get_mftport_config(FRAME_DEL_TIME_CFG, 10)
        self.sleep(del_time)
        return True, ""
    
    def analy_result(self):
        dport_list = self.get_dport_list()
        sdk_dport_list = self.sdk_cmd_util.get_dport_list()
        if dport_list == None or sdk_dport_list == None:
            dport_list = list()
        else:
            dport_list = [dport for dport in dport_list if dport in sdk_dport_list]
        successports = set()
        errorports = set()
        updownerrorports = set()
        port_info_dict = dict()
        prbs_info_list = list()
        test_result = True
        packets_counters = self.packets_counters
        for dport in dport_list:
            tbit_rate = round(packets_counters[dport][TX_BYTE_FRAME][COUNTER_RATE]*8/G_BITS*(self.pkt_size+20)/self.pkt_size, 2)
            rbit_rate = round(packets_counters[dport][RX_BYTE_FRAME][COUNTER_RATE]*8/G_BITS*(self.pkt_size+20)/self.pkt_size, 2)
            effect_tbit_rate = round(packets_counters[dport][TX_BYTE_FRAME][COUNTER_RATE]*8/G_BITS, 2)
            effect_rbit_rate = round(packets_counters[dport][RX_BYTE_FRAME][COUNTER_RATE]*8/G_BITS, 2)
            port_info_dict[dport] = {
                "port_info": "",
                "status": "",
                "log": ""
            }
            port_info_dict[dport]['status'] = self.sdk_cmd_util.port_status[dport]['Ena/Link']
            port_info_dict[dport]['port_info'] = self.sdk_cmd_util.port_map[dport]['port']
            port_info_dict[dport]['log'] = "\n实际发送速率:{} Gbps, 实际接收速率:{} Gbps\n有效发送速率:{} Gbps, 有效接收速率:{} Gbps".format(
                                                        tbit_rate, rbit_rate, effect_tbit_rate, effect_rbit_rate)
            if self.sdk_cmd_util.port_status[dport]['Ena/Link'] != PORT_LINK_UP:
                updownerrorports.add(dport)
                continue
            successports.add(dport)
        successports = successports.difference(errorports)
        successports = successports.difference(updownerrorports)
        if len(updownerrorports) or len(errorports):
            test_result = False
        ret_dict = {
            "test_result": test_result,
            "test_info": "",
            "successports": sorted(list(successports)),
            "errorports": sorted(list(errorports)),
            "updownerrorports": sorted(list(updownerrorports)),
            "prbs_info": '\n'.join(prbs_info_list),
            "snake_info": "",
            "other_info": "",
            "port_info_dict": port_info_dict
        }
        return test_result, ret_dict

class PortSceneBase():
    u'''负责与factest交互的上层接口'''
    def __init__(self, mftport_config):
        self.mftport_config = mftport_config
        self.dport_list = self.mftport_config.get(DPORT_LIST_CFG, [])
        self.dport_start_index = self.mftport_config.get(DPORT_START_INDEX_CFG, 1)
        self.loopback_mode = None
        self.port_list = list()
        self.redirect = True
        self.test_type = None
        self.test_class_map = {
            PORT_PRBS_TEST_CLASS: PortPrbsTest,
            PORT_BRCST_TEST_CLASS: PortBrcstTest,
            PORT_FRAME_TEST_CLASS: PortFrameTest,
            PORT_KR_TEST_CLASS: PortKrTest,
            PORT_SNAKE_TEST_CLASS: PortSnakeTest
        }
        self.snake_test_obj = None
        self.logging_init(self.mftport_config.get(LOG_LEVEL_CFG, 1))
        
    def logging_init(self, level=1) -> None:
        self.log_file_path = LOG_FILE_PATH
        self.log_backup_count = 5
        debug_level_map = {
            1: logging.DEBUG,
            2: logging.INFO,
            3: logging.WARNING,
            4: logging.ERROR
        }
        level = debug_level_map.get(level, logging.DEBUG)
        logging.basicConfig(level=level)
        logger = logging.getLogger('')
        formatter = logging.Formatter('%(asctime)s %(filename)s %(levelname)s %(funcName)s[%(lineno)d]: %(message)s')
        logger.handlers.clear()
        # 记录到日志文件
        fh = RotatingFileHandler(self.log_file_path, maxBytes=1024*1024*5, backupCount=self.log_backup_count)
        fh.setLevel(level)
        fh.setFormatter(formatter)
        logger.addHandler(fh)
        # 输出到前台
        ch = logging.StreamHandler()
        ch.setLevel(logging.WARNING)
        ch.setFormatter(formatter)
        logger.addHandler(ch)

    def log_keeping(self, output_file_path):
        if os.path.exists(output_file_path):
            # 尝试添加数字后缀
            counter = 1
            while True:
                new_file_path = "{}.{}".format(output_file_path, counter)  # 新文件名
                if not os.path.exists(new_file_path):
                    output_file_path = new_file_path
                    break
                counter += 1  # 递增数字
        # 压缩保存日志文件
        log_files = list()
        log_files.append(self.log_file_path)
        for i in range(0, self.log_backup_count):
            log_files.append("{}.{}".format(self.log_file_path, i+1))
        # 确保输出路径的目录存在
        output_dir = os.path.dirname(output_file_path)
        if output_dir and not os.path.exists(output_dir):
            os.makedirs(output_dir)
        # 创建 ZIP 文件
        with zipfile.ZipFile(output_file_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
            for file_path in log_files:
                if os.path.isfile(file_path):  # 确保是文件
                    # 将文件添加到 ZIP 中
                    zipf.write(file_path, os.path.basename(file_path))
                    logging.debug(f"Added {file_path} to {output_file_path}")
                else:
                    logging.debug(f"Skipped {file_path} (not a file)")

    def get_dport_list(self) -> list[int]:
        u'''获取上层或配置文件指定的端口,上层优先级高'''
        if len(self.port_list) == 0:
            return [int(dport) for dport in self.dport_list]
        else:
            # 上层指定了测试端口
            return [int(self.dport_list[index]) for index in self.port_list]

    def get_mftport_config(self, field, default):
        u'''获取生测端口配置项'''
        try:
            return self.mftport_config.get(field, default)
        except Exception as e:
            return default

    def get_loopback_mode(self):
        u'''获取生测端口回环模式配置'''
        return self.loopback_mode

    def port_frame_test(self, port_list=[], redirect=True, mac_lb=False) -> dict:
        u'''端口收发帧测试'''
        test_class = self.test_class_map.get(PORT_FRAME_TEST_CLASS, None)
        if test_class == None:
            raise NotImplementedError("port_frame_test not implemented")
        self.port_list = port_list
        self.redirect = redirect
        if mac_lb:
            self.loopback_mode = LOOPBACK_MODE_MAC
        retry_count = 0
        retry_time = self.get_mftport_config(FRAME_TEST_RETRYNUM_CFG, 1)
        while retry_count < retry_time:
            retry_count +=1
            ret, output = self.port_test(test_class)
            if ret == True:
                return output
        return output

    def port_brcst_test(self, port_list=[], redirect=True, mac_lb=False) -> dict:
        u'''端口广播测试'''
        test_class = self.test_class_map.get(PORT_BRCST_TEST_CLASS, None)
        if test_class == None:
            raise NotImplementedError("port_brcst_test not implemented")
        self.port_list = port_list
        self.redirect = redirect
        if mac_lb:
            self.loopback_mode = LOOPBACK_MODE_MAC
        retry_count = 0
        retry_time = self.get_mftport_config(BRCST_TEST_RETRYNUM_CFG, 1)
        while retry_count < retry_time:
            retry_count +=1
            ret, output = self.port_test(test_class)
            if ret == True:
                return output
        return output
    
    def port_prbs_test(self, port_list=[], test_type="", redirect=True) -> dict:
        u'''端口PRBS测试'''
        test_class = self.test_class_map.get(PORT_PRBS_TEST_CLASS, None)
        if test_class == None:
            raise NotImplementedError("port_prbs_test not implemented")
        self.port_list = port_list
        self.test_type = test_type
        self.redirect = redirect
        retry_count = 0
        retry_time = self.get_mftport_config(PRBS_TEST_RETRYNUM_CFG, 1)
        while retry_count < retry_time:
            retry_count +=1
            ret, output = self.port_test(test_class)
            if ret == True:
                return output
        return output
    
    def port_kr_test(self, port_list=[], redirect=True) -> dict:
        u'''内部管理口收发包测试'''
        test_class = self.test_class_map.get(PORT_KR_TEST_CLASS, None)
        if test_class == None:
            raise NotImplementedError("port_kr_test not implemented")
        self.port_list = port_list
        self.redirect = redirect
        retry_count = 0
        retry_time = self.get_mftport_config(KR_TEST_RETRYNUM_CFG, 1)
        while retry_count < retry_time:
            retry_count +=1
            ret, output = self.port_test(test_class)
            if ret == True:
                return output
        return output
    
    def port_snake_test(self, port_list=[], test_type="", redirect=True, packet_size=1518, payload_type=0, mac_lb=False) -> dict:
        u'''开始蛇形打流测试'''
        snake_test_class = self.test_class_map.get(PORT_SNAKE_TEST_CLASS, None)
        if snake_test_class == None:
            raise NotImplementedError("port_snake_test not implemented")
        self.port_list = port_list
        self.test_type = test_type
        self.redirect = redirect
        self.packet_size = packet_size
        self.payload_type = payload_type
        self.mac_lb = mac_lb
        self.snake_test_obj = snake_test_class(self)
        ret, output = self.snake_test_obj.init_test()
        if not ret:
            logging.error("{} init test failed".format(snake_test_class.__name__))
            return output
        ret, output = self.snake_test_obj.start_test()
        if not ret:
            logging.error("{} start test failed".format(snake_test_class.__name__))
        return output
    
    def port_snake_test_stop(self, port_list=[], test_type="", redirect=True) -> dict:
        u'''停止蛇形打流测试'''
        snake_test_class = self.test_class_map.get(PORT_SNAKE_TEST_CLASS, None)
        if snake_test_class == None:
            raise NotImplementedError("port_snake_test not implemented")
        if self.snake_test_obj == None:
            logging.warning("snake test not started.")
            return False
        self.port_list = port_list
        self.test_type = test_type
        self.redirect = redirect
        ret, output = self.snake_test_obj.stop_test()
        if not ret:
            logging.error("{} stop test failed".format(snake_test_class.__name__))
            return output
        ret, output = self.snake_test_obj.deinit_test()
        if not ret:
            logging.error("{} deinit test failed".format(snake_test_class.__name__))
        return output
    
    def port_snake_test_result(self, port_list=[], test_type="", redirect=True) -> dict:
        u'''获取蛇形打流测试结果'''
        snake_test_class = self.test_class_map.get(PORT_SNAKE_TEST_CLASS, None)
        if snake_test_class == None:
            raise NotImplementedError("port_snake_test not implemented")
        if self.snake_test_obj == None:
            logging.warning("snake test not started.")
            return False
        self.port_list = port_list
        self.test_type = test_type
        self.redirect = redirect
        output = dict()
        ret, output = self.snake_test_obj.analy_result()
        if isinstance(output, dict):
            logging.debug(json.dumps(output, indent=4))
        else:
            logging.debug(output)
        return output

    def port_test(self, test_class):
        test_obj = test_class(self)
        try:
            ret, output = test_obj.init_test()
            if not ret:
                logging.error("{} init test failed".format(test_class.__name__))
            ret, output = test_obj.start_test()
            if not ret:
                logging.error("{} start test failed".format(test_class.__name__))
            ret, output = test_obj.stop_test()
            if not ret:
                logging.error("{} stop test failed".format(test_class.__name__))
            ret, output = test_obj.deinit_test()
            if not ret:
                logging.error("{} deinit test failed".format(test_class.__name__))
        except Exception as e:
            logging.debug(traceback.format_exc())
        try:
            ret = False
            output = dict()
            ret, output = test_obj.analy_result()
            if isinstance(output, dict):
                logging.debug(json.dumps(output, indent=4))
            else:
                logging.debug(output)
        except Exception as e:
            logging.debug(traceback.format_exc())
        finally:
            if ret == False:
                self.log_keeping(LOG_ZIP_FILE_PATH)
            return ret, output
