#ifndef CREDO_SDK_H
#define CREDO_SDK_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// SDK Management

/**
 * @ingroup SDKConfig
 *
 * Providing a configuration, the sdk handle keeps the register access functions and logging functions. There is no
 * internal limit to the number of sdk handles you may create. This allows for flexibility in how external drivers are
 * implemented.
 *
 * @param[in] config reg access and logging information
 * @param[out] sdk handle to create
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_sdk_create(const CredoSdkConfig_t* config, CredoSdk_t** sdk);

/**
 * @brief Destroy sdk handle
 * @ingroup SDKConfig
 * @param[in] sdk handle to destroy
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_sdk_destroy(CredoSdk_t* sdk);

// SDK Information
/**
 * @brief Indicates what version of the top level sdk is run.
 * @ingroup SDKInfo
 * Encoded [8 bits major].[8 bits minor].[8 bits patch]
 * @return encoded sdk version
 */
CREDOAPI uint32_t cr_sdk_version(void);

/**
 * @brief Get the sdk revision string
 * @ingroup SDKInfo
 * @return stringified sdk revision
 */
CREDOAPI char const* cr_sdk_version_str(void);

// Chip Library

/**
 *
 * Allows the loading of chip libraries at runtime. This is an alternative to linking at compile time.
 * @ingroup Lib
 * @param[in] chip_library path to chip library
 * @return Successfully loaded chip library
 */
CREDOAPI CredoErrorCodes_t cr_lib_load_chip(const char* chip_library);

/**
 * @brief Set the logger
 * @ingroup Logger
 * @param logger
 */
CREDOAPI void cr_logger_set(CredoLog_t logger);

/**
 * @brief Set the log level
 * @ingroup Logger
 * @param loglevel
 */
CREDOAPI void cr_logger_set_level(CredoLogLevel_t loglevel);

/**
 * @brief Get the log level
 * @ingroup Logger
 * @return loglevel
 */
CREDOAPI CredoLogLevel_t cr_logger_get_level(void);
// Driver

/**
 *
 * Enable burst read functionality to speed up certain function operations (e.g., firmware commands). Burst reading is
 * for reading from contiguous register addresses.
 * @ingroup SDKDriver
 * @param[in] sdk sdk handle
 * @param[in] burst_read_func burst read function
 */
CREDOAPI void cr_sdk_set_burst_read(CredoSdk_t* sdk, CredoReadRegisterBurst_t burst_read_func);

/**
 * Enable burst read functionality to speed up certain function operations (e.g., firmware commands). Burst writing is
 * for writing to contiguous register addresses.
 * @ingroup SDKDriver
 * @param[in] sdk sdk handle
 * @param[in] burst_write_func burst write function
 */
CREDOAPI void cr_sdk_set_burst_write(CredoSdk_t* sdk, CredoWriteRegisterBurst_t burst_write_func);

/**
 * @brief Write to multiple slices at one time.
 *
 * NOTE: requires additional configuration from the driver to support feature
 * @ingroup SDKDriver
 * @param[in] sdk sdk handle
 * @param[in] func broadcast write function
 */
CREDOAPI void cr_sdk_set_broadcast_write(CredoSdk_t* sdk, CredoWriteRegisterBroadcast_t func);

/**
 * @brief Write to multiple slices at one time for contiguous registers.
 *
 * NOTE: requires additional configuration from the driver to support feature
 * @ingroup SDKDriver
 * @param[in] sdk sdk handle
 * @param[in] func broadcast burst write function
 */
CREDOAPI void cr_sdk_set_broadcast_burst_write(CredoSdk_t* sdk, CredoWriteRegisterBroadcastBurst_t func);

// Logger

/**
 * @brief Set the max log level
 * @deprecated Use cr_logger_set_level v2.2.0
 * @ingroup SDKLogger
 * @param[in] sdk sdk handle
 * @param[in] max_level logging threshold to set
 */
CREDOAPI void cr_sdk_set_loglevel(CredoSdk_t* sdk, CredoLogLevel_t max_level);

/**
 * @brief Get the max log level object
 * @deprecated Use cr_logger_set_level v2.2.0
 * @ingroup SDKLogger
 * @param[in] sdk sdk handle
 * @return current logging threshold
 */
CREDOAPI CredoLogLevel_t cr_sdk_get_loglevel(CredoSdk_t* sdk);

#ifdef __cplusplus
}
#endif

#endif
