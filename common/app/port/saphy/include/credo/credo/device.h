#ifndef CREDO_DEVICE_H
#define CREDO_DEVICE_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a device
 *
 * Using a specified device_type create a device handle that can be used to access the slices for that device.
 *
 * @ingroup DeviceConfig
 * @param[in] sdk sdk handle
 * @param[in] device_type what kind of Credo Device to create
 * @param[out] device device handle to create
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_device_create(CredoSdk_t* sdk, CredoDeviceType_t device_type, CredoDevice_t** device);

/**
 * @brief Destroy a device handle.
 * @ingroup DeviceConfig
 * @param[in] device device handle to destroy
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_device_destroy(CredoDevice_t* device);

/**
 * @brief Detect device type of the slice.
 * @ingroup DeviceInfo
 * @param[in] sdk sdk handle
 * @param[in] slice_context register access information
 * @param[out] device_type device type
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_device_detect_type(CredoSdk_t* sdk, void* slice_context, CredoDeviceType_t* device_type);

/**
 * @brief Detect device type of the slice with push pull mode. This function will set to push pull mode when reading
 * device info.
 * @ingroup DeviceInfo
 * @param[in] sdk sdk handle
 * @param[in] slice_context register access information
 * @param[out] device_type device type
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_device_detect_type_push_pull(CredoSdk_t* sdk, void* slice_context,
                                                           CredoDeviceType_t* device_type);

/**
 * @brief Get device type of an allocated device.
 * @ingroup DeviceInfo
 * @param[in] device device handle to destroy
 * @param[out] device_type what kind of Credo Device the device uses
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_device_get_type(CredoDevice_t* device, CredoDeviceType_t* device_type);

/**
 * @brief Get the device type string name
 * @ingroup DeviceInfo
 * @param[in] device device handle
 * @return string name of device_type
 */
CREDOAPI const char* cr_device_get_type_name(CredoDevice_t* device);

// Slice Management
/**
 * @brief Get how many slices the device contains
 * @ingroup DeviceSlice
 * @param[in] device device handle
 * @param[out] slice_count slice count for the device
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_device_get_slice_count(CredoDevice_t* device, int* slice_count);

/**
 * @brief Get a slice handle from the device.
 * @ingroup DeviceSlice
 * @param[in] device device handle
 * @param[in] slice_index slice index to obtain
 * @param[out] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_device_get_slice(CredoDevice_t* device, int slice_index, CredoSlice_t** slice);

#ifdef __cplusplus
}
#endif
#endif
