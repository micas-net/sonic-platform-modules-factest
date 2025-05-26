#ifndef CREDO_PORT_H
#define CREDO_PORT_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Display information about a port to logger.
 * @deprecated Alias of display_slice_info where port_id is parsed to a string
 * @ingroup Port
 * @param[in] slice slice handle
 * @param[in] command command string to use
 * @param[in] port_id which port to use
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_display_info(CredoSlice_t* slice, const char* command, uint32_t port_id);

// Port Operations

/**
 * @brief Configure a port through firmware
 * @ingroup Port
 * @param[in] slice slice handle
 * @param[in] port_config port configuration. If port_config.port_id == CR_PORT_AUTO_ASSIGN_ID, port_config.port_id will
 * be the identifier assigned by firmware on exit
 * @param[in] force force port configuration if lanes already in use
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_configure(CredoSlice_t* slice, CredoPortConfig_t* port_config, int force);

/**
 * @brief Destroy a port through firmware
 * @ingroup Port
 * @param[in] slice slice handle
 * @param[in] portId port to destroy
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_destroy(CredoSlice_t* slice, uint32_t portId);

/**
 * @brief Get information about a configured port
 * @ingroup Port
 * @param[in] slice slice handle
 * @param[in] portId port to get infromation
 * @param[out] port_config port configuration that is provided
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_query(CredoSlice_t* slice, uint32_t portId, CredoPortConfig_t* port_config);

/**
 * @brief Destroy all ports configured
 * @ingroup Port
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_destroy_all(CredoSlice_t* slice);

/**
 * @brief Get RS-FEC align status
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[out] rsfec_status
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_align_status(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                     CredoRSFECStatus_t* rsfec_status);

/**
 * @brief Get RS-FEC fifo status
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[out] rsfec_fifo
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_fifo(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                             CredoRSFECFifo_t* rsfec_fifo);

/**
 * @brief Get RS-FEC lane mapping
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[out] lane_mapping [7:6] mapped to PMA lane 3
 *                     [5:4] mapped to PMA lane 2
 *                     [3:2] mapped to PMA lane 1
 *                     [1:0] mapped to PMA lane 0
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_lane_mapping(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                     unsigned* lane_mapping);

/**
 * @brief Get RS-FEC histogram for given hist bin
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[in] hist_bin
 * @param[out] hist
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_histogram(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side, int hist_bin,
                                                  uint64_t* hist);

/**
 * @brief Get RS-FEC corrected codewords count
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[out] corr_cw
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_corrected_codeword(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                           uint64_t* corr_cw);

/**
 * @brief Get RS-FEC uncorrected codewords count
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[out] uncorr_cw
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_uncorrected_codeword(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                             unsigned* uncorr_cw);

/**
 * @brief Get RS-FEC symbol error for each lane
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[in] fec_lane per-lane index
 * @param[out] symbol_error
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_symobl_error(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                     int fec_lane, unsigned* symbol_error);

/**
 * @brief Get RS-FEC total bits
 * @deprecated v2.5.0 get codeword count and then use frame size to compute total bits
 * @ingroup ExPortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[out] total_bits
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_total_bits(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                   uint64_t* total_bits);

/**
 * @brief Get RS-FEC total codewords received.
 * @ingroup PortRsfec
 * @param slice
 * @param port_id
 * @param side
 * @param total_cw
 * @return CREDOAPI
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_total_codeword(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                       uint64_t* total_cw);

/**
 * @brief Get RS-FEC corrected bits
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[out] corrected_bits
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_corrected_bits(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                       uint64_t* corrected_bits);

/**
 * @brief Set RS-FEC count freeze state
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[in] enable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_set_count_freeze(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                     bool enable);

/**
 * @brief Get RS-FEC count freeze state
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @param[out] enable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_get_count_freeze(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side,
                                                     bool* enable);

/**
 * @brief Reset RS-FEC count
 * @ingroup PortRsfec
 * @param[in] slice slice handle
 * @param[in] port_id
 * @param[in] side
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_reset_count(CredoSlice_t* slice, uint32_t port_id, CredoSide_t side);

/* RS-FEC register (32/64-bit) read/write. Address=fec_base_addr(port, side)+offset*4.
 * For 64-bit registers, MSB are in register DATA_HI. */

/**
 * @brief
 * @ingroup FEC
 * @param[in] slice
 * @param[in] portId
 * @param[in] side
 * @param[in] offset
 * @param[out] val
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_read(CredoSlice_t* slice, uint32_t portId, CredoSide_t side, unsigned offset,
                                         unsigned* val);

/**
 * @brief
 * @ingroup FEC
 * @param[in] slice
 * @param[in] portId
 * @param[in] side
 * @param[in] offset
 * @param[in] val
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_rsfec_write(CredoSlice_t* slice, uint32_t portId, CredoSide_t side, unsigned offset,
                                          unsigned val);

/**
 * @addtogroup PortParam
 * @{
 */
