LOG_FILE_PATH = '/var/log/factest_port.log'
LOG_ZIP_FILE_PATH = '/var/log/factest_port.zip'

PORT_PRBS_TEST_CLASS = "port_prbs_test"
PORT_BRCST_TEST_CLASS = "port_brcst_test"
PORT_FRAME_TEST_CLASS = "port_frame_test"
PORT_KR_TEST_CLASS = "port_kr_test"
PORT_SNAKE_TEST_CLASS = "port_snake_test"

# mftport上层应用配置相关
LOG_LEVEL_CFG = "port_log_level"
DPORT_LIST_CFG = "dport_list"
DPORT_START_INDEX_CFG = "dport_start_index"
PRBS_BER_CFG = "prbs_ber"
PRBS_TIME_CFG = "prbs_time"
PRBS_BER_DICT_CFG = "prbs_ber_dict"
PRBS_POLY_CFG = "prbs_poly"
PRBS_DEL_TIME_CFG = "port_prbs_del_time"
BRCST_DEL_TIME_CFG = "port_brcst_del_time"
FRAME_DEL_TIME_CFG = "port_frame_del_time"
CPU_MAC_PORTMAP_CFG = "cpu_mac_portmap"
FRAME_TEST_RETRYNUM_CFG = "port_frame_test_retrynum"
BRCST_TEST_RETRYNUM_CFG = "port_brcst_test_retrynum"
PRBS_TEST_RETRYNUM_CFG = "port_prbs_test_retrynum"
KR_TEST_RETRYNUM_CFG = "port_kr_test_retrynum"

# 端口属性相关
PORT_LINK_UP = "up"
PORT_LINK_DOWN = "down"
PORT_LINK_DISABLE = "!ena"

PRBS_LOCKED = True
PRBS_NOT_LOCKED = False

PRBS_POLY_P7 = "p7"
PRBS_POLY_P9 = "p9"
PRBS_POLY_P10 = "p10"
PRBS_POLY_P11 = "p11"
PRBS_POLY_P13 = "p13"
PRBS_POLY_P15 = "p15"
PRBS_POLY_P20 = "p20"
PRBS_POLY_P23 = "p23"
PRBS_POLY_P31 = "p31"
PRBS_POLY_P49 = "p49"
PRBS_POLY_P58 = "p58"

STG_STP_FORWARD = "forward"
STG_STP_DISABLE = "disable"
STG_STP_LISTEN = "listen"
STG_STP_BLOCK = "block"
STG_STP_LEARN = "learn"

LOOPBACK_MODE_NONE = "none"
LOOPBACK_MODE_MAC = "mac"
LOOPBACK_MODE_PHY = "phy"
LOOPBACK_MODE_RMT = "rmt"
LOOPBACK_MODE_MAC_RMT = "mac_rmt"
LOOPBACK_MODE_NIF = "nif"

TX_ALL_PKT_FRAME = "tx_all_pkt_frame"
RX_ALL_PKT_FRAME = "rx_all_pkt_frame"
TX_FCS_ERROR_FRAME = "tx_fcs_error_frame"
RX_FCS_ERROR_FRAME = "rx_fcs_error_frame"
TX_BYTE_FRAME = "tx_byte_frame"
RX_BYTE_FRAME = "rx_byte_frame"

G_BITS = 1000000000
COUNTER_VALUE = "Counter Value"
COUNTER_DIFF = "Diff from last call"
COUNTER_RATE = "Rate"