# sfputil.py
#
# Platform-specific SFP transceiver interface for SONiC
#

try:
    import time
    import subprocess
    import re
    import os
    import threading
    import traceback
    from sonic_sfp.sfputilbase import SfpUtilBase
except ImportError as e:
    raise ImportError("%s - required module not found" % str(e))

class SfpUtil(SfpUtilBase):
    """Platform-specific SfpUtil class"""

    PORT_START = 0
    PORT_END = 63
    PORTS_IN_BLOCK = 64

    EEPROM_OFFSET = 25
    SFP_DEVICE_TYPE = "optoe2"
    QSFP_DEVICE_TYPE = "optoe1"
    I2C_MAX_ATTEMPT = 3

    SFP_STATUS_INSERTED = '1'
    SFP_STATUS_REMOVED = '0'

    MODULE_TYPE_SFP    = 1
    MODULE_TYPE_QSFP   = 2
    MODULE_TYPE_QSFPDD = 3

    ERR_NOT_PRESENT = -1
    ERR_READ_E2     = -2

    _port_to_eeprom_mapping = {}
    port_to_i2cbus_mapping ={}
    port_dict = {}
    port_dict_temp = {}

    qsfp_ports_list = []
    osfp_ports_list = []

    @property
    def port_start(self):
        return self.PORT_START

    @property
    def port_end(self):
        return self.PORT_END

    @property
    def qsfp_ports(self):
        return self.qsfp_ports_list

    @property
    def osfp_ports(self):
        return self.osfp_ports_list

    @property
    def port_to_eeprom_mapping(self):
        return self._port_to_eeprom_mapping

    def __init__(self):
        for x in range(self.PORT_START, self.PORTS_IN_BLOCK):
            self.port_to_i2cbus_mapping[x] = (x + self.EEPROM_OFFSET)
        # self.port_to_i2cbus_mapping = {
        #         1:25,2:26,3:27,4:28,5:29,6:30,7:31,8:32,
        #         9:33,10:34,11:35,12:36,13:37,14:38,15:39,16:40,17:41,18:42,
        #         19:43,20:44,21:45,22:46,23:47,24:48,25:49,26:50,27:51,28:52,
        #         29:53,30:54,31:55,32:56,33:57,34:58,35:59,36:60,37:61,38:62,
        #         39:63,40:64,41:65,42:66,43:67,44:68,45:69,46:70,47:71,48:72,
        #         49:73,50:74,51:75,52:76,53:77,54:78,55:79,56:80,57:81,58:82,
        #         59:83,60:84,61:85,62:86,63:87,64:88
        #     }
        for x in range(self.PORT_START, self.PORTS_IN_BLOCK):
            if self.get_presence(x):
                self.port_dict_temp[x] = self.SFP_STATUS_INSERTED
            else:
                self.port_dict_temp[x] = self.SFP_STATUS_REMOVED
        self.update_ports_list()
        SfpUtilBase.__init__(self)

    def _sfp_read_file_path(self, file_path, offset, num_bytes):
        attempts = 0
        while attempts < self.I2C_MAX_ATTEMPT:
            try:
                file_path.seek(offset)
                read_buf = file_path.read(num_bytes)
            except:
                attempts += 1
                time.sleep(0.05)
            else:
                return True, read_buf
        return False, None

    def _sfp_eeprom_present(self, sysfs_sfp_i2c_client_eeprompath, offset):
        """Tries to read the eeprom file to determine if the
        device/sfp is present or not. If sfp present, the read returns
        valid bytes. If not, read returns error 'Connection timed out"""

        if not os.path.exists(sysfs_sfp_i2c_client_eeprompath):
            return False
        else:
            with open(sysfs_sfp_i2c_client_eeprompath, "rb", buffering=0) as sysfsfile:
                rv, buf = self._sfp_read_file_path(sysfsfile, offset, 1)
                return rv

    def _add_new_sfp_device(self, sysfs_sfp_i2c_adapter_path, devaddr, devtype):
        try:
            sysfs_nd_path = "%s/new_device" % sysfs_sfp_i2c_adapter_path

            # Write device address to new_device file
            nd_file = open(sysfs_nd_path, "w")
            nd_str = "%s %s" % (devtype, hex(devaddr))
            nd_file.write(nd_str)
            nd_file.close()

        except Exception as err:
            print("Error writing to new device file: %s" % str(err))
            return 1
        else:
            return 0

    def _get_port_eeprom_path(self, port_num, devid):
        return "/sys/s3ip/transceiver/eth%d/eeprom" % (port_num + 1)

    def _read_eeprom_specific_bytes(self, sysfsfile_eeprom, offset, num_bytes):
        #if self.check_power_on() is False:
        #    return None

        eeprom_raw = []
        for i in range(0, num_bytes):
            eeprom_raw.append("0x00")

        rv, raw = self._sfp_read_file_path(sysfsfile_eeprom, offset, num_bytes)
        if rv == False:
            return None

        try:
            for n in range(0, num_bytes):
                eeprom_raw[n] = hex(raw[n])[2:].zfill(2)
        except:
            return None

        return eeprom_raw

    def get_eeprom_dom_raw(self, port_num):
        if port_num in self.qsfp_ports:
            # QSFP DOM EEPROM is also at addr 0x50 and thus also stored in eeprom_ifraw
            return None
        else:
            # Read dom eeprom at addr 0x51
            return self._read_eeprom_devid(port_num, self.IDENTITY_EEPROM_ADDR, 256)

    def get_presence(self, port_num):
        # Check for invalid port_num
        if port_num < self.port_start or port_num > self.port_end:
            return False

        # cmd = "cat /sys/rg_plat/sff/sff{}/present".format(str(port_num+1))
        cmd = "cat /sys/s3ip/transceiver/eth{}/present".format(str(port_num+1)) 
        ret, output = subprocess.getstatusoutput(cmd)
        if ret != 0:
            return False
        if output == "1":
            return True
        return False

    def get_low_power_mode(self, port_num):
        # Check for invalid port_num

        return True

    def set_low_power_mode(self, port_num, lpmode):
        # Check for invalid port_num

        return True

    def reset(self, port_num):
        # Check for invalid port_num
        if port_num < self.port_start or port_num > self.port_end:
            return False

        return True

    def check_module_type(self, port_num):
        result = 0
        try:
            if self.get_presence(port_num) == False:
                return self.ERR_NOT_PRESENT

            eeprom_path = self._get_port_eeprom_path(port_num, 0x50)
            with open(eeprom_path, mode="rb", buffering=0) as eeprom:
                eeprom_raw = self._read_eeprom_specific_bytes(eeprom, 0, 1)
                # according to sff-8024 A0h Byte 0 is '1e' or '18' means the transceiver is qsfpdd
                if (eeprom_raw[0] == '1e' or eeprom_raw[0] == '18' or eeprom_raw[0] == '19'):
                    result = self.MODULE_TYPE_QSFPDD
                elif (eeprom_raw[0] == '11' or eeprom_raw[0] == '0d'):
                    result = self.MODULE_TYPE_QSFP
                elif (eeprom_raw[0] == '03'):
                    result = self.MODULE_TYPE_SFP
                else:
                    result = self.ERR_READ_E2
        except Exception as e:
            print(traceback.format_exc())
            result = self.ERR_READ_E2

        return result

    def update_ports_list(self):
        self.qsfp_ports_list = []
        self.osfp_ports_list = []
        for x in range(self.PORT_START, self.PORTS_IN_BLOCK):
            ret = self.check_module_type(x)
            if (ret == self.ERR_NOT_PRESENT):
                self.port_dict_temp[x] = self.SFP_STATUS_REMOVED
            elif (ret == self.MODULE_TYPE_QSFPDD):
                self.osfp_ports_list.append(x)
            elif (ret == self.MODULE_TYPE_QSFP):
                self.qsfp_ports_list.append(x)

    def get_transceiver_change_event(self, timeout=0):
        start_time = time.time()
        current_port_dict = {}
        forever = False

        if timeout == 0:
            forever = True
        elif timeout > 0:
            timeout = timeout / float(1000) # Convert to secs
        else:
            print ("get_transceiver_change_event:Invalid timeout value", timeout)
            return False, {}

        end_time = start_time + timeout
        if start_time > end_time:
            print ('get_transceiver_change_event:' \
                       'time wrap / invalid timeout value', timeout)

            return False, {} # Time wrap or possibly incorrect timeout

        while timeout >= 0:
            # Check for OIR events and return updated port_dict
            for x in range(self.PORT_START, self.PORTS_IN_BLOCK):
                if self.get_presence(x):
                    current_port_dict[x] = self.SFP_STATUS_INSERTED
                else:
                    current_port_dict[x] = self.SFP_STATUS_REMOVED
            if (current_port_dict == self.port_dict):
                if forever:
                    time.sleep(1)
                else:
                    timeout = end_time - time.time()
                    if timeout >= 1:
                        time.sleep(1) # We poll at 1 second granularity
                    else:
                        if timeout > 0:
                            time.sleep(timeout)
                        self.update_ports_list()
                        return True, {}
            else:
                self.update_ports_list()
                # Update reg value
                self.port_dict = current_port_dict
                return True, self.port_dict
        print ("get_transceiver_change_event: Should not reach here.")
        return False, {}

    def check_is_qsfpdd(self, port_num):
        try:
            if self.get_presence(port_num) == False:
                return False

            eeprom_path = self._get_port_eeprom_path(port_num, 0x50)
            with open(eeprom_path, mode="rb", buffering=0) as eeprom:
                eeprom_raw = self._read_eeprom_specific_bytes(eeprom, 0, 1)
                if eeprom_raw is None:
                    return False
                if (eeprom_raw[0] == '1e' or eeprom_raw[0] == '18'):
                    return True
        except Exception as e:
            #print(traceback.format_exc())
            pass

        return False

    def check_power_on(self):
        power_on_flag = False
        power_on_path = "/tmp/power_on"

        if os.path.exists(power_on_path) is True:
            power_on_flag = True
            return power_on_flag

        cmd = "sonic-db-cli -n STATE_DB HGET 'SYSTEM_STATUS_TABLE|SDK_STATUS' status"
        ret, output = subprocess.getstatusoutput(cmd)
        if ret == 0 and "READY" in output:
            with open(power_on_path, 'w') as sfp_f:
                pass
            power_on_flag = True
            return power_on_flag

        return power_on_flag

    def get_highest_temperature(self):
        offset = 0
        hightest_temperature = -9999

        presence_flag = False
        read_eeprom_flag = False
        temperature_valid_flag = False

        # device is not powered on
        #if self.check_power_on() is False:
        #    return -10000

        for port in range(self.PORT_START, self.PORTS_IN_BLOCK):
            if self.get_presence(port) == False:
                continue
            if (self.port_dict_temp[port] == self.SFP_STATUS_REMOVED):
                ret = self.check_module_type(port)
                if (ret == self.ERR_NOT_PRESENT):
                    self.port_dict_temp[port] = self.SFP_STATUS_REMOVED
                    continue
                elif (ret == self.MODULE_TYPE_QSFPDD):
                    self.osfp_ports_list.append(port)
                elif (ret == self.MODULE_TYPE_QSFP):
                    self.qsfp_ports_list.append(port)
                elif (ret == self.MODULE_TYPE_SFP):
                    #print("port %d module type is sfp" % (port))
                    pass
                else:
                    print("port %d read eeprom err" % (port))
                    continue

            presence_flag = True

            if port in self.osfp_ports:
                offset = 14
            elif port in self.qsfp_ports:
                offset = 22
            else:
                offset = 96

            eeprom_path = self._get_port_eeprom_path(port, 0x50)
            try:
                with open(eeprom_path, mode="rb", buffering=0) as eeprom:
                    read_eeprom_flag = True
                    eeprom_raw = self._read_eeprom_specific_bytes(eeprom, offset, 2)
                    msb = int(eeprom_raw[0], 16)
                    lsb = int(eeprom_raw[1], 16)

                    result = (msb << 8) | (lsb & 0xff)
                    if ((result & (1 << (16 - 1))) != 0):
                        result = result - (1 << 16)
                    result = float(result / 256.0)
                    if -50 <= result <= 200:
                        temperature_valid_flag = True
                        if hightest_temperature < result:
                            hightest_temperature = result
            except Exception as e:
                #print(traceback.format_exc())
                pass

        # all port not presence
        if presence_flag == False:
            hightest_temperature = -10000

        # all port read eeprom fail
        elif read_eeprom_flag == False:
            hightest_temperature = -9999

        # all port temperature invalid
        elif read_eeprom_flag == True and temperature_valid_flag == False:
            hightest_temperature = -10000

        hightest_temperature = round(hightest_temperature, 2)

        return hightest_temperature