#define CR_PPARAM_CIPHER_SUITE         "cipher_suite"
#define CR_PPARAM_PACKET_EXPANSION     "packet_expansion"
#define CR_PPARAM_EXTENDED_PAD_REMOVAL "extended_pad_removal"
#define CR_PPARAM_LINK                 "link"
#define CR_PPARAM_LINK_HOST            "link_host"
#define CR_PPARAM_LINK_LINE            "link_line"

typedef enum { CR_CIPHER_AES_GCM = 0, CR_CIPHER_SM4 = 1 } CredoCipher_t;
/** @} */

// utility for c11 that enables port
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define cr_port_set_param(slice, name, index, val) \
    _Generic(val, double                           \
             : cr_port_set_paramf, int             \
             : cr_port_set_parami, unsigned        \
             : cr_port_paramu)(slice, name, index, val)

#define cr_port_get_param(slice, name, index, val) \
    _Generic(val, double*                             \
             : cr_port_get_paramf, int*             \
             : cr_port_get_parami, unsigned*        \
             : cr_port_get_paramu)(slice, name, index, val)

#define cr_port_set_paramlist(slice, name, index, val, count) \
    _Generic(val, double*                                        \
             : cr_port_set_paramlistf, int*                    \
             : cr_port_set_paramlisti, unsigned*               \
             : cr_port_set_paramlistu)(slice, name, index, val, count)
#define cr_port_get_paramlist(slice, name, index, val, count) \
    _Generic(val, double*                                        \
             : cr_port_get_paramlistf, int*                    \
             : cr_port_get_paramlisti, unsigned*               \
             : cr_port_get_paramlistu)(slice, name, index, val, count)
#endif

/**
 * @brief Set port parameter with an integer value
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_set_parami(CredoSlice_t* slice, const char* name, int index, int value);
/**
 * @brief Get port parameter with an integer value
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_get_parami(CredoSlice_t* slice, const char* name, int index, int* value);

/**
 * @brief Set port parameter with an integer value
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_set_paramu(CredoSlice_t* slice, const char* name, int index, unsigned value);
/**
 * @brief Get port parameter with an unsigned value
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_get_paramu(CredoSlice_t* slice, const char* name, int index, unsigned* value);

/**
 * @brief Set port parameter with a double value
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_set_paramf(CredoSlice_t* slice, const char* name, int index, double value);

/**
 * @brief Get port parameter with a double value
 *
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_get_paramf(CredoSlice_t* slice, const char* name, int index, double* value);

/**
 * @brief Set port parameter with multiple integer values
 *
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_set_paramlisti(CredoSlice_t* slice, const char* name, int index, int values[],
                                                  int count);
/**
 * @brief Get port parameter with multiple integer values
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_get_paramlisti(CredoSlice_t* slice, const char* name, int index, int values[],
                                                  int count);

/**
 * @brief Set port parameter with multiple unsigned values
 *
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_set_paramlistu(CredoSlice_t* slice, const char* name, int index, unsigned values[],
                                                  int count);
/**
 * @brief Get port parameter with multiple unsigned values
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_get_paramlistu(CredoSlice_t* slice, const char* name, int index, unsigned values[],
                                                  int count);

/**
 * @brief Set port parameter with multiple double values
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_set_paramlistf(CredoSlice_t* slice, const char* name, int index, double values[],
                                                  int count);

/**
 * @brief Get parameter with multiple double values
 *
 *
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_get_paramlistf(CredoSlice_t* slice, const char* name, int index, double values[],
                                                  int count);

/**
 * @brief Get number of port Parameters the slice contains
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[out] count number of parameters
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_get_param_count(CredoSlice_t* slice, int* count);

/**
 * @brief Get the number of values a Port Parameter contains.
 *
 * For parameters that have the CR_PARAM_FLAG_VAR_COUNT flag enabled. This indicates how many of the values are needed
 * in parameters that may have variable size depending upon the mode.
 *
 * If the parameter does not have CR_PARAM_FLAG_VAR_COUNT set then it will return the count from the definition.
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] name name of the parameter
 * @param[in] index index of param val
 * @param[out] count number of values the parameter contains
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_get_param_val_count(CredoSlice_t* slice, const char* name, int index, int* count);

/**
 * @brief Get port parameter definition by index
 *
 *
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] param_index parameter in list from [0, param_count - 1]
 * @param[out] param parameter information if in bounds
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_index_param_def(CredoSlice_t* slice, int param_index, CredoParam_t* param);

/**
 * @brief Find port parameter definition by name
 * @ingroup PortParam
 * @param[in] slice slice to use
 * @param[in] name name of the parameter
 * @param[out] found indicates if the parameter was found
 * @param[out] param parameter information
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_port_find_param_def(CredoSlice_t* slice, const char* name, bool* found,
                                                  CredoParam_t* param);

#ifdef __cplusplus
}
#endif

#endif
