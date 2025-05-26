#ifndef CREDO_TCM_H
#define CREDO_TCM_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// TCM R/W
/**
 * @brief Read TCM register
 * @ingroup TCM
 * @param[in] slice slice handle
 * @param[in] addr register address
 * @param[out] data register value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_tcm_read(CredoSlice_t* slice, unsigned addr, unsigned* data);

/**
 * @brief Write TCM regiser
 * @ingroup TCM
 * @param[in] slice slice handle
 * @param[in] addr register address
 * @param[in] data register value
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_tcm_write(CredoSlice_t* slice, unsigned addr, unsigned data);

/* TCM multiple raw address read/write. This go through firmware. 64-bit statistics read is automatic. */

/**
 * @brief Reads continuous TCM registers.
 *
 * This reads a continuous range of TCM registers with help of firmware.
 * 64-bit statistics read is automatically handled.
 *
 * @ingroup TCM
 * @param[in] slice Slice handle
 * @param[in] first_address First TCM register address
 * @param[out] val Pointer to hold returned registers.
 * @param[in] count
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_tcm_burst_read(CredoSlice_t* slice, unsigned first_address, uint64_t val[],
                                             unsigned count);

/**
 * @brief Writes continuous TCM registers.
 *
 * This writes a continuous range of TCM registers with help of firmware.
 *
 * @ingroup TCM
 * @param[in] slice
 * @param[in] first_address
 * @param[out] val
 * @param[in] count
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_tcm_burst_write(CredoSlice_t* slice, unsigned first_address, const uint64_t val[],
                                              unsigned count);
/* PCS register (16-bit) read/write. Address=pcs_base_addr(port, side)+offset*2. */

/**
 * @brief This reads any PCS register of specified port/side.
 *
 * The PCS register map conforms to IEEE 802.3 Clause 45, section 45.2.3.
 *
 * @ingroup PCS
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] side Port side selection
 * @param[in] offset PCS register offset
 * @param[out] val Holds value to be returned
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_pcs_read(CredoSlice_t* slice, uint32_t portId, CredoSide_t side, unsigned offset,
                                       unsigned* val);

/**
 * @brief This writes any PCS register of specified port/side.
 *
 * The PCS register map conforms to IEEE 802.3 Clause 45, section 45.2.3.
 *
 * @ingroup PCS
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] side Port side selection
 * @param[in] offset PCS register offset
 * @param[in] val Value to be written
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_pcs_write(CredoSlice_t* slice, uint32_t portId, CredoSide_t side, unsigned offset,
                                        unsigned val);
/* PCS status read. Equivalent to read PCS offset 1 (address 0x02). */

/**
 * @brief This reads PCS register 1 (PCS status 1) of specified port/side.
 *
 * See IEEE 802.3 section 45.2.3.2 for PCS status 1. Note the link status (bit 2) is latching low.
 *
 * @ingroup PCS
 * @param[in] slice
 * @param[in] portId
 * @param[in] side
 * @param[out] pcs_status
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_pcs_status_read(CredoSlice_t* slice, uint32_t portId, CredoSide_t side,
                                              unsigned* pcs_status);

/* MAC register (32-bit) read/write. Address=mac_base_addr(port, side)+offset*4.
 * *** For status register (reg 16), must use mac_status_read() */

/**
 * @brief Reads any MAC register of specified port/side.
 *
 * See MAC register map for more information. For MAC status register (register 16),
 * use function mac_status_read() instead.
 *
 * @ingroup MAC
 * @param[in] slice
 * @param[in] portId
 * @param[in] side
 * @param[in] offset
 * @param[out] val
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_read(CredoSlice_t* slice, uint32_t portId, CredoSide_t side, unsigned offset,
                                       unsigned* val);

/**
 * @brief Writes any MAC register of specified port/side.
 *
 * See MAC register map for more information.
 *
 * @ingroup MAC
 * @param[in] slice
 * @param[in] portId
 * @param[in] side
 * @param[in] offset
 * @param[in] val
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_write(CredoSlice_t* slice, uint32_t portId, CredoSide_t side, unsigned offset,
                                        unsigned val);
/* MAC status read. Equivalent to read MAC offset 16 (address 0x40), but goes through firmware */

/**
 * @brief Reads MAC status register (MAC register 16) of specified port/side.
 *
 * Note the fault status (bit 0 and bit 1) are latch low.
 *
 * @ingroup MAC
 * @param[in] slice
 * @param[in] portId
 * @param[in] side
 * @param[out] mac_status
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_status_read(CredoSlice_t* slice, uint32_t portId, CredoSide_t side,
                                              unsigned* mac_status);
/* MAC statistics register (32/64-bit) read/write. Address=mac_stat_base_addr(port, side)+offset*4.
 * For 64-bit registers, MSB are in register DATA_HI. */

