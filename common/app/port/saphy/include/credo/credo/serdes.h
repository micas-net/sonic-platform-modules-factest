#ifndef CREDO_SERDES_H
#define CREDO_SERDES_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Top PLL
/**
 * @brief Calibration slice top pll
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 *
 * @param[in] slice slice handle
 * @param[in] lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_cal_top_pll(CredoSlice_t* slice, int lane);

/**
 * @brief Init slice all pll setting
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_init_top_pll(CredoSlice_t* slice);

/**
 * @brief Get slice top pll vco cap
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[out] cap
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_top_pll_cap(CredoSlice_t* slice, unsigned* cap);

// Capability

/**
 * @brief Get target lane rx ffe capability
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] taps_len taps length
 * @param[out] sum_len summer length
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ffe_range(CredoSlice_t* slice, int lane, int* taps_len, int* sum_len);

/**
 * @brief Get target lane rx ffe weighting table capability
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] row table row
 * @param[out] col table column
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ffe_weighting_table_range(CredoSlice_t* slice, int lane, int* row,
                                                                      int* col);

/**
 * @brief Get target lane tx ffe capability
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] length
 * @param[out] extended_length
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_ffe_range(CredoSlice_t* slice, int lane, int* length, int* extended_length);

/**
 * @brief Get target lane used dfe count
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] length
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_dfe_range(CredoSlice_t* slice, int lane, int* length);

/**
 * @brief Get target lane used isi count
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] length
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_isi_range(CredoSlice_t* slice, int lane, int* length);

// Polarity

/**
 * @brief Set target lane rx polarity
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] rx_pol
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_polarity(CredoSlice_t* slice, int lane, int rx_pol);

/**
 * @brief Set target lane tx polarity
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] tx_pol
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_tx_polarity(CredoSlice_t* slice, int lane, int tx_pol);

/**
 * @brief Get target lane rx polarity
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] rx_pol
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_polarity(CredoSlice_t* slice, int lane, int* rx_pol);

/**
 * @brief Get target lane tx polarity
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] tx_pol
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_polarity(CredoSlice_t* slice, int lane, int* tx_pol);

// Input Mode

/**
 * @brief Set target lane input couple mode
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] input_mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_coupling(CredoSlice_t* slice, int lane, CredoLaneCoupling_t input_mode);

/**
 * @brief Get target lane input couple mode
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] input_mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_coupling(CredoSlice_t* slice, int lane, CredoLaneCoupling_t* input_mode);

// Gray & Pre Coding

/**
 * @brief Set target lane tx gray code
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] tx_gc 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_tx_gray_code(CredoSlice_t* slice, int lane, int tx_gc);

/**
 * @brief Set target lane rx gray code
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] rx_gc 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_gray_code(CredoSlice_t* slice, int lane, int rx_gc);

/**
 * @brief Get target lane tx gray code enable status
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] tx_gc 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_gray_code(CredoSlice_t* slice, int lane, int* tx_gc);

/**
 * @brief Get target lane rx gray code enable status
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] rx_gc 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_gray_code(CredoSlice_t* slice, int lane, int* rx_gc);

/**
 * @brief Set target lane tx precoder
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] tx_pc 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_tx_precoder(CredoSlice_t* slice, int lane, int tx_pc);

/**
 * @brief Set target lane rx precoder
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] rx_pc 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_precoder(CredoSlice_t* slice, int lane, int rx_pc);

/**
 * @brief Get target lane tx precoder, return programmed value instead of register value if firmware support tx precoder
 * setting
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] tx_pc 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_precoder(CredoSlice_t* slice, int lane, int* tx_pc);

/**
 * @brief Get target lane rx precoder
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] rx_pc 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_precoder(CredoSlice_t* slice, int lane, int* rx_pc);

// Bit swapping

/**
 * @brief Set target lane tx msb and lsb swap
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] tx_msb 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_tx_msb(CredoSlice_t* slice, int lane, int tx_msb);

/**
 * @brief Set target lane rx msb and lsb swap
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] rx_msb 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_msb(CredoSlice_t* slice, int lane, int rx_msb);

/**
 * @brief Get target lane tx msb and lsb swap status
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] tx_msb 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_msb(CredoSlice_t* slice, int lane, int* tx_msb);

/**
 * @brief Get target lane rx msb and lsb swap status
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] rx_msb 1 = enable, 0 = disable
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_msb(CredoSlice_t* slice, int lane, int* rx_msb);

// Lane PLL

/**
 * @brief Set target lane tx vco cap value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] tx_cap
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_tx_cap(CredoSlice_t* slice, int lane, int tx_cap);

/**
 * @brief Set target lane rx vco cap value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] rx_cap
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_cap(CredoSlice_t* slice, int lane, int rx_cap);

/**
 * @brief Get target lane tx vco cap value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] tx_cap
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_cap(CredoSlice_t* slice, int lane, int* tx_cap);

/**
 * @brief Get target lane rx vco cap value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] rx_cap
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_cap(CredoSlice_t* slice, int lane, int* rx_cap);

// RX Detail

/**
 * @brief Get target lane frequency accumulator value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] ppm
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ppm(CredoSlice_t* slice, int lane, int* ppm);

/**
 * @brief Get target lane rx skin effect enable status and degen value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] enable
 * @param[out] degen
 * @param[out] addcap
 * @param[out] gain
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_skef(CredoSlice_t* slice, int lane, int* enable, int* degen, int* addcap,
                                                 int* gain);

/**
 * @brief Get target lane rx dac value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] rx_dac dac value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_dac(CredoSlice_t* slice, int lane, int* rx_dac);

/**
 * @brief Get target lane rx attenuator value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] passive
 * @param[out] gain
 * @param[out] termtune
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_attenuator(CredoSlice_t* slice, int lane, int* passive, int* gain,
                                                       int* termtune);

/**
 * @brief Get target lane ffe taps value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] taps rx taps value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_ffe_taps(CredoSlice_t* slice, int lane, int taps[]);

/**
 * @brief Get target lane ffe fine taps value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] taps rx fine taps value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_ffe_taps_fine(CredoSlice_t* slice, int lane, int taps[]);

/**
 * @brief Get target lane f1over3 value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_f1over3(CredoSlice_t* slice, int lane, int* value);

/**
 * @brief Get the agcgain count
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane id
 * @param[out] count how many rx agcgain taps
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_agcgain_count(CredoSlice_t* slice, int lane, unsigned* count);

/**
 * @brief Get target lane agcgain value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] agcgain gain1 store in agcgain[0], gain2 store in agcgain[1]
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_agcgain(CredoSlice_t* slice, int lane, unsigned agcgain[]);

/**
 * @brief Get the ctle count
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane id
 * @param[out] count how many rx ctle values
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_ctle_count(CredoSlice_t* slice, int lane, unsigned* count);

/**
 * @brief Get target lane ctle value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] value ctle value list
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_ctle(CredoSlice_t* slice, int lane, unsigned value[]);

/**
 * @brief Get target lane delta overwrite value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] value delta overwrite value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_delta_phase(CredoSlice_t* slice, int lane, int* value);

/**
 * @brief Get target lane edge value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_edge(CredoSlice_t* slice, int lane, unsigned* value);

/**
 * @brief Get target lane dfe value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] dfe_taps
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_dfe(CredoSlice_t* slice, int lane, double dfe_taps[]);  // may only use 1

/**
 * @brief Get target lane eye value. If target lane is NRZ mode, only eyes[0] is available.
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] eyes
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_raw_eye(CredoSlice_t* slice, int lane, int eyes[3]);

/**
 * @brief
 * @ingroup SerdesStatus
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] status
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ready(CredoSlice_t* slice, int lane, int* status);

/**
 * @brief Get signal detect status
 * @ingroup SerdesStatus
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] sd signal detect value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_signal_detect(CredoSlice_t* slice, int lane, int* sd);

// RX debugging -- not to be used lightly

/**
 * @brief Set target lane rx skin effect enable status and degen value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] enable
 * @param[in] degen
 * @param[in] addcap
 * @param[in] gain
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_skef(CredoSlice_t* slice, int lane, int enable, int degen, int addcap,
                                                 int gain);

/**
 * @brief Set target lane rx dac value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] rx_dac dac value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_dac(CredoSlice_t* slice, int lane, int rx_dac);

/**
 * @brief Set target lane rx attenuator value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] passive
 * @param[in] gain
 * @param[in] termtune
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_rx_attenuator(CredoSlice_t* slice, int lane, int passive, int gain,
                                                       int termtune);

/**
 * @brief Set target lane rx ffe taps value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] taps rx taps value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_ffe_taps(CredoSlice_t* slice, int lane, const int taps[]);

/**
 * @brief Set target lane rx ffe fine taps value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] taps rx fine taps value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_ffe_taps_fine(CredoSlice_t* slice, int lane, const int taps[]);

/**
 * @brief Set target lane f1over3 value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_f1over3(CredoSlice_t* slice, int lane, int value);

/**
 * @brief Set target lane agcgain value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] value gain value list
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_agcgain(CredoSlice_t* slice, int lane, unsigned value[]);

/**
 * @brief Set target lane ctle value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] ctle ctle value list
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_ctle(CredoSlice_t* slice, int lane, unsigned ctle[]);

/**
 * @brief Set target lane delta overwrite value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] value delta overwrite value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_delta_phase(CredoSlice_t* slice, int lane, int value);

/**
 * @brief Set target lane edge value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_edge(CredoSlice_t* slice, int lane, unsigned value);

// TX Taps

/**
 * @brief Set target lane half amptitude value
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane lane to use
 * @param[in] taps_scale taps scale of lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_tx_taps_scale(CredoSlice_t* slice, int lane, const unsigned taps_scale[]);

/**
 * @brief Get target lane half amptitude value
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane lane to use
 * @param[out] taps_scale taps scale of lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_taps_scale(CredoSlice_t* slice, int lane, unsigned taps_scale[]);

/**
 * @brief Set target lane taps value
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] taps tx taps value (f-N,...,f-2,f-1,f0,f1,f2,fN)
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_tx_taps(CredoSlice_t* slice, int lane, const int taps[]);

/**
 * @brief Set target lane extended taps value
 * @ingroup ExSerdes
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[in] taps_extended
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_tx_taps_extended(CredoSlice_t* slice, int lane, const int taps_extended[]);

/**
 * @brief Get target lane taps value
 * @ingroup SerdesControl
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] taps tx taps value (f-N,...,f-2,f-1,f0,f1,f2,fN)
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_taps(CredoSlice_t* slice, int lane, int taps[]);

/**
 * @brief Get target lane extended taps value
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane
 * @param[out] taps_extended
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_tx_taps_extended(CredoSlice_t* slice, int lane, int taps_extended[]);

/**
 * @brief status of the phy (Lane) layer
 * @ingroup SerdesStatus
 * @param[in] slice slice handle
 * @param[out] rdy ready status of the slice
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_all_phy_ready(CredoSlice_t* slice, unsigned* rdy);

/**
 * @brief status of the phy (Lane) layer for a specific lane
 * @ingroup SerdesStatus
 * @param[in] slice slice handle
 * @param[in] lane lane to use
 * @param[out] rdy ready status of the slice lane
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_phy_ready(CredoSlice_t* slice, int lane, unsigned* rdy);

/**
 * @brief Get adapt count from firmware
 * @ingroup SerdesStatus
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] count Every time receiver starts to adapt, this counter will increase by 1.
 * Wrap around on overflow, does not clear on read.
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_adapt_count(CredoSlice_t* slice, int lane, unsigned* count);

/**
 * @brief Get readapt count from firmware
 * @ingroup SerdesStatus
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] count Every time receiver starts to adapt, this counter will increase by 1.
 * Saturate on overflow, clear on read.
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_readapt_count(CredoSlice_t* slice, int lane, unsigned* count);

/**
 * @brief Get link lost count from firmware
 * @ingroup SerdesStatus
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] count Every time receiver can not hold the link, and the link is already established, this counter will
 * increase. Saturate on overflow, clear on read.
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_link_lost_count(CredoSlice_t* slice, int lane, unsigned* count);

/**
 * @brief Get link loss of signal count from firmware
 * @ingroup SerdesStatus
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] count Every time receiver detects signal loss, this counter will increase by 1.
 * Saturate on overflow, clear on read. Note: If the receiver does not see signal, the counter will be at least 1.
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_los_count(CredoSlice_t* slice, int lane, unsigned* count);

/**
 * @brief Get channel estimate from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] chan_est channel estimate
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_channel_estimate(CredoSlice_t* slice, int lane, double* chan_est);

/**
 * @brief Get overflow frequency from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] of overflow frequency
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_of(CredoSlice_t* slice, int lane, unsigned* of);

/**
 * @brief Get high frequency from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] hf high frequency
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_hf(CredoSlice_t* slice, int lane, unsigned* hf);

/**
 * @brief Get eyes from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] eyes eye value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_eye(CredoSlice_t* slice, int lane, int eyes[3]);

/**
 * @brief Get isi from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] isi isi value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_isi(CredoSlice_t* slice, int lane, int isi[]);

/**
 * @brief Get ffe taps from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] taps ffe taps value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ffe(CredoSlice_t* slice, int lane, int taps[]);

/**
 * @brief Get ffe nbias from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] nbias ffe nbias
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ffe_nbias(CredoSlice_t* slice, int lane, int nbias[]);

/**
 * @brief Get ffe kaccu from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] kaccu ffe kaccu
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ffe_kaccu(CredoSlice_t* slice, int lane, double kaccu[]);

/**
 * @brief Get ffe weighting table from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] wt_table ffe weighting table
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ffe_weighting_table(CredoSlice_t* slice, int lane, double** wt_table);

/**
 * @brief Get ffe polarity flip counter from firmware
 * @ingroup ExSerdes
 * @deprecated replaced by SerdesParam group for more flexibility in v2.1.0
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] flip_counter ffe polarity flip counter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_rx_ffe_flip_counter(CredoSlice_t* slice, int lane, int flip_counter[]);

// EYE Monitor

/**
 * @brief Start eye monitor
 * @ingroup SerdesEyeMonitor
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[in] ber_exp
 * @param[in] flag
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_eye_monitor_start(CredoSlice_t* slice, int lane, int ber_exp, int flag);

/**
 * @brief Stop eye monitor
 * @ingroup SerdesEyeMonitor
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_eye_monitor_stop(CredoSlice_t* slice, int lane);

/**
 * @brief Get eye monitor progress
 * @ingroup SerdesEyeMonitor
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] percent percent 100 means data ready
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_eye_monitor_get_progress(CredoSlice_t* slice, int lane, int* percent);

/**
 * @brief Get eye monitor data
 * @ingroup SerdesEyeMonitor
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] data eye monitor data
 * @param[out] extent_mv vertical scale
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_eye_monitor_get_data(CredoSlice_t* slice, int lane, int** data, int* extent_mv);

/**
 * @brief Get eye monitor vertical and horizontal range
 * @ingroup SerdesEyeMonitor
 * @param[in] slice slice handle
 * @param[in] lane lane to select
 * @param[out] vstep_side vstep_side is defined in firmware
 * @param[out] hstep_side hstep_side is defined in firmware, return 0 if bathtub mode
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_eye_monitor_get_range(CredoSlice_t* slice, int lane, int* vstep_side, int* hstep_side);

/**
 * @brief Get eye monitor separator
 * @ingroup SerdesEyeMonitor
 * @param[in] slice slice handle
 * @param[out] separator vstep_separator is defined in firmware
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_eye_monitor_get_separator(CredoSlice_t* slice, int separator[5]);

/**
 * @addtogroup SerdesParam
 * @{
 */
