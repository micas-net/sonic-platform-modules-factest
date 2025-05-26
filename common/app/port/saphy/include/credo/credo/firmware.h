#ifndef CREDO_FW_H
#define CREDO_FW_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Display infromation from firmware
 * @deprecated Alias of display_slice_info
 * @ingroup Firmware
 * @param[in] slice slice handle
 * @param[in] command commamnd string to use
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_display_info(CredoSlice_t* slice, const char* command);

// Firmware Management
/**
 * @brief Unload firmware on the slice
 * @ingroup ExFirmwareManagement
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_unload(CredoSlice_t* slice);

/**
 * @brief Load firmware onto slice
 * @ingroup ExFirmwareManagement
 * @param[in] slice slice handle
 * @param[in] image_file file path to firmware
 * @param[in] force force if firmware is already loaded
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_load(CredoSlice_t* slice, const char* image_file, int force);

/**
 * @brief Firmware load onto
 * @ingroup Firmware
 * @param[in] slices slices to load to simultaneously
 * @param[in] slice_count how many slices to load
 * @param[in] image_file file path to firmware binary
 * @param[in] delay_time_us delay between write frames before writing the next frame
 * @param[in] force force if firmware is already loaded
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_load_broadcast(CredoSlice_t* slices[], int slice_count, const char* image_file,
                                                      unsigned delay_time_us, int force);

/**
 * @brief Wait for firmware to provide magic word to indicat it is ready
 * @ingroup ExFirmwareStatus
 * @param[in] slice slice handle
 * @param[in] timeout how long to wait usec before exiting
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_wait_magic_word(CredoSlice_t* slice, unsigned timeout);

/**
 * @brief Wait for firmware to complete top pll calibration
 * @ingroup  ExFirmwareStatus
 * @param[in] slice slice handle
 * @param[in] timeout how long to wait usec before exiting
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_wait_top_pll_cal(CredoSlice_t* slice, unsigned timeout);

/**
 * @brief Get firmware running status
 * @ingroup Firmware
 * @param[in] slice slice handle
 * @param[out] status firmware status
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_firmware_get_status(CredoSlice_t* slice, unsigned* status);

// Firmware Information

/**
 * @brief Get loaded firmware magic word
 * @ingroup ExFirmwareInformation
 * @param[in] slice slice handle
 * @param[out] magic magic word value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_magic(CredoSlice_t* slice, unsigned* magic);

/**
 * @brief Get the firmware version
 * @ingroup Firmware
 * @param[in] slice slice handle
 * @param[out] version firmware version encoded
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_version(CredoSlice_t* slice, unsigned* version);

/**
 * @brief Get the firmware version as a human readable string
 * @ingroup Firmware
 * @param[in] slice slice handle
 * @param[out] version firmware version string
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_version_str(CredoSlice_t* slice, char version[32]);

/**
 * @brief Get the firmware hash value
 * @ingroup ExFirmwareInformation
 * @param[in] slice slice handle
 * @param[out] hash firmware hash
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_hash(CredoSlice_t* slice, unsigned* hash);

/**
 * @brief Get the firmware crc code
 * @ingroup ExFirmwareInformation
 * @param[in] slice slice handle
 * @param[out] crc crc value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_crc(CredoSlice_t* slice, unsigned* crc);

/**
 * @brief Get the firmware release date
 * @ingroup ExFirmwareInformation
 * @param[in] slice slice handle
 * @param[out] date firmware date
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_date(CredoSlice_t* slice, unsigned* date);

// Firmware Commands

/**
 * @brief Run a firmware command
 * @ingroup ExFirmwareCommands
 * @param[in] slice slice handle
 * @param[in] cmd command to run
 * @param[in] param parameter for command
 * @param[out] response response status
 * @param[out] response_param response value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_cmd(CredoSlice_t* slice, unsigned cmd, unsigned param, unsigned* response,
                                           unsigned* response_param);

/**
 * @brief Run a firmware command w/ extra parameter
 * @ingroup ExFirmwareCommands
 * @param[in] slice slice handle
 * @param[in] cmd command to run
 * @param[in] param1 parameter 1 for command
 * @param[in] param2 parameter 2 for command
 * @param[out] response response status
 * @param[out] response_param1 response value 1
 * @param[out] response_param2 response value 2
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_cmd_ex(CredoSlice_t* slice, unsigned cmd, unsigned param1, unsigned param2,
                                              unsigned* response, unsigned* response_param1, unsigned* response_param2);

/**
 * @brief Subset of firmware commands for debug
 * @ingroup ExFirmwareCommands
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[in] section section to select
 * @param[in] index index to select
 * @param[out] response_params response value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_debug_cmd(CredoSlice_t* slice, int lane, unsigned section, unsigned index,
                                                 unsigned* response_params);

/**
 * @brief Subset of firmware commands for debug
 * @ingroup ExFirmwareCommands
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[in] section section to select
 * @param[in] index index to select
 * @param[out] response1 for response value
 * @param[out] response2 for response value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_debug_cmd_ex(CredoSlice_t* slice, int lane, unsigned section, unsigned index,
                                                    unsigned* response1, unsigned* response2);
// Firmware Registers

/**
 * @brief Read a firmware register
 * @ingroup ExFirmwareRegisters
 * @param[in] slice slice handle
 * @param[in] addr register address
 * @param[out] value register value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_reg_rd(CredoSlice_t* slice, unsigned addr, unsigned* value);

/**
 * @brief Write a firmware register
 * @ingroup ExFirmwareRegisters
 * @param[in] slice slice handle
 * @param[in] addr register addess
 * @param[in] value register value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_reg_wr(CredoSlice_t* slice, unsigned addr, unsigned value);

/**
 * @brief Read a firmware register extended
 * @ingroup ExFirmwareRegisters
 * @param[in] slice slice handle
 * @param[in] addr register address
 * @param[in] section register section
 * @param[out] value register value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_reg_rd_ex(CredoSlice_t* slice, unsigned addr, unsigned section, unsigned* value);

/**
 * @brief Write a firmware register extended
 * @ingroup ExFirmwareRegisters
 * @param[in] slice slice handle
 * @param[in] addr register address
 * @param[in] section register section
 * @param[in] value register value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_firmware_reg_wr_ex(CredoSlice_t* slice, unsigned addr, unsigned section, unsigned value);

/**
 * @brief Load firmware from SPI flash onto slice
 * @ingroup FirmwareSPI
 * @param[in] slice slice handle
 * @param[in] partition_num partition number in SPI flash
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_spiflash_load_firmware(CredoSlice_t* slice, int partition_num);

/**
 * @brief Display MBR information
 * @ingroup FirmwareSPI
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_spiflash_display_mbr(CredoSlice_t* slice);

/**
 * @brief Format MBR in SPI flash
 * @ingroup FirmwareSPI
 * @param[in] slice slice handle
 * @param[in] flash_kb_size kilobytes size for flash
 * @param[in] force force to format MBR
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_spiflash_format_mbr(CredoSlice_t* slice, unsigned flash_kb_size, int force);

/**
 * @brief Read firmware from SPI flash
 * @ingroup FirmwareSPI
 * @param[in] slice slice handle
 * @param[in] fwname firmware path
 * @param[in] partition_num partition number in SPI flash
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_spiflash_read_firmware(CredoSlice_t* slice, const char* fwname, int partition_num);

/**
 * @brief Write firmware into SPI flash
 * @ingroup FirmwareSPI
 * @param[in] slice slice handle
 * @param[in] fwname firmware path
 * @param[in] partition_num partition number in SPI flash
 * @param[in] force force to write firmware to flash
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_spiflash_write_firmware(CredoSlice_t* slice, const char* fwname, int partition_num,
                                                      int force);
#ifdef __cplusplus
}
#endif

#endif
