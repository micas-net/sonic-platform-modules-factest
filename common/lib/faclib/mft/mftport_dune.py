import sys, time, traceback, copy, json
import logging, re, subprocess
from faclib.mft.mftport_basic import PortSceneBase, PortSdkCmdBase, PortPrbsTest, PortBrcstTest, PortFrameTest, PortKrTest, PortSnakeTest
from faclib.mft.mftport_const import *

def dune_table_parse1(input_lines, row_sections_info, col_sections_info, main_key_index = 0, sub_key_index = None):
    col_section_divider = '|'
    # section_0可以解析出表名
    section_0_lines = input_lines[row_sections_info[0]["start_row"] + 1: row_sections_info[0]["end_row"]]
    table_name_keys = list()
    for line in section_0_lines:
        col_sections = line.split(col_section_divider)[1:-1]
        col_sections = [item.strip() for item in col_sections]
        for col_section_index in range(len(col_sections)):
            table_name_keys.append(col_sections[col_section_index])
    table_name = ' '.join(table_name_keys)
    # section_1可以解析出表头
    section_1_lines = input_lines[row_sections_info[1]["start_row"] + 1: row_sections_info[1]["end_row"]]
    table_header_keys = dict()
    for line in section_1_lines:
        col_sections = line.split(col_section_divider)[1:-1]
        col_sections = [item.strip() for item in col_sections]
        for col_section_index in range(len(col_sections)):
            table_header_key = table_header_keys.get(col_section_index, list())
            table_header_key.append(col_sections[col_section_index])
            table_header_keys[col_section_index] = table_header_key
    # 表头可能是分成多行,需要拼接到一起
    for table_header_key_index, table_header_key_values in table_header_keys.items():
        table_header_keys[table_header_key_index] = ' '.join(table_header_key_values).strip()
    # section_2是表格各行数值
    table = dict()
    section_2_lines = input_lines[row_sections_info[2]["start_row"] + 1: row_sections_info[2]["end_row"]]
    for line in section_2_lines:
        col_sections = line.split(col_section_divider)[1:-1]
        col_sections = [item.strip() for item in col_sections]
        if len(col_sections) == 0:
            continue
        table_item = dict()
        for key_index, key in table_header_keys.items():
            if key_index == main_key_index:
                continue
            if key_index == sub_key_index:
                continue
            table_item[key] = col_sections[key_index]
        if sub_key_index == None:
            table[col_sections[main_key_index]] = table_item
        else:
            if table.get(col_sections[main_key_index], None) == None:
                table[col_sections[main_key_index]] = dict()
            table[col_sections[main_key_index]][col_sections[sub_key_index]] = table_item
        
    return {table_name: table}

def dune_table_parse(lines_input:str, main_key_index = 0, sub_key_index = None):
    ''' Example: 
    ====================================================================================
    |                                        Port status                               |
    ====================================================================================
    | Port        | Ena/Link | Configured | Linkscan | Autoneg | FEC        | Loopback |
    |             |          | Speed      |          |         |            |          |
    ====================================================================================
    | eth1(1)     | down     | 400G       | HW       | No      | RS-544-2xN | NONE     |
    | eth2(2)     | down     | 400G       | HW       | No      | RS-544-2xN | NONE     |
    ====================================================================================
    '''
    tables = dict()
    lines = lines_input.splitlines()
    # 计算行、列长度, 确认表的数量
    line_len_set = set()
    for line in lines:
        line_len_set.add(len(line))
    # 解析各表数据
    for table_row_length in line_len_set:
        table_col_length = 0
        for line in lines:
            if len(line) == table_row_length:
                table_col_length +=1
        # 将表格按行分区
        row_section_divider = '='*table_row_length
        row_sections_info = dict()
        row_section_divider_list = list()
        for line_index in range(len(lines)):
            if lines[line_index] == row_section_divider:
                row_section_divider_list.append(line_index)
        row_section_num = int(len(row_section_divider_list)-1)
        for section_index in range(row_section_num):
            row_sections_info[section_index] = {
                "start_row": row_section_divider_list[section_index],
                "end_row": row_section_divider_list[section_index+ 1]
            }
        # 关键条件,用来过滤非表格输出
        if len(row_sections_info) != 3:
            continue
        # 将表格按列分区并解析
        col_section_divider = '|'
        col_section_divider_list = list()
        col_sections_info = dict()
        section_1_lines = lines[row_sections_info[1]["start_row"] + 1: row_sections_info[1]["end_row"]]
        for char_index in range(len(section_1_lines[0])):
            if section_1_lines[0][char_index] == col_section_divider:
                col_section_divider_list.append(char_index)
        col_section_num = len(col_section_divider_list) - 1
        for section_index in range(col_section_num):
            col_sections_info[section_index] = {
                "start_col": col_section_divider_list[section_index],
                "end_col": col_section_divider_list[section_index+ 1]
            }
        table = dune_table_parse1(lines, row_sections_info, col_sections_info, main_key_index, sub_key_index)
        tables.update(table)
    return tables

