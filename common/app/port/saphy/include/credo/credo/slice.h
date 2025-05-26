#ifndef CREDO_SLICE_H
#define CREDO_SLICE_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize a slice with a configuration. Provide the context to communicate with the slice and other information such
 * as firmware to load.
 * @ingroup SliceConfig
 * @param[in] slice slice handle
 * @param[in] config configuration for the slice
 * @return Error Code
 */

CREDOAPI CredoErrorCodes_t cr_slice_init(CredoSlice_t* slice, const CredoSliceConfig_t* config);

/**
 * @brief Convenience functio to re-initialize a slice
 *
 * Underneath it simply calls slice_init again but keeps track of the information you provided.
 *
 * @note only do this if the slice has already been initialized
 * @ingroup SliceConfig
 * @param slice slice handle
 * @param init initialization type
 * @param firmware firmware to use if the init_type requires it.
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_reinit(CredoSlice_t* slice, CredoSliceInitType_t init, const char* firmware);

// Slice Information
//

/**
 * @brief Get slice company OUI
 * @ingroup SliceInformation
 * @param[in] slice slice handle
 * @param[out] oui company OUI
 * @return Error code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_oui(CredoSlice_t* slice, unsigned* oui);

/**
 * @brief Get slice model number
 * @ingroup SliceInformation
 * @param[in] slice slice handle
 * @param[out] model_number model number
 * @return Error code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_model_number(CredoSlice_t* slice, unsigned* model_number);

/**
 * @brief Get slice revision number
 * @ingroup SliceInformation
 * @param[in] slice slice handle
 * @param[out] revision_number revision number
 * @return Error code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_revision_number(CredoSlice_t* slice, unsigned* revision_number);

/**
 * @brief Get custom data stored in the slice handle.
 * @deprecated since v1.2.0, store slice specific information in the slice context
 * @ingroup ExSliceInformation
 * @param[in] slice slice handle
 * @return user data
 */
CREDOAPI void* cr_slice_get_userdata(const CredoSlice_t* slice);

