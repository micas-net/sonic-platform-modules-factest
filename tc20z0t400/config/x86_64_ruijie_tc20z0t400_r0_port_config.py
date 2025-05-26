#!/usr/bin/python3
# -*- coding: UTF-8 -*-

PLATFORM_INTF_OPTOE = {
    "port_num": 10,
    "port_bus_map": {
        1  :48,
        2  :49,
        3  :50,
        4  :51,
        5  :52,
        6  :53,
        7  :54,
        8  :55,
        9  :56,
        10 :57,
    }
}

port_config = {
    # 模块显示
    "sfps": {
        "ver": '2.0',
        "port_index_start": 1,
        "port_num": 42,
        "bp_port_list": "11-42",
        "log_level": 2,
        "eeprom_retry_times": 5,
        "eeprom_retry_break_sec": 0.2,
        "presence_path": "/sys/s3ip/transceiver/eth%d/present",
        "presence_val_is_present": 1,
        "eeprom_path": "/sys/s3ip/transceiver/eth%d/eeprom",
        "lpmode_path": "/sys/s3ip/transceiver/eth%d/low_power_mode",
        "reset_path": "/sys/s3ip/transceiver/eth%d/reset",
        "reset_val_is_reset": 0,
        "tx_dis_path": "/sys/s3ip/transceiver/eth%d/tx_disable",
        "rx_los_path": "/sys/s3ip/transceiver/eth%d/rx_los",
        "tx_fault_path": "/sys/s3ip/transceiver/eth%d/tx_fault",
    },
    # 生测使用配置
    "mft_port" : {
        # 是否是Hsdk设备(是 : 1, 不是 : 0)
        "hsdk_device": 1,
        # 内部管理口对应的unit_port
        "mgmt_kr_ports": {"eth1":64, "eth2":174},
        # cpu口收包的关键字
        "test_kr_eth_rx_keyword": "RX packets",
        # 面板口unit_port范围(不包含mgmt口和loopback口)
        "prbs_port_range": "1-63,66-173",
        # 是否是有外部phy的设备(是 : 1, 不是 : 0)
        "extphy_device": 0,
        # prbs测试允许的误码率
        "prbs_ber": 1.0e-9,
        # prbs测试时间 (通常为120s/180s)
        "prbs_time": 180,
        "port_frame_test_retrynum": 1,  # 端口收发帧重试次数(内部重试)
        "port_frame_test_edb_port_list": "1-8,11-42",
        "port_brcst_test_retrynum": 1,  # 端口广播测试重试次数(内部重试)
        "port_prbs_test_retrynum": 1,  # 端口PRBS测试重试次数(内部重试)
        "port_kr_test_retrynum": 1,  # 内部管理口测试重试次数(内部重试)
        "port_frame_del_time": 10,  # 端口收发帧恢复测试环境等待时间(s)
        "port_brcst_del_time": 10,  # 端口广播测试恢复测试环境等待时间(s)
        "port_prbs_del_time": 10,  # 端口PRBS测试恢复测试环境等待时间(s)
        "port_kr_del_time": 10,  # 内部管理口测试恢复测试环境等待时间(s)
        "port_log_level": 1,  # PORT组件log级别(DEBUG : 1, INFO : 2, WARNING : 3, ERROR : 4)
    },
}