def show_counter_parse(lines_input):
    '''output: {"TX COUNTERS": {}, "RX COUNTERS": {}}'''
    ret = dune_table_parse(lines_input)
    for table, table_value in ret.items():
        for port, port_value in table_value.items():
            for counter, counter_value in port_value.items():
                port_value[counter] = int(counter_value.replace(',', ''))
    return ret

def port_status_parse(lines_input):
    return dune_table_parse(lines_input).get("Port status", {})

def show_counter_full_parse(lines_input:str):
    ret = dune_table_parse(lines_input, main_key_index=0, sub_key_index=1).get("Full counters", {})
    for port, port_value in ret.items():
        for counter, counter_value in port_value.items():
            for count_type, count_value in counter_value.items():
                if count_value == "N/A":
                    counter_value[count_type] = 0
                else:
                    counter_value[count_type] = int(count_value.replace(',', ''))
    return ret

def dune_prbs_get_parse(lines_input:str):
    lines = lines_input.splitlines()
    result = dict()
    for line in lines:
        if len(line) == 0:
            continue
        line_split = line.split(':')
        if len(line_split) != 2:
            continue
        port_field = line_split[0]
        prbs_field = line_split[1]
        port_pattern = '(eth\d+) \(lane (\d+)\)'
        matchs = re.match(pattern=port_pattern, string=port_field)
        if matchs == None or len(matchs.groups()) != 2:
            continue
        eth_port = matchs.group(1)
        lane_index = int(matchs.group(2))
        if result.get(eth_port, None) == None:
            result[eth_port] = dict()
        result[eth_port][lane_index] = {"log": line}
        if prbs_field.find("not LOCKED") >= 0:
            result[eth_port][lane_index].update({
                "status": PRBS_NOT_LOCKED,
            })
            continue
        if prbs_field.find("no error") >= 0:
            result[eth_port][lane_index].update({
                "status": PRBS_LOCKED,
                "ber": 0,
                "error": 0,
            })
            continue
        prbs_pattern = "PRBS FAILED with (\d+) errors. BER=(\S+)!"
        matchs = re.match(prbs_pattern, prbs_field.strip())
        result[eth_port][lane_index].update({
            "status": PRBS_LOCKED,
            "error": matchs.group(1),
            "ber": matchs.group(2),
        })
    return result