/**
 * @brief Store custom data in slice handle.
 * @deprecated since v1.2.0, store slice specific information in the slice context
 * @ingroup ExSliceInformation
 * @param[in] slice slice handle
 * @param[in] userdata data to store
 * @return Error code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_userdata(CredoSlice_t* slice, void* userdata);

/**
 * @brief Get slice type of the slice.
 * @ingroup SliceInformation
 * @param[in] slice slice handle
 * @param[out] slice_type slice type of the slice
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_type(CredoSlice_t* slice, CredoSliceType_t* slice_type);

/**
 * @brief Get device type of the slice.
 * @ingroup SliceInformation
 * @param[in] slice slice handle
 * @param[out] device_type device type of the slice
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_device_type(const CredoSlice_t* slice, CredoDeviceType_t* device_type);

/**
 * @brief Get slice limitation.
 * @ingroup SliceInformation
 * @param[in] slice slice handle
 * @param[out] limits slice limitation
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_limits(const CredoSlice_t* slice, CredoSliceLimit_t* limits);

/**
 * @brief Get slice vsensor.
 * @ingroup SliceVSensor
 * @param[in] slice slice handle
 * @param[out] vsensor slice voltage value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_vsensor(CredoSlice_t* slice, double* vsensor);

/**
 * @brief Get slice vsensor with input mux.
 * @ingroup SliceVSensor
 * @param[in] slice slice handle
 * @param[in] sel_vin vsensor mux select
 * @param[out] vsensor slice voltage value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_vsensor_ex(CredoSlice_t* slice, unsigned sel_vin, double* vsensor);

/**
 * @brief Get slice sram ecc status.
 * @ingroup Sram
 * @param[in] slice slice handle
 * @param[out] sram_status slice sram status
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_sram_get_status(CredoSlice_t* slice, CredoSramStatus_t* sram_status);

/**
 * @brief Inject slice sram ecc error.
 * @ingroup Sram
 * @param[in] slice slice handle
 * @param[in] sram_status slice sram error type
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_sram_generate_error(CredoSlice_t* slice, CredoSramStatus_t sram_status);

/**
 * @brief Read a register from the slice
 * @ingroup ExSliceRegAccess
 * @param[in] slice slice handle
 * @param[in] address register address to read
 * @param[out] value register value read
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_read(CredoSlice_t* slice, unsigned address, unsigned* value);

/**
 * @brief Write a register in the slice
 * @ingroup ExSliceRegAccess
 * @param[in] slice slice handle
 * @param[in] address register address to write
 * @param[in] value register value to write
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_write(CredoSlice_t* slice, unsigned address, unsigned value);

/**
 * @brief Read contiguous registers in the slice
 * @ingroup ExSliceRegAccess
 * @param[in] slice slice handle
 * @param[in] first_address first register address to read
 * @param[out] value Array to store all values from burst read
 * @param[in] count how many registers to read
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_burst_read(CredoSlice_t* slice, unsigned first_address, unsigned value[],
                                               unsigned count);

/**
 * @brief Write contiguous registers in the slice
 * @ingroup ExSliceRegAccess
 * @param[in] slice slice handle
 * @param[in] first_address first register address to write
 * @param[in] value Array of all the register values to write
 * @param[in] count how many register to write
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_burst_write(CredoSlice_t* slice, unsigned first_address, const unsigned value[],
                                                unsigned count);

/**
 * @brief Load a register setup file to a slice
 * @ingroup ExSliceSetup
 * @param[in] slice slice handle
 * @param[in] file_path path to register setup file
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_load_setup(CredoSlice_t* slice, const char* file_path);

/**
 * @brief Save register setup of a slice to a file
 * @ingroup ExSliceSetup
 * @param[in] slice slice handle
 * @param[in] file_path path to save file
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_save_setup(CredoSlice_t* slice, const char* file_path);

// Clock Output

/**
 * @brief Enable clock output
 *
 * Please refer to chip specs to find out available cloock output. Use a lane clock signal to generate a reference clock
 * output.
 * @ingroup SliceClockOutput
 * @param slice
 * @param clock_output index of clock output. chip dependent on what clock outputs are available and if they are
 * single-ended or differential
 * @param lane lane to use to generate clock output
 * @param divider divider to use from lane signal to reduce clock output frequency
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_clockout_enable(CredoSlice_t* slice, unsigned clock_output, unsigned lane,
                                              unsigned divider);

/**
 * @brief Disable all clock output generation
 * @ingroup SliceClockOutput
 * @param slice slice to disable clock output
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_clockout_disable_all(CredoSlice_t* slice);

/**
 * @brief Disable clock output generation for one clock
 * @ingroup SliceClockOutput
 * @param slice slice to disable clock output
 * @param clock_output clock output to disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_clockout_disable(CredoSlice_t* slice, unsigned clock_output);

/**
 * @brief Set the slice mdio mode between push/pull and pull-up.
 *
 * This should be set before @ref cr_slice_init is called.
 *
 * NOTE: Please confirm with Credo hardware team before using push/pull to ensure hardware configuration is correct.
 * Otherwise damage may occur to the board.
 *
 * @ingroup SliceConfig
 * @param slice slice to set mdio mode
 * @param is_push_pull set into push_pull mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_mdio_mode(CredoSlice_t* slice, bool is_push_pull);

/**
 * @brief Display information about a slice.
 *
 * Use command="help" or empty string to display documentation of commands, parameters, and descriptions available.
 *
 * @ingroup SliceInformation
 * @param[in] slice slice handle
 * @param[in] command command string to use
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_display_info(CredoSlice_t* slice, const char* command);

/**
 * @brief get the option list of a given slice
 * @note option_list points to constant memory and caller shall not free it.
 * @ingroup SliceOption
 * @param[in] slice slice handle
 * @param[in,out] option_count pointer to the number of the available options, NULL to print the available option in
 * logger interface.
 * @param[in,out] option_list pointer to the available options, NULL to print the available option in logger interface.
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_option_list(CredoSlice_t* slice, int* option_count,
                                                    const CredoSliceOption_t** option_list);

/**
 * @brief query if the given option name is supported
 * @ingroup SliceOption
 * @param[in] slice slice handle
 * @param[in] option_name option name of the query
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_is_option_supported(CredoSlice_t* slice, const char* option_name);

/**
 * @brief get the value of the given option name
 * @ingroup SliceOption
 * @param[in] slice slice handle
 * @param[in] option_name option name
 * @param[out] value pointer to the return value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_option(CredoSlice_t* slice, const char* option_name, int* value);

/**
 * @brief set the value of the given option name
 * @ingroup SliceOption
 * @param[in] slice slice handle
 * @param[in] option_name option name
 * @param[in] value the value to set
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_option(CredoSlice_t* slice, const char* option_name, int value);

/**
 * @brief Get slice temperature
 * @ingroup SliceInformation
 * @param[in] slice slice handle
 * @param[out] temp temperature in Celsius
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_temperature(CredoSlice_t* slice, double* temp);

/**
 * @brief Software reset, reset FIFO, PHY, register map, cpu
 * @ingroup ExSliceReset
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_soft_reset(CredoSlice_t* slice);

/**
 * @brief Logic reset, reset FIFO, PHY
 * @ingroup ExSliceReset
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_logic_reset(CredoSlice_t* slice);

/**
 * @brief CPU reset
 * @ingroup ExSliceReset
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_mcu_reset(CredoSlice_t* slice);

/**
 * @brief CPU reset, keep in reset mode
 * @ingroup ExSliceReset
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_mcu_reset_hold(CredoSlice_t* slice);

/**
 * @brief Register map reset
 * @ingroup ExSliceReset
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_reg_reset(CredoSlice_t* slice);

/**
 * @addtogroup SliceParam
 * @{
 */
