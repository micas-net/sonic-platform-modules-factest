#ifndef CREDO_LANE_H
#define CREDO_LANE_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configure a lane into a mode
 * @ingroup LaneMode
 * @param[in] slice slice handle
 * @param[in] lane lane to configure
 * @param[in] lane_mode lane mode to set
 * @param[in] speed speed of lane in Mb/s
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_configure_mode(CredoSlice_t* slice, int lane, CredoLaneMode_t lane_mode,
                                                  uint32_t speed);

/**
 * @brief Configure a lane into phy loopback mode
 * @ingroup LaneMode
 * @param[in] slice slice handle
 * @param[in] lane lane to configure
 * @param[in] speed speed of lane in Mb/s
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_configure_mode_loopback(CredoSlice_t* slice, int lane, uint32_t speed);

/**
 * @brief Unconfigure a lane back to off
 * @ingroup LaneMode
 * @param[in] slice slice handle
 * @param[in] lane lane to configure
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_destroy_mode(CredoSlice_t* slice, int lane);

/**
 * @brief Display information about a lane.
 * @deprecated Alias of display_slice_info where lane is parsed to a string
 * @ingroup LaneInfo
 * @param[in] slice slice handle
 * @param[in] command command string to use
 * @param[in] lane which lane to use
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_display_info(CredoSlice_t* slice, const char* command, int lane);

/**
 * @brief Get lane speed from firmware
 * @ingroup LaneInfo
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] speed_kbps speed_kbps value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_get_speed(CredoSlice_t* slice, int lane, uint32_t* speed_kbps);

/**
 * @brief get the option list of a given slice
 * @note option_list points to constant memory and caller shall not free it.
 * @ingroup LaneOption
 * @param[in] slice slice handle
 * @param[in,out] option_count pointer to the number of the available options for lane of a given slce, NULL to print
 * the available option in logger interface.
 * @param[in,out] option_list pointer to the available options for lane of a given slce, NULL to print the available
 * option in logger interface.
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_get_option_list(CredoSlice_t* slice, int* option_count,
                                                   const CredoLaneOption_t** option_list);

/**
 * @brief query if the given lane option name is supported
 * @ingroup LaneOption
 * @param[in] slice slice handle
 * @param[in] option_name option name of the query
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_is_option_supported(CredoSlice_t* slice, const char* option_name);

/**
 * @brief get the value of the given lane option name
 * @ingroup LaneOption
 * @param[in] slice slice handle
 * @param[in] lane lane index
 * @param[in] option_name option name
 * @param[out] value pointer to the return value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_get_option(CredoSlice_t* slice, int lane, const char* option_name, int* value);

/**
 * @brief set the value of the given lane option name
 * @ingroup LaneOption
 * @param[in] slice slice handle
 * @param[in] lane lane index
 * @param[in] option_name option name
 * @param[in] value the value to set
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_set_option(CredoSlice_t* slice, int lane, const char* option_name, int value);

// Top Level
/**
 * @brief Set target lane config
 * @ingroup ExLaneConfig
 * @deprecated This implies these are the only parameters that need to be set when there are more. Just use the
 * individual functions instead
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] lane_config
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_set_config(CredoSlice_t* slice, int lane, CredoLaneConfig_t* lane_config);

/**
 * @brief Get target lane config
 * @ingroup ExLaneConfig
 * @deprecated This implies these are the only parameters that need to be set when there are more. Just use the
 * individual functions instead
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] lane_config
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_get_config(CredoSlice_t* slice, int lane, CredoLaneConfig_t* lane_config);

// Loopback

/**
 * @brief Set target lane loopback mode
 * @ingroup LaneMode
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_set_loopback_mode(CredoSlice_t* slice, int lane, CredoLaneLoopbackMode_t mode);

/**
 * @brief Get target lane loopback mode
 * @ingroup LaneMode
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_get_loopback_mode(CredoSlice_t* slice, int lane, CredoLaneLoopbackMode_t* mode);

// Lane Mode

/**
 * @brief Get target lane current mode
 * @ingroup LaneMode
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_get_mode(CredoSlice_t* slice, int lane, CredoLaneMode_t* mode);

/**
 * @brief Set target lane mode
 * @ingroup ExLaneMode
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_set_mode(CredoSlice_t* slice, int lane, CredoLaneMode_t mode);

/**
 * @brief Sync SDK lane mode with firmware
 * @ingroup ExLaneMode
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_update_mode(CredoSlice_t* slice, int lane);

/**
 * @brief Disable target lane, this is an irreversible operation until chip reset
 * @ingroup ExLaneConfig
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_disable(CredoSlice_t* slice, int lane);

/**
 * @brief Get slice max host/line number
 * @ingroup LaneInfo
 * @param[in] slice slice handle
 * @param[out] host_lane
 * @param[out] line_lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_get_count(CredoSlice_t* slice, int* host_lane, int* line_lane);

/**
 * @brief Set target lane tx state to quiet mode
 * @ingroup LaneTrafficControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_tx_disable(CredoSlice_t* slice, int lane);

/**
 * @brief Set target lane tx state to normal mode
 * @ingroup LaneTrafficControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_tx_no_disable(CredoSlice_t* slice, int lane);

/**
 * @brief Get target lane tx state
 * @ingroup LaneTrafficControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] status
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_tx_get_status(CredoSlice_t* slice, int lane, CredoLaneTxState_t* status);

// RX Control

/**
 * @brief Set target lane rx state to quiet mode
 * @ingroup LaneTrafficControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_rx_disable(CredoSlice_t* slice, int lane);

/**
 * @brief Set target lane rx state to normal mode
 * @ingroup LaneTrafficControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_rx_no_disable(CredoSlice_t* slice, int lane);

// Resets

/**
 * @brief Reset target lane
 * @ingroup ExLaneReset
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_rx_reset(CredoSlice_t* slice, int lane);

/**
 * @brief Per lane Logic reset
 * @ingroup ExLaneReset
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_logic_reset(CredoSlice_t* slice, int lane);

/**
 * @brief per lane register map reset
 * @ingroup ExLaneReset
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_lane_reg_reset(CredoSlice_t* slice, int lane);

// PRBS

/**
 * @brief Get target lane tx prbs enable status and prbs pattern
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] enable 1 = enable, 0 = disable
 * @param[out] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_get_tx_generator(CredoSlice_t* slice, int lane, int* enable,
                                                    CredoLanePrbsPattern_t* mode);

/**
 * @brief Get target lane rx prbs enable status and prbs pattern
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] enable 1 = enable, 0 = disable
 * @param[out] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_get_rx_checker(CredoSlice_t* slice, int lane, int* enable,
                                                  CredoLanePrbsPattern_t* mode);

/**
 * @brief Set target lane tx prbs enable status and prbs pattern
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable 1 = enable, 0 = disable
 * @param[in] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_set_tx_generator(CredoSlice_t* slice, int lane, int enable,
                                                    CredoLanePrbsPattern_t mode);

/**
 * @brief Set target lane rx prbs enable status and prbs pattern
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable 1 = enable, 0 = disable
 * @param[in] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_set_rx_checker(CredoSlice_t* slice, int lane, int enable,
                                                  CredoLanePrbsPattern_t mode);

/**
 * @brief Set target lane rx prbs to nrz mode
 * @ingroup ExLanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable 1 = enable, 0 = disable
 * @param[in] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_set_rx_nrz(CredoSlice_t* slice, int lane, int enable, CredoLanePrbsPattern_t mode);

/**
 * @brief Set target lane rx prbs to pam4 mode
 * @ingroup ExLanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable 1 = enable, 0 = disable
 * @param[in] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_set_rx_pam4(CredoSlice_t* slice, int lane, int enable, CredoLanePrbsPattern_t mode);

/**
 * @brief Set target lane tx prbs to nrz mode
 * @ingroup ExLanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable 1 = enable, 0 = disable
 * @param[in] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_set_tx_nrz(CredoSlice_t* slice, int lane, int enable, CredoLanePrbsPattern_t mode);

/**
 * @brief Set target lane tx prbs to pam4 mode
 * @ingroup ExLanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable 1 = enable, 0 = disable
 * @param[in] mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_set_tx_pam4(CredoSlice_t* slice, int lane, int enable, CredoLanePrbsPattern_t mode);

/**
 * @brief Gets if the prbs checker is locked to a pattern
 * @ingroup LanePRBS
 * @param slice slice handle
 * @param lane lane to check
 * @param is_locked is the rx checker locked
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_get_rx_lock(CredoSlice_t* slice, int lane, CredoPrbsLockStatus_t* is_locked);

/**
 * @brief Get target lane rx prbs error counter
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] count
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_get_rx_count(CredoSlice_t* slice, int lane, uint32_t* count);

/**
 * @brief Get target lane rx ber
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] time_ms 0 to collect ber from calling cr_prbs_reset_rx_count(), otherwise time in ms to collect ber
 * without reset
 * @param[out] ber
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_get_rx_ber(CredoSlice_t* slice, int lane, int time_ms, double* ber);

/**
 * @brief Get prbs timer duration
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane lane to get timer
 * @param[out] duration_ms milliseconds prbs timer is running
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_get_rx_duration(CredoSlice_t* slice, int lane, unsigned long* duration_ms);

/**
 * @brief Reset target lane prbs error counter
 *
 * Also clears slice prbs timer.
 *
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_reset_rx_count(CredoSlice_t* slice, int lane);

/**
 * @brief Generate 1 bit error to target lane
 * @ingroup LanePRBS
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_prbs_generate_tx_error(CredoSlice_t* slice, int lane);

// TX Control
/**
 * @brief Set target lane test pattern enable state
 * @ingroup ExLaneTestPattern
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable Tx test pattern enable state
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_testpatt_set_tx_enable(CredoSlice_t* slice, int lane, bool enable);

/**
 * @brief Get target lane test pattern enable state
 * @ingroup ExLaneTestPattern
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] enable Tx test pattern enable state
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_testpatt_get_tx_enable(CredoSlice_t* slice, int lane, bool* enable);

/**
 * @brief Set target lane test pattern memory
 * @ingroup ExLaneTestPattern
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] pattern Tx test pattern memory
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_testpatt_set_tx_memory(CredoSlice_t* slice, int lane, uint64_t pattern);

/**
 * @brief Get target lane test pattern memory
 * @ingroup ExLaneTestPattern
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] pattern Tx test pattern memory
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_testpatt_get_tx_memory(CredoSlice_t* slice, int lane, uint64_t* pattern);

/**
 * @brief Set target lane test pattern mode, only valid when test pattern enabled
 * @ingroup ExLaneTestPattern
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] mode Tx test pattern mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_testpatt_set_tx_mode(CredoSlice_t* slice, int lane, CredoLaneTxTestPatternMode mode);

/**
 * @brief Get target lane test pattern mode, only valid when test pattern enabled
 * @ingroup ExLaneTestPattern
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] mode Tx test pattern mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_testpatt_get_tx_mode(CredoSlice_t* slice, int lane, CredoLaneTxTestPatternMode* mode);

// FEC Analyzer

/**
 * @brief Set target lane FEC analyzer
 * @ingroup LanePRBSFecAnalyzer
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable
 * @param[in] config
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_fecana_configure(CredoSlice_t* slice, int lane, int enable,
                                               CredoFecAnalyzerConfig_t* config);

/**
 * @brief Get target lane FEC analyzer
 * @ingroup LanePRBSFecAnalyzer
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] enable
 * @param[out] config
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_fecana_query(CredoSlice_t* slice, int lane, int* enable,
                                           CredoFecAnalyzerConfig_t* config);

/**
 * @brief Get target lane FEC analyzer counter by counter selection
 * @ingroup LanePRBSFecAnalyzer
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] counter_sel
 * @param[out] counter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_fecana_get_raw_counter(CredoSlice_t* slice, int lane, int counter_sel, unsigned* counter);

/**
 * @brief Get target lane FEC analyzer counter
 * @ingroup LanePRBSFecAnalyzer
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] pre_fec
 * @param[out] post_fec
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_fecana_get_counter(CredoSlice_t* slice, int lane, unsigned* pre_fec, unsigned* post_fec);

/**
 * @brief Set target lane FEC analyzer histogram group
 * @ingroup LanePRBSFecAnalyzer
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] group
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_fecana_set_hist_group(CredoSlice_t* slice, int lane, int group);

/**
 * @brief Get target lane FEC analyzer histogram data
 * @ingroup LanePRBSFecAnalyzer
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] hist_data
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_fecana_get_hist_counter(CredoSlice_t* slice, int lane, unsigned hist_data[4]);

/**
 * @brief Get the fec analyzer duration for lane counters
 * @ingroup LanePRBSFecAnalyzer
 * @param[in] slice slice handle
 * @param[in] lane lane to get duration
 * @param[out] duration_ms duration in milliseconds
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_fecana_get_duration(CredoSlice_t* slice, int lane, unsigned long* duration_ms);

/**
 * @brief Get the fec analyzer error rate for a lane
 *
 * @note If you provide duration_ms=0, the fec analyzer will use the lane global duration
 * @ingroup LanePRBSFecAnalyzer
 * @param[in] slice slice
 * @param[in] lane lane to use
 * @param[in] counter_sel what counter to get error rate
 * @param[in] duration_ms duration in milliseconds to record
 * @param[out] error_rate error rate for the counter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_fecana_get_error_rate(CredoSlice_t* slice, int lane, int counter_sel, int duration_ms,
                                                    double* error_rate);

/**
 * @brief set Auto Neg page
 * @ingroup LaneAutoNeg
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] pageId page ID to be set
 * @param[in] page page value to be set
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_autoneg_set_pages(CredoSlice_t* slice, int lane, int pageId, uint64_t page);

/**
 * @brief Get Auto Neg transmitted/received page
 * @ingroup LaneAutoNeg
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] page_count the number of transmitted/received pages
 * @param[out] transmitted_pages the values of transmitted pages
 * @param[out] received_pages the value of received pages
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_autoneg_get_exchanged_pages(CredoSlice_t* slice, int lane, int* page_count,
                                                          uint64_t transmitted_pages[9], uint64_t received_pages[9]);

#ifdef __cplusplus
}
#endif

#endif