class PortSdkCmdDune(PortSdkCmdBase):
    def __init__(self, redirect: bool, dport_start_index=1, cmd_util="bcmcmdb"):
        super().__init__(redirect, dport_start_index, cmd_util)
        self.default_vlan = 1
        self.poly_modes_map = {
            PRBS_POLY_P7: "X7_X6_1",
            PRBS_POLY_P9: "X9_X5_1",
            PRBS_POLY_P10: "X10_X7_1",
            PRBS_POLY_P11: "X11_X9_1",
            PRBS_POLY_P13: "X13_X12_X2_1",
            PRBS_POLY_P15: "X15_X14_1",
            PRBS_POLY_P20: "X20_X3_1",
            PRBS_POLY_P23: "X23_X18_1",
            PRBS_POLY_P31: "X31_X28_1",
            PRBS_POLY_P49: "X49_X40_1",
            PRBS_POLY_P58: "X58_X31_1",
        }
        self.counter_keys_map = {
            TX_FCS_ERROR_FRAME: "TX FCS error frame",
            RX_FCS_ERROR_FRAME: "RX FCS error frame",
            TX_ALL_PKT_FRAME: "TX all packets frame",
            RX_ALL_PKT_FRAME: "RX all packets frame",
            TX_BYTE_FRAME: "TX Byte frame",
            RX_BYTE_FRAME: "RX Byte frame"
        }
        self.loopback_modes_map = {
            LOOPBACK_MODE_NONE: "None",
            LOOPBACK_MODE_MAC: "mac",
            LOOPBACK_MODE_PHY: "phy",
            LOOPBACK_MODE_RMT: "rmt",
            LOOPBACK_MODE_MAC_RMT: "mac_rmt",
            LOOPBACK_MODE_NIF: "nif"
        }
        self.stp_state_map = {
            STG_STP_DISABLE: "disable",
            STG_STP_BLOCK: "block",
            STG_STP_LISTEN: "listen",
            STG_STP_LEARN: "learn",
            STG_STP_FORWARD: "forward"
        }
        self.update_port_status()

    def exec_bcmcmd(self, cmd: str, timeout=10):
        ret, output = super().exec_bcmcmd(cmd, timeout=timeout)
        if ret != True:
            return False, output
        if (output.find("is not supported for current device and/or configuration")>=0):
            return False, output
        return True, output

    def update_port_status(self):
        ret, output = self.exec_bcmcmd("port status")
        if ret != True:
            logging.error("update_port_status failed")
            return False, output
        port_status_dict = port_status_parse(output)
        self.port_status = dict()
        self.port_map = dict()
        dport = self.dport_start_index
        for port_name, port_status in port_status_dict.items():
            pattern = "(eth\d+)\((\d+)\)"
            matchs = re.match(pattern=pattern, string=port_name)
            if matchs == None or len(matchs.groups()) != 2:
                continue
            self.port_status[dport] = port_status # TODO, 需要按设计文档格式化输出
            self.port_map[dport] = {
                "port": port_name,
                "lport": matchs.group(2), # type: str
                "eth_port": matchs.group(1)
            }
            dport +=1
        return True, ""

    def show_counter_full(self):
        ret, output = self.exec_bcmcmd("show counters full")
        if ret != True:
            logging.warning("show_counter_full failed, output: %s" % output)
            return False, output
        return True, output

    def update_packets_counters(self):
        ret, output = self.show_counter_full()
        if ret is False:
            logging.error("update_packets_counters failed")
            return False, output
        default_dict = {
            COUNTER_VALUE: 0,
            COUNTER_DIFF: 0,
            COUNTER_RATE: 0
        }
        show_counter_full_dict = show_counter_full_parse(output)
        self.packets_counters = dict.fromkeys(self.get_dport_list())
        for dport in self.get_dport_list():
            self.packets_counters[dport] = dict()
            eth_port = self.get_eport_by_dport(dport)
            eport_counter_full_dict = show_counter_full_dict.get(eth_port, dict())
            for key, counter in self.counter_keys_map.items():
                self.packets_counters[dport][key] = eport_counter_full_dict.get(counter, default_dict)
        return True, copy.deepcopy(self.packets_counters)

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
        ret, output = self.exec_bcmcmd("stg STP set ID={stg} port={lports} state={state}".format(**stg_cmd))
        if ret != True:
            logging.warning("port_stp_config failed, output: %s" % output)
            return False, output
        return True, output

    def port_pvid_set(self, dports: list[int], vlan_id: int):
        unit = 0
        cint_code_list = list()
        for dport in dports:
            lport = self.get_lport_by_dport(dport)
            cint_code = "print bcm_port_untagged_vlan_set({}, {}, {});\n".format(unit, lport, vlan_id)
            cint_code_list.append(cint_code)
        file_path = "/tmp/pvid_set.cint"
        with open(file_path, "w") as file:
            file.writelines(cint_code_list)
        ret, output = self.exec_bcmcmd_cint(cint_file_path=file_path)
        if ret != True:
            logging.warning("port_pvid_set failed, output: %s" % output)
            return False, output
        ret, output = self.cint_output_parse(output)
        if len(output) != len(dports):
            logging.warning("port_pvid_set failed, output: %s" % output)
            return False, output
        for port_ret in output:
            if port_ret.get("value") != 0:
                logging.warning("port_pvid_set failed, output: %s" % output)
                return False, output
        return True, output

    def l2_add(self, dport: int, vlan_id: int, mac: str, **kwargs):
        lport = self.get_lport_by_dport(dport)
        if int(lport) >=33:
            core_group = "core_group=1"
        else:
            core_group = ""
        l2_add_cmd = {
            "pbm": lport,
            "vlan": vlan_id,
            "mac": mac,
            "fwd": "",
            "core_group": core_group
        }
        if kwargs.get("fwd", None) != None:
            l2_add_cmd["fwd"] = "fwd={}".format(kwargs["fwd"])
        ret, output = self.exec_bcmcmd("l2 add portbitmap={pbm} vlan={vlan} mac={mac} {fwd} {core_group}".format(**l2_add_cmd))
        if ret != True:
            logging.warning("l2_add failed, output: %s" % output)
            return False, output
        return True, output