#define CR_SPARAM_UP_TIME                "up_time"
#define CR_SPARAM_DOWN_TIME              "down_time"
#define CR_SPARAM_RX_ADAPT               "rx_adapt"
#define CR_SPARAM_RX_AGCGAIN             "rx_agcgain"
#define CR_SPARAM_RX_AGCATTEN            "rx_agcatten"
#define CR_SPARAM_RX_ATTEN_GAIN          "rx_atten_gain"
#define CR_SPARAM_RX_ATTEN_PASSIVE       "rx_atten_passive"
#define CR_SPARAM_RX_ATTEN_TERMTUNE      "rx_atten_termtune"
#define CR_SPARAM_RX_CHANNEL_EST         "rx_channel_est"
#define CR_SPARAM_RX_CHANNEL_HF          "rx_channel_hf"
#define CR_SPARAM_RX_CHANNEL_OF          "rx_channel_of"
#define CR_SPARAM_RX_ENVELOPE            "rx_envelope"
#define CR_SPARAM_RX_CTLE                "rx_ctle"
#define CR_SPARAM_RX_TIA1_BIAS           "rx_tia1_bias"
#define CR_SPARAM_RX_DAC                 "rx_dac"
#define CR_SPARAM_RX_DELTA               "rx_delta"
#define CR_SPARAM_RX_THS                 "rx_ths"
#define CR_SPARAM_RX_DFE_NL_MODE         "rx_dfe_nl_mode"
#define CR_SPARAM_RX_DFE                 "rx_dfe"
#define CR_SPARAM_RX_DFE_F1              "rx_dfe_f1"
#define CR_SPARAM_RX_DFE_ALL             "rx_dfe_all"
#define CR_SPARAM_RX_EDGE                "rx_edge"
#define CR_SPARAM_RX_EYE_HEIGHT          "rx_eye_height"
#define CR_SPARAM_RX_EYE_ALL             "rx_eye_all"
#define CR_SPARAM_RX_F1OVER3             "rx_f1over3"
#define CR_SPARAM_RX_FFE_FLIP_COUNTER    "rx_ffe_flip_counter"
#define CR_SPARAM_RX_FFE_KACCU           "rx_ffe_kaccu"
#define CR_SPARAM_RX_FFE_NBIAS           "rx_ffe_nbias"
#define CR_SPARAM_RX_FFE_TAPS_ALL        "rx_ffe_taps_all"
#define CR_SPARAM_RX_FFE_TAPS            "rx_ffe_taps"
#define CR_SPARAM_RX_FFE_TAPS_FINE       "rx_ffe_taps_fine"
#define CR_SPARAM_RX_FFE_WEIGHTING_TABLE "rx_ffe_weighting_table"
#define CR_SPARAM_RX_GRAYCODE            "rx_graycode"
#define CR_SPARAM_RX_INPUT_COUPLING      "rx_input_coupling"
#define CR_SPARAM_RX_LINKLOST            "rx_linklost"
#define CR_SPARAM_RX_MSBLSB              "rx_msblsb"
#define CR_SPARAM_RX_PLL_CAP             "rx_pll_cap"
#define CR_SPARAM_RX_POL                 "rx_pol"
#define CR_SPARAM_RX_PPM                 "rx_ppm"
#define CR_SPARAM_RX_PRECODER            "rx_precoder"
#define CR_SPARAM_RX_READAPT             "rx_readapt"
#define CR_SPARAM_RX_READY               "rx_ready"
#define CR_SPARAM_RX_SIGNAL_DETECT       "rx_signal_detect"
#define CR_SPARAM_RX_SKEF                "rx_skef"
#define CR_SPARAM_RX_SKEF_ADDCAP         "rx_skef_addcap"
#define CR_SPARAM_RX_SKEF_DEGEN          "rx_skef_degen"
#define CR_SPARAM_RX_SKEF_EN             "rx_skef_en"
#define CR_SPARAM_RX_SKEF_GAIN           "rx_skef_gain"
#define CR_SPARAM_TX_GRAYCODE            "tx_graycode"
#define CR_SPARAM_TX_MSBLSB              "tx_msblsb"
#define CR_SPARAM_TX_PLL_CAP             "tx_pll_cap"
#define CR_SPARAM_TX_POL                 "tx_pol"
#define CR_SPARAM_TX_PRECODER            "tx_precoder"
#define CR_SPARAM_TX_TAPS                "tx_taps"
#define CR_SPARAM_TX_TAPS_SCALE          "tx_taps_scale"
#define CR_SPARAM_TOP_PLL_CAP            "top_pll_cap"
#define CR_SPARAM_RX_ISI                 "rx_isi"
/** @} */

