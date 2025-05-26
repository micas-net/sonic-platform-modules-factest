#!/usr/bin/python3

devices = {
    "onie_e2": [
        {
            "name": "ONIE_E2",
            "e2loc": {"loc": "/sys/bus/i2c/devices/0-0056/eeprom", "way": "sysfs"},
            "airflow": "intake"
        },
    ],
    "sfps": {
        "ver": '2.0',
        "port_index_start": 0,
        "port_num": 40,
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
    }
}