class PortPrbsTestDune(PortPrbsTest):
    u'''dune芯片端口Prbs测试类'''
    def __init__(self, port_scene: PortSceneBase):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdDune(port_scene.redirect, self.port_scene.dport_start_index)

    def collect_result(self):
        ret, output = self.sdk_cmd_util.prbs_poly_get(self.get_dport_list())
        if ret != True:
            logging.error("PortPrbsTestDune collect_result failed")
            return False, output
        prbs_get_output_parsed = dune_prbs_get_parse(output)
        self.prbs_get_output_parsed = {}
        for eth, eth_result in prbs_get_output_parsed.items():
            dport = self.sdk_cmd_util.get_dport_by_eth(eth)
            self.prbs_get_output_parsed[dport] = eth_result
        return True, ""
    
class PortBrcstTestDune(PortBrcstTest):
    u'''dune芯片端口广播测试类'''
    def __init__(self, port_scene: PortSceneBase):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdDune(port_scene.redirect, self.port_scene.dport_start_index)

    def collect_result(self):
        # 收集show counter full的输出
        ret, output = self.sdk_cmd_util.update_packets_counters()
        if ret is False:
            logging.error("PortBrcstTestDune collect_result failed")
            return False, output
        self.packets_counters = output
        return True, ""
    
class PortFrameTestDune(PortFrameTest):
    u'''dune芯片端口收发帧测试类'''
    def __init__(self, port_scene: PortSceneBase):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdDune(port_scene.redirect, self.port_scene.dport_start_index)

    def collect_result(self) -> {bool, str}:
        # 收集show counter full的输出
        ret, output = self.sdk_cmd_util.update_packets_counters()
        if ret is False:
            logging.error("PortFrameTestDune collect_result failed")
            return False, output
        self.packets_counters = output
        return True, ""

class PortKrTestDune(PortKrTest):
    u'''dune芯片端口收发帧测试类'''
    def __init__(self, port_scene: PortSceneBase):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdDune(port_scene.redirect, self.port_scene.dport_start_index)

class PortSnakeTestDune(PortSnakeTest):
    u'''dune芯片端口自发包蛇形测试类'''
    def __init__(self, port_scene: PortSceneBase):
        super().__init__(port_scene)
        self.sdk_cmd_util = PortSdkCmdDune(port_scene.redirect, self.port_scene.dport_start_index)

    def stop_test(self):
        u'''端口自发包蛇形测试结束'''
        logging.debug("端口自发包蛇形测试结束")
        # 收集测试结果
        ret, output = self.collect_result()
        if ret is False:
            logging.error("PortSnakeTest stop_test failed")
            return False, output
        # 停止流量转发
        ret, output = self.stop_send_port_packets()
        if ret is False:
            logging.error("PortSnakeTest stop_test failed")
            return False, output
        del_time = self.port_scene.get_mftport_config(FRAME_DEL_TIME_CFG, 10)
        self.sleep(del_time)
        return True, ""

    def collect_result(self):
        # 收集端口updown状态
        ret, output = self.sdk_cmd_util.update_port_status()
        if ret is False:
            logging.error("PortSnakeTest stop_test failed")
            return False, output
        # 收集show counter full的输出
        ret, output = self.sdk_cmd_util.update_packets_counters()
        if ret is False:
            logging.error("PortBrcstTestDune collect_result failed")
            return False, output
        self.packets_counters = output
        return True, ""

class PortSceneDune(PortSceneBase):
    def __init__(self, mftport_config: dict):
        super().__init__(mftport_config)
        self.test_class_map.update({
            PORT_PRBS_TEST_CLASS: PortPrbsTestDune,
            PORT_BRCST_TEST_CLASS: PortBrcstTestDune,
            PORT_FRAME_TEST_CLASS: PortFrameTestDune,
            PORT_KR_TEST_CLASS: PortKrTestDune,
            PORT_SNAKE_TEST_CLASS: PortSnakeTestDune
        })