// utility for c11 that enables serdes
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define cr_serdes_set_param(slice, name, index, val) \
    _Generic(val, double                             \
             : cr_serdes_set_paramf, int             \
             : cr_serdes_set_parami, unsigned        \
             : cr_serdes_set_paramu)(slice, name, index, val)

#define cr_serdes_get_param(slice, name, index, val) \
    _Generic(val, double*                             \
             : cr_serdes_get_paramf, int*             \
             : cr_serdes_get_parami, unsigned*        \
             : cr_serdes_get_paramu)(slice, name, index, val)

#define cr_serdes_set_paramlist(slice, name, index, val, count) \
    _Generic(val, double*                                        \
             : cr_serdes_set_paramlistf, int*                    \
             : cr_serdes_set_paramlisti, unsigned*               \
             : cr_serdes_set_paramlistu)(slice, name, index, val, count)
#define cr_serdes_get_paramlist(slice, name, index, val, count) \
    _Generic(val, double*                                        \
             : cr_serdes_get_paramlistf, int*                    \
             : cr_serdes_get_paramlisti, unsigned*               \
             : cr_serdes_get_paramlistu)(slice, name, index, val, count)
#endif

/**
 * @brief Set SerDes Parameter with an integer value
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_parami(CredoSlice_t* slice, const char* name, int index, int value);
/**
 * @brief Get SerDes Parameter with an integer value
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_parami(CredoSlice_t* slice, const char* name, int index, int* value);

/**
 * @brief Set SerDes Parameter with an integer value
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_paramu(CredoSlice_t* slice, const char* name, int index, unsigned value);
/**
 * @brief Get SerDes Parameter with an unsigned value
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or lane
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_paramu(CredoSlice_t* slice, const char* name, int index, unsigned* value);

/**
 * @brief Set SerDes Parameter with a double value
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] value value to set parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_paramf(CredoSlice_t* slice, const char* name, int index, double value);

/**
 * @brief Get SerDes Parameter with a double value
 *
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] value value of the parameter
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_paramf(CredoSlice_t* slice, const char* name, int index, double* value);

/**
 * @brief Set SerDes Parameter with multiple integer values
 *
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_paramlisti(CredoSlice_t* slice, const char* name, int index, int values[],
                                                    int count);
/**
 * @brief Get SerDes Parameter with multiple integer values
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_paramlisti(CredoSlice_t* slice, const char* name, int index, int values[],
                                                    int count);

/**
 * @brief Set SerDes Parameter with multiple unsigned values
 *
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_paramlistu(CredoSlice_t* slice, const char* name, int index, unsigned values[],
                                                    int count);
/**
 * @brief Get SerDes Parameter with multiple unsigned values
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_paramlistu(CredoSlice_t* slice, const char* name, int index, unsigned values[],
                                                    int count);

/**
 * @brief Set SerDes Parameter with multiple double values
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[in] values values to set
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_set_paramlistf(CredoSlice_t* slice, const char* name, int index, double values[],
                                                    int count);

/**
 * @brief Get parameter with multiple double values
 *
 *
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] index index of parameter, can be top, side, or laner
 * @param[in] name name of the parameter
 * @param[out] values parameter values
 * @param[in] count how many values the parameter contains -- must be the same as the definition
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_paramlistf(CredoSlice_t* slice, const char* name, int index, double values[],
                                                    int count);

/**
 * @brief Get number of SerDes Parameters the slice contains
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[out] count number of parameters
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_param_count(CredoSlice_t* slice, int* count);

/**
 * @brief Get the number of values a SerDes Parameter contains.
 *
 * For parameters that have the CR_PARAM_FLAG_VAR_COUNT flag enabled. This indicates how many of the values are needed
 * in parameters that may have variable size depending upon the mode.
 *
 * If the parameter does not have CR_PARAM_FLAG_VAR_COUNT set then it will return the count from the definition.
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] name name of the parameter
 * @param[in] index index of param val
 * @param[out] count number of values the parameter contains
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_get_param_val_count(CredoSlice_t* slice, const char* name, int index, int* count);

/**
 * @brief Get SerDes parameter definition by index
 *
 * Useful to generate a list of all the serdes param definitions.
 *
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] param_index parameter in list from [0, param_count - 1]
 * @param[out] param parameter information if in bounds
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_index_param_def(CredoSlice_t* slice, int param_index, CredoParam_t* param);

/**
 * @brief Find SerDes parameter definition by name
 * @ingroup SerdesParam
 * @param[in] slice slice to use
 * @param[in] name name of the parameter
 * @param[out] found indicates if the parameter was found
 * @param[out] param parameter information
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_serdes_find_param_def(CredoSlice_t* slice, const char* name, bool* found,
                                                    CredoParam_t* param);

#ifdef __cplusplus
}
#endif

#endif