/**
 * @brief Reads MAC statistics register for specified port/side.
 *
 * For 64-bit statistics, this returns 32 LSB only. The 32 MSBs can be read from register DATA_HI (MAC register 7) after
 * LSB is read.
 *
 * @ingroup MAC
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] side Port side selection
 * @param[in] offset MAC statistics register offset
 * @param[out] val Holds value to be returned
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_stats_read(CredoSlice_t* slice, uint32_t portId, CredoSide_t side, unsigned offset,
                                             unsigned* val);
/**
 * @brief Writes MAC statistics register for specified port/side.
 *
 * @ingroup MAC
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] side Port side selection
 * @param[in] offset MAC statistics register offset
 * @param[in] val Value to be written
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_stats_write(CredoSlice_t* slice, uint32_t portId, CredoSide_t side, unsigned offset,
                                              unsigned val);

/**
 * @brief Reads MAC statistics counters
 *
 * @ingroup MAC
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] side Port side selection
 * @param[out] stats MAC statistics counters
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_stats_read_counters(CredoSlice_t* slice, uint32_t portId, CredoSide_t side,
                                                      CredoMACStatistics_t* stats);

/**
 * @brief Clears MAC statistics counters
 *
 * @ingroup MAC
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] side Port side selection
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_stats_clear_counters(CredoSlice_t* slice, uint32_t portId, CredoSide_t side);

/* MAC and EIP stop function before port is stopped. */

/**
 * @brief Stops MAC before port is destroyed.
 * @ingroup MAC
 * @param[in] slice
 * @param[in] portId
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_stop(CredoSlice_t* slice, uint32_t portId);

/**
 * @brief Stops EIP before port is destroyed.
 * @ingroup EIP
 * @param[in] slice
 * @param[in] portId
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_stop(CredoSlice_t* slice, uint32_t portId);

/* MAC and EIP configuration function. Should be replaceable */

/**
 * @brief Configures EIP when port is created.
 * @ingroup EIP
 * @param[in] slice
 * @param[in] portId
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_configure(CredoSlice_t* slice, uint32_t portId);

/**
 * @brief Configures MAC when port is created.
 * @ingroup MAC
 * @param[in] slice
 * @param[in] portId
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_configure(CredoSlice_t* slice, uint32_t portId);
/* Per-slice topology control. Must change without running ports. */

/**
 * @brief Print to logs the supported topology types
 * @ingroup Topology
 * @param[in] slice
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_display_topologies(CredoSlice_t* slice);

/**
 * @brief Set the topology of the slice.
 *
 * The topology shall only be changed when no port is configured.
 *
 * @ingroup Topology
 * @param[in] slice
 * @param[in] topology
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_set_topology(CredoSlice_t* slice, const char* topology);

/**
 * @brief Get the current running topology of the slice.
 * @ingroup Topology
 * @param[in] slice
 * @param[out] topology
 * @param[in] len
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_get_topology(CredoSlice_t* slice, char* topology, unsigned len);

/**
 * @brief Packet inject in the given port.
 * Note that this function is still experimental
 * @ingroup EIP
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] side Side selection
 * @param[in] pkt_config Inject Packet configuration
 * @return NS CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_inject_packet(CredoSlice_t* slice, uint32_t portId, CredoSide_t side,
                                                CredoPacketInjectConfig_t* pkt_config);

/**
 * @brief Change the fault propagation control of specified port.
 * @ingroup Fault
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] enable The desired fault propagation mode for this port
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_set_faultprop(CredoSlice_t* slice, uint32_t portId, CredoFaultPropagation_t enable);

/**
 * @brief Get the current status of fault propagation control.
 * @ingroup Fault
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[out] enable The current fault propagation mode for this port
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_mac_get_faultprop(CredoSlice_t* slice, uint32_t portId, CredoFaultPropagation_t* enable);

/**
 * @brief Enable/Disable low latency bypass with respect to MACsec flow
 * @ingroup EIP
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] direction MACsec direction
 * @param[in] enable
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_set_low_latency_bypass(CredoSlice_t* slice, uint32_t portId,
                                                         CredoMACsecDirection_t direction, uint8_t enable);

/**
 * @brief Retrieve low latency bypass status with respect to MACsec flow
 * @ingroup EIP
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] direction MACsec direction
 * @param[out] enable Bypass status
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_get_low_latency_bypass(CredoSlice_t* slice, uint32_t portId,
                                                         CredoMACsecDirection_t direction, uint8_t* enable);

/**
 * @brief Get the client ID of specified port/side.
 * @ingroup EIP
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[in] side Side selection
 * @param[out] client Holds value to be returned
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_get_client_id(CredoSlice_t* slice, uint32_t portId, CredoSide_t side,
                                                uint32_t* client);

/**
 * @brief Get the channel id of specified port.
 * @ingroup EIP
 * @param[in] slice Slice handle
 * @param[in] portId Port ID
 * @param[out] channel_id Holds value to be returned
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_get_channel_id(CredoSlice_t* slice, uint32_t portId, uint32_t* channel_id);

/**
 * @brief Return the data path of the given MACsec flow.
 * @ingroup TCM
 * @param[in] slice Slice handle
 * @param[in] direction MACsec direction
 * @param[out] datapath Returned data path structure
 * @return  CredoErrorCodes_t
 */
CREDOAPI CredoErrorCodes_t cr_eip_get_macsec_datapath(CredoSlice_t* slice, CredoMACsecDirection_t direction,
                                                      CredoMACsecDataPath_t* datapath);

#ifdef __cplusplus
}
#endif

#endif