#define CR_SLCPARAM_FW_CMD_TIMEOUT  "fw_cmd_timeout"
#define CR_SLCPARAM_FW_UP_TIME      "fw_up_time"
#define CR_SLCPARAM_TOP_CAL_TIMEOUT "top_cal_timeout"
#define CR_SLCPARAM_REFCLK          "refclk"
/** @} */

// utility for c11 that enables Slice
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define cr_slice_set_param(slice, name, index, val) \
    _Generic(val, double                            \
             : cr_slice_set_paramf, int             \
             : cr_slice_set_parami, unsigned        \
             : cr_slice_paramu)(slice, name, index, val)

#define cr_slice_get_param(slice, name, index, val) \
    _Generic(val, double*                             \
             : cr_slice_get_paramf, int*             \
             : cr_slice_get_parami, unsigned*        \
             : cr_slice_get_paramu)(slice, name, index, val)

#define cr_slice_set_paramlist(slice, name, index, val, count) \
    _Generic(val, double*                                        \
             : cr_slice_set_paramlistf, int*                    \
             : cr_slice_set_paramlisti, unsigned*               \
             : cr_slice_set_paramlistu)(slice, name, index, val, count)
#define cr_slice_get_paramlist(slice, name, index, val, count) \
    _Generic(val, double*                                        \
             : cr_slice_get_paramlistf, int*                    \
             : cr_slice_get_paramlisti, unsigned*               \
             : cr_slice_get_paramlistu)(slice, name, index, val, count)
#endif

/**
 * @brief Set Slice Parameter with an integer value
 *@ ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_parami(CredoSlice_t* slice, const char* name, int index, int value);
/**
 * @brief Get Slice Parameter with an integer value
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_parami(CredoSlice_t* slice, const char* name, int index, int* value);

/**
 * @brief Set Slice Parameter with an integer value
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_paramu(CredoSlice_t* slice, const char* name, int index, unsigned value);
/**
 * @brief Get Slice Parameter with an unsigned value
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_paramu(CredoSlice_t* slice, const char* name, int index, unsigned* value);

/**
 * @brief Set Slice Parameter with a double value
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_paramf(CredoSlice_t* slice, const char* name, int index, double value);

/**
 * @brief Get Slice Parameter with a double value
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_paramf(CredoSlice_t* slice, const char* name, int index, double* value);

/**
 * @brief Set Slice Parameter with multiple integer values
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_paramlisti(CredoSlice_t* slice, const char* name, int index, int values[],
                                                   int count);
/**
 * @brief Get Slice Parameter with multiple integer values
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_paramlisti(CredoSlice_t* slice, const char* name, int index, int values[],
                                                   int count);

/**
 * @brief Set Slice Parameter with multiple unsigned values
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_paramlistu(CredoSlice_t* slice, const char* name, int index, unsigned values[],
                                                   int count);
/**
 * @brief Get Slice Parameter with multiple unsigned values
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_paramlistu(CredoSlice_t* slice, const char* name, int index, unsigned values[],
                                                   int count);

/**
 * @brief Set Slice Parameter with multiple double values
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_set_paramlistf(CredoSlice_t* slice, const char* name, int index, double values[],
                                                   int count);

/**
 * @brief Get parameter with multiple double values
 *
 *
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_paramlistf(CredoSlice_t* slice, const char* name, int index, double values[],
                                                   int count);

/**
 * @brief Get number of Slice Parameters the slice contains
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[out] count number of parameters
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_param_count(CredoSlice_t* slice, int* count);

/**
 * @brief Get the number of values a Slice Parameter contains.
 *
 * For parameters that have the CR_PARAM_FLAG_VAR_COUNT flag enabled. This indicates how many of the values are needed
 * in parameters that may have variable size depending upon the mode.
 *
 * If the parameter does not have CR_PARAM_FLAG_VAR_COUNT set then it will return the count from the definition.
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] name name of the parameter
 * @param[in] index index of param val
 * @param[out] count number of values the parameter contains
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_get_param_val_count(CredoSlice_t* slice, const char* name, int index, int* count);

/**
 * @brief Get Slice parameter definition by index
 *
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] param_index parameter in list from [0, param_count - 1]
 * @param[out] param parameter information if in bounds
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_index_param_def(CredoSlice_t* slice, int param_index, CredoParam_t* param);

/**
 * @brief Find Slice parameter definition by name
 * @ingroup SliceParam
 * @param[in] slice slice to use
 * @param[in] name name of the parameter
 * @param[out] found indicates if the parameter was found
 * @param[out] param parameter information
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_slice_find_param_def(CredoSlice_t* slice, const char* name, bool* found,
                                                   CredoParam_t* param);

#ifdef __cplusplus
}
#endif

#endif