if __name__== "__main__":
    mft_portconfig = {
        # 指定待测端口
        "dport_start_index": 1,
        "dport_list": list(range(55, 65)),
        "cpu_mac_portmap": {
            "eth1": "eth65",
            "eth2": "eth67",
            "eth3": "eth70",
            "eth4": "eth71",
        },
        # prbs测试允许的误码率
        "prbs_ber": 1.0e-9,
        # prbs测试时间 (通常为120s/180s)
        "prbs_time": 5,
        "prbs_ber_dict": {
            1: {
                1: 1.00E-8,
                3: 1.00E-8,
            },
            3: {
                3: 1.00E-8
            },
            4: {
                1: 1.00E-8
            }
        },  # 设备单个端口对应的误码率
        "prbs_poly": "p31",
        "port_frame_test_retrynum": 1,  # 端口收发帧重试次数(内部重试)
        # "port_frame_test_edb_port_list": "1-128",
        "port_brcst_test_retrynum": 1,  # 端口广播测试重试次数(内部重试)
        "port_prbs_test_retrynum": 1,  # 端口PRBS测试重试次数(内部重试)
        "port_kr_test_retrynum": 1,  # 内部管理口测试重试次数(内部重试)
        "port_frame_del_time": 10,  # 端口收发帧恢复测试环境等待时间(s)
        "port_brcst_del_time": 10,  # 端口广播测试恢复测试环境等待时间(s)
        "port_prbs_del_time": 10,  # 端口PRBS测试恢复测试环境等待时间(s)
        "port_kr_del_time": 10,  # 内部管理口测试恢复测试环境等待时间(s)
        "port_log_level": 1,  # PORT组件log级别(DEBUG : 1, INFO : 2, WARNING : 3, ERROR : 4)
    }
    port_scene_obj = PortSceneDune(mft_portconfig)
    test_item_map = {
        "收发帧测试": {"func": port_scene_obj.port_frame_test},
        "广播测试": {"func": port_scene_obj.port_brcst_test},
        "PRBS测试": {"func": port_scene_obj.port_prbs_test},
        "KR收发帧测试": {"func": port_scene_obj.port_kr_test},
        "蛇形打流测试开始": {"func": port_scene_obj.port_snake_test},
        "蛇形打流测试停止": {"func": port_scene_obj.port_snake_test_stop},
        "蛇形打流测试结果": {"func": port_scene_obj.port_snake_test_result},
    }
    test_item_list = list(test_item_map.keys())
    test_item_cnt = len(test_item_list)
    while True:
        print("\n 端口生测单元测试")
        for test_item_key_index in range(test_item_cnt):
            print("{}. {}".format(test_item_key_index+1, test_item_list[test_item_key_index]))
        print("q. 退出")
        choice = input("请输入选项:")
        if choice == 'q':
            break
        if len(choice) <= 0:
            continue
        try:
            choice = int(choice)-1
        except:
            print("无效的选项，请重新输入。")
            continue
        if choice < 0 or choice > test_item_cnt:
            print("无效的选项，请重新输入。")
            continue
        test_item_key = test_item_list[choice]
        try:
            test_func = test_item_map[test_item_key]["func"]
            output = test_func(redirect=False)
        except Exception as e:
            print(traceback.format_exc())
        if isinstance(output, dict):
            prbs_result_dict = output.get("prbs_result_dict", None)
            if prbs_result_dict != None:
                output = prbs_result_dict
            test_result = output.get("test_result", None)
            print("test_result: {}".format("Pass" if test_result else "Failed"))
            successports = output.get("successports", [])
            errorports = output.get("errorports", [])
            updownerrorports = output.get("updownerrorports", [])
            port_output_func = lambda port_list: [port_list[i:i+8] for i in range(0, len(port_list), 8)]
            print("successports:")
            for line_ports in port_output_func(successports):
                print(' '.join(map(lambda port: "Port{}".format(port), line_ports)))
            print("errorports:")
            for line_ports in port_output_func(errorports):
                print(' '.join(map(lambda port: "Port{}".format(port), line_ports)))
            print("updownerrorports:")
            for line_ports in port_output_func(updownerrorports):
                print(' '.join(map(lambda port: "Port{}".format(port), line_ports)))
            prbs_info = output.get("prbs_info", [])
            print("prbs_info: \n{}".format(prbs_info))
            print("log: \n")
            port_info_dict = output.get("port_info_dict", None)
            if port_info_dict != None:
                for key, value in port_info_dict.items():
                    print("port{}: {}".format(key, value["log"]))
        else:
            print("{}: {}".format(test_item_key, output))
