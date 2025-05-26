/**
 * Credo Public API Header.
 *
 * This is an amalgamation of headers seen in the public headers folder. You may use either.
 */
/*
 * credo.h
 *
 * Property of Credo Technology Group. Unauthorized use prohibited.
 * All rights reserved.
 *
 * --CONFIDENTIAL--
 */
#ifndef CREDO_H
#define CREDO_H

// #include "credo/device.h"
#ifndef CREDO_DEVICE_H
#define CREDO_DEVICE_H

// #include "credo/types.h"
#ifndef CREDO_TYPES_H
#define CREDO_TYPES_H

#ifndef SWIG
#include <stdbool.h>
#include <stdint.h>
#endif

#ifdef _WIN32
#ifdef BUILD_HAL
#define CREDOAPI __declspec(dllimport)
#else
#define CREDOAPI __declspec(dllexport)
#endif
#else
#define CREDOAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
#ifndef CR_CPPAPI
/**
 * @brief Log levels of the sdk.
 *
 * @ingroup Logger
 */
typedef enum {
    CR_LOG_ERROR = 0,  // error logging
    CR_LOG_WARN = 1,
    CR_LOG_INFO = 2,
    CR_LOG_DEBUG = 3,
    CR_LOG_TRACE = 4
} CredoLogLevel_t;
#else
typedef enum {
    CR_LOG_ERROR = 0,  // error logging
    CR_LOG_WARN = 1,
    CR_LOG_INFO = 2,
    CR_LOG_DEBUG = 3,
    CR_LOG_TRACE = 4
} CredoLogLevel_t;
#endif

/* SDK information output, debug, error */

/**
 * @ingroup Lib
 * A list of corresponding error code types. The list is sparse as most error code information will be kept outputted in
 * the logger.
 */
typedef enum {
    CR_SUCCESS = 0,
    CR_FAIL = -1,
    CR_PHY_FW_DOWNLOAD_FAIL = -3,
    CR_FW_TIMEOUT = -4,
    CR_NOT_READY = -5,
    CR_OUT_OF_MEMORY = -6,
    CR_HAL_LOAD_FAIL = -7,
    CR_HAL_NOT_FOUND = -8,

    CR_INVALID_ARGS = -9,
    CR_MUTEX_TIMEOUT = -10,
    CR_UNSUPPORTED = -99,
    CR_NOTIMPLEMENTED = -100
} CredoErrorCodes_t;

// Driver API

/**
 * @ingroup SDKDriver
 * Read register function signature that a driver must provide.
 * @param[in] slice_context provides the slice context to read from the slice
 * @param[in] reg_addr register address to read
 * @param[out] val pass back the value
 * @return Error Code if driver fails to read value
 */
typedef CredoErrorCodes_t (*CredoReadRegister_t)(void* slice_context, unsigned reg_addr, unsigned* val);

/**
 * @ingroup SDKDriver
 * Write register signature that a driver must provide.
 * @param[in] slice_context provides the slice context to write to the slice
 * @param[in] reg_addr register address to write
 * @param[in] val value to write
 * @return Error Code if driver fails to write value
 */
typedef CredoErrorCodes_t (*CredoWriteRegister_t)(void* slice_context, unsigned reg_addr, unsigned val);

/* Optional device access */

/**
 * @brief Optional burst read register signature
 * @ingroup SDKDriver
 * @param[in] slice_context provides the slice context to burst read the slice
 * @param[in] first_addr the initial address to read from
 * @param[out] val where the read values should be stored
 * @param[in] count how many registers to read
 */
typedef CredoErrorCodes_t (*CredoReadRegisterBurst_t)(void* slice_context, unsigned first_addr, unsigned val[],
                                                      unsigned count);

/**
 * @brief Optional burst write register signature
 * @ingroup SDKDriver
 * @param[in] slice_context provides the slice context to burst write the slice
 * @param[in] first_addr the initial address to write to
 * @param[in] val register values to write
 * @param[in] count how many registers to write
 */
typedef CredoErrorCodes_t (*CredoWriteRegisterBurst_t)(void* slice_context, unsigned first_addr, const unsigned val[],
                                                       unsigned count);
/**
 * @brief Optional broadcast register signature
 * @ingroup SDKDriver
 * @param[in] slice_context provides the slice context to broadcast write to the slice
 * @param[in] reg_addr register address to write
 * @param[in] val value to write
 * @return Error Code if driver fails to write value
 */
typedef CredoErrorCodes_t (*CredoWriteRegisterBroadcast_t)(void* slice_context, unsigned reg_addr, unsigned val);

/**
 * @brief Optional broadcast burst write register signature
 * @ingroup SDKDriver
 * @param[in] slice_context provides the slice context to broadcast burst write the slice
 * @param[in] first_addr the initial address to write to
 * @param[in] val register values to write
 * @param[in] count how many registers to write
 */
typedef CredoErrorCodes_t (*CredoWriteRegisterBroadcastBurst_t)(void* slice_context, unsigned first_addr,
                                                                const unsigned val[], unsigned count);

// Logging API
/**
 * @brief Logger function signature
 * @ingroup SDKDriver
 * @param[in] slice_context optional slice_context if it is a log pertaining to a slice
 * @param[in] user_data optionally provided user_data set for a slice
 * @param[in] scope the scope of the message (slice id or global)
 * @param[in] level the log level of the message
 * @param[in] message the log message
 */
typedef void (*CredoLog_t)(void* slice_context, void* user_data, CredoLogLevel_t level, const char* scope,
                           const char* message);
/**
 * @brief Where to pass in driver and logger functions.
 * @ingroup SDKConfig
 */
typedef struct {
    CredoLog_t log;                       //!< Log function pointer
    CredoReadRegister_t read_register;    //!< Read register function pointer
    CredoWriteRegister_t write_register;  //!< Write register function pointer
    CredoLogLevel_t max_log_level;        //!< Max logging level setting
} CredoSdkConfig_t;

/**
 * @brief Lane Configuration Structure
 * @ingroup ExLaneConfig
 * Used to configure the lanes for properties that the firmware cannot handle.
 *
 */
typedef struct {
    uint8_t tx_polarity_swap;  //!< whether to swap tx polarity
    uint8_t rx_polarity_swap;  //!< whether to swap rx polarity
    // TODO: remove this isnt needed
    uint8_t facing_line_side;  //!< is the lane line side or host side
    uint8_t using_dc_input;    //!< is the lane ac or dc coupled
} CredoLaneConfig_t;

/**
 * @brief Slice init type enum
 * @ingroup SliceConfig
 * Used to configure the lanes for properties that the firmware cannot handle.
 *
 */
typedef enum {
    CR_INIT_FULL = 0,         //!< full reset, load firmware, initialize any sensible default settings
    CR_INIT_NO_FIRMWARE = 1,  //!< fully reset, do not load firmware, initialize any sensible default settings
    CR_INIT_WARM = 2,         //!< do not change any setting, fetch any settings from firmware (“warm boot”)
    CR_INIT_NONE = 3          //!< do nothing, except setup the data structure (reg read/write setup)
} CredoSliceInitType_t;

/**
 * @brief Slice Configuration Structure
 * @ingroup SliceConfig
 */
typedef struct {
    CredoSliceInitType_t init_type;  //!< init type
    const char* firmware_filename;   //!< path to firmware file to load
    uint32_t slice_id;               //!< identifier of the slice for logging purposes and crsh slice selection
    void* slice_context;  //!< provide register access information. NOTE: this value must be in heap or globally as it
                          //!< is stored as a reference in the slice handle
} CredoSliceConfig_t;

/**
 * @brief Slice Capability Structure
 * @ingroup SliceInformation
 */
typedef struct {
    int max_ports;     //!< max ports
    int max_lanes;     //!< max lanes
    int max_vsensors;  //!< max voltage sensors
} CredoSliceLimit_t;

/**
 * @brief Fec analyzer error type
 * @ingroup LanePRBSFecAnalyzer
 */
typedef enum { CR_FEC_ERROR_FRAME = 0, CR_FEC_ERROR_SYMBOL = 1, CR_FEC_ERROR_BIT = 2 } CredoFecErrorType_t;

/**
 * @brief Fec analyzer Configuration Structure
 * @ingroup LanePRBSFecAnalyzer
 */
typedef struct {
    uint16_t codeword_size;  //!< in bits, for 802.3 typically 5440 (PAM4) or 5280 (NRZ)
    uint16_t symbol_size;    //!< for 802.3 typically 10
    uint16_t threshold;      //!< for 802.3 typically 15 (PAM4) or 7 (NRZ)
    CredoFecErrorType_t error_type;
} CredoFecAnalyzerConfig_t;

/**
 * @brief Eye monitor flag to indicate destructive mode
 * @ingroup SerdesEyeMonitor
 */
#define CR_EYE_MONITOR_DESTRUCTIVE (1 << 0)

/**
 * @brief Eye monitor flag to indicate bathtub mode
 * @ingroup SerdesEyeMonitor
 */
#define CR_EYE_MONITOR_BATHTUB (1 << 1)

/**************************************************************************/
/* Context types. These are all opaque. */

/* Declare the structure first. Some old compiler just doesn't support it. */
struct CredoSdk;
struct CredoDevice;
struct CredoSlice;
/**
 * @brief Opaque handle that store driver and logger functions.
 * @ingroup SDKDriver
 * The SDK handle is opaque to simplify the public API.
 *
 */
typedef struct CredoSdk CredoSdk_t;

/**
 * @brief Opaque handle that stores slices and device specific information
 * @ingroup DeviceConfig
 * The Device handle is opaque to simplify the public API.
 */
typedef struct CredoDevice CredoDevice_t;

/**
 * @brief Opaque handle that stores slice information
 * @ingroup SliceConfig
 * The Slice handle is opaque to simplify the public API.
 */
typedef struct CredoSlice CredoSlice_t;

/**
 * @brief Credo device types
 * @ingroup DeviceConfig
 *
 * All the possible credo device types that are supported by the sdk.
 */
typedef enum {

    CREDO_BALDEAGLE_400 = 0x001001,
    CREDO_BALDEAGLE_800 = 0x001002,

    CREDO_OWL_400 = 0x002001,
    CREDO_OWL_800 = 0x002002,

    CREDO_BLACKHAWK_400 = 0x003001,
    CREDO_BLACKHAWK_800 = 0x003002,

    CREDO_HERON_P1 = 0x004001,
    CREDO_HERON_P3 = 0x004002,
    CREDO_HERON_1P0 = 0x004003,

    CREDO_OSPREY_400 = 0x005001,
    CREDO_OSPREY_800 = 0x005002,
    CREDO_OSPREY_AEC = 0x005003,

    CREDO_NUTCRACKER_32 = 0x006001,

    CREDO_KITE_TEST = 0x007001,

    CREDO_ADMIRAL_TEST = 0x008001,

    CREDO_SEAHAWK = 0x00A001,

    CREDO_FAKE_400 = 0x100001,
    CREDO_FAKE_800 = 0x100002
} CredoDeviceType_t;

/**
 * @brief Credo slice types
 * @ingroup SliceInformation
 *
 * All the possible credo slice types that are supported by the sdk.
 */
typedef enum {

    CREDO_BALDEAGLE = 0x000100,

    CREDO_OWL = 0x000200,
    CREDO_OWL_A0 = 0x000201,
    CREDO_OWL_B0 = 0x000202,
    CREDO_OWL_B0B = 0x000203,
    CREDO_OWL_B1 = 0x000204,

    CREDO_BLACKHAWK = 0x000300,
    CREDO_BLACKHAWK_DC = 0x000301,
    CREDO_BLACKHAWK_AC = 0x000302,

    CREDO_HERON = 0x000400,

    CREDO_OSPREY = 0x000500,

    CREDO_NUTCRACKER = 0x000600,

    CREDO_KITE = 0x000700,

    CREDO_ADMIRAL = 0x000800,

    CREDO_SLC_SEAHAWK = 0x000A00,

    CREDO_FAKE = 0x100000
} CredoSliceType_t;

/**************************************************************************/
/* Port and operation modes */

/**
 * @brief Identifer for the connection type of the port
 * @ingroup Port
 * Used to indicate what type of mode to set the port depending on usage.
 *
 */
typedef enum {
    CR_PORT_RETIMER,  //!< CR_PORT_RETIMER Mode
    CR_PORT_BITMUX,   //!< CR_PORT_BITMUX Mode
    CR_PORT_GEARBOX   //!< CR_PORT_GEARBOX Mode
} CredoPortConnectionMode_t;

/**
 * @brief Identifier to indicate how the port connects with the next layer.
 * @ingroup Port
 */
typedef enum {
    CR_PMODE_SERDES,  //!< basic Lane only mode
    CR_PMODE_PCS,     //!< serdes + pcs used (no macsec)
    CR_PMODE_MACSEC   //!< macsec all blocks used
} CredoPortMode_t;

/**
 * @brief CR_COUPLING_AC or CR_COUPLING_DC coupling of receiver
 * @ingroup ExSerdes
 */
typedef enum {
    CR_COUPLING_DC = 0,  //!< CR_COUPLING_DC Coupling
    CR_COUPLING_AC       //!< CR_COUPLING_AC Coupling
} CredoLaneCoupling_t;

/**
 * @brief The operation mode of the lane
 * @ingroup LaneMode
 */
typedef enum {
    CR_LMODE_OFF = 0,
    CR_LMODE_NRZ = 1,
    CR_LMODE_PAM4 = 2,
    CR_LMODE_AN = 3,
    CR_LMODE_DISABLE = 0xff
} CredoLaneMode_t;

/**
 * @brief Fec type of the port
 * @ingroup Port
 */
typedef enum {
    CR_FEC_NONE,  //!< NO FEC
    CR_FEC_FIRE_CODE,
    CR_FEC_RS_528,
    CR_FEC_RS_544
} CredoFecType_t;

/**
 * @brief PortRsfec fifo status
 * @ingroup PortRsfec
 */
typedef struct {
    uint32_t tx_min;
    uint32_t tx_cur;
    uint32_t tx_max;
    uint32_t rx_min;
    uint32_t rx_cur;
    uint32_t rx_max;
} CredoRSFECFifo_t;

/**
 * @brief PortRsfec status
 * @ingroup PortRsfec
 */
typedef struct {
    bool pcs_aligned;   //!< FEC encoder lock and align status
    bool fec_aligned;   //!< RS-FEC receive lanes lock and align status
    bool AM_locked[4];  //!< RS-FEC receivce lane[0-4] lock and align status
} CredoRSFECStatus_t;

/**
 * @brief Sram status
 * @ingroup Sram
 */
typedef enum {
    CR_SRAM_NO_ERROR,  //!< Sram ECC no error
    CR_SRAM_CORR_ERROR,
    CR_SRAM_UNCORR_ERROR
} CredoSramStatus_t;

/**
 * @brief
 * @ingroup TCM
 */
typedef enum {
    CR_SIDE_SYSTEM,  //!< System Side
    CR_SIDE_LINE
} CredoSide_t;

/**
 * @brief
 * @ingroup TCM
 */
typedef enum {
    CR_MACSEC_EGRESS,  //!< MACsec Egress
    CR_MACSEC_INGRESS
} CredoMACsecDirection_t;

/**
 * @brief Prbs pattern of Lane
 * @ingroup LanePRBS
 */
typedef enum {
    CR_PRBS7 = 0,
    CR_PRBS9 = 1,
    CR_PRBS11 = 2,
    CR_PRBS13 = 3,
    CR_PRBS15 = 4,
    CR_PRBS23 = 5,
    CR_PRBS31 = 6,
    CR_PRBS_UNKNOWN = 100
} CredoLanePrbsPattern_t;

/**
 * @brief State of the transmitter
 * @ingroup ExLaneTestPattern
 */
typedef enum {
    CR_TESTPATT_CUSTOM,  //!< Use custom bit pattern
    CR_TESTPATT_JP03A,
    CR_TESTPATT_JP03B,
    CR_TESTPATT_LINEAR,
    CR_TESTPATT_UNKNOWN = 100
} CredoLaneTxTestPatternMode;

/**
 * @brief Lane TX
 * @ingroup ExLaneTestPattern
 */
typedef enum {
    CR_TX_UNKNOWN,
    CR_TX_LOWPOWER,
    CR_TX_TRAFFIC,
    CR_TX_SQUELCH,
    CR_TX_PRBS_PAM4,
    CR_TX_PRBS_NRZ,
    CR_TX_FORCE_DISABLE,
    CR_TX_FORCE_PRBSS_PAM4,
    CR_TX_FORCE_PRBS_NRZ,
    CR_TX_FORCE_TRAFFIC,
    CR_TX_FORCE_TEST_PATT
} CredoLaneTxState_t;

/**
 * @brief Indicate the loopback mode type of the lane
 * @ingroup ExLaneMode
 */
typedef enum {
    CR_LB_DISABLED,  // No loopback enabled
    CR_LB_TX_TO_RX,
    CR_LB_RX_TO_TX
} CredoLaneLoopbackMode_t;

/* Fault propagation control */
/**
 * @brief
 * @ingroup Fault
 */
typedef enum {
    CR_FAULTPROP_NONE = 0,
    CR_FAULTPROP_LINE_TO_SYS = 1,
    CR_FAULTPROP_SYS_TO_LINE = 2,
    CR_FAULTPROP_BOTH = 3
} CredoFaultPropagation_t;

/**
 * @brief Auto assign port
 * @ingroup Port
 *
 * Use as the port id.
 */
#define CR_PORT_AUTO_ASSIGN_ID 0x10000

/**
 * @brief
 * @ingroup Port
 * @note it is the same value as CR_PORT_AUTO_ASSIGN_ID.
 * When querying a port will return this value in port_id if the port is not configured. Note
 */
#define CR_PORT_UNCONFIGURED 0x10000

// flags for CredoPortConfig_t use
/**
 * @brief Port config flag to indicate line side optical
 * @ingroup Port
 */
#define CR_PFLAG_LINE_SIDE_OPTICAL (1 << 0)

/**
 * @brief Port config flag to indicate line side ANLT
 * @ingroup Port
 */
#define CR_PFLAG_LINE_SIDE_ANLT (1 << 1)

/**
 * @brief Port config flag to indicate system side LT.
 * @details This is only effective for PAM4/Clause 136 link training.
 *          For NRZ link in system side, this flag is ignored.
 *          For Lane only modes, this flag is also ignored.
 * @ingroup Port
 */
#define CR_PFLAG_SYS_SIDE_LT (1 << 2)

/**
 * @brief Port config flag to indicate system side optical.
 *
 * @ingroup Port
 */
#define CR_PFLAG_SYS_SIDE_OPTICAL (1 << 3)

/**
 * @brief Port config flag to indicate line side AN
 * @ingroup Port
 */
#define CR_PFLAG_LINE_SIDE_AN CR_PFLAG_LINE_SIDE_ANLT

/**
 * @brief Port config flag to indicate line side LT
 * @ingroup Port
 */
#define CR_PFLAG_LINE_SIDE_LT (CR_PFLAG_LINE_SIDE_ANLT | CR_PFLAG_AUTONEG_DISABLE)

/**
 * @brief Port config flag to override auto negotiation
 * @ingroup Port
 */
#define CR_PFLAG_AUTONEG_OVERRIDE (1 << 8)

/**
 * @brief Port config flag to disable line side auto negotiation.
 * @details This is only effective for PAM4/Clause 136 link training.
 *          If line side is configured as NRZ mode, an error will be returned.
 *          This flag has higher priority than @ref CR_PFLAG_AUTONEG_OVERRIDE.
 * @ingroup Port
 */
#define CR_PFLAG_AUTONEG_DISABLE (1 << 9)

/**
 * @brief Port config flag to enable double CRC (double CRC is disabled by default)
 * @ingroup Port
 */
#define CR_PFLAG_ENABLE_DOUBLE_CRC (1 << 16)

/**
 * @brief Information needed to configure a port
 * @ingroup Port
 */
typedef struct {
    uint32_t port_id;                           //!< Which port id to use in firmware
    uint32_t flags;                             //!< special flags to set about the port
    CredoPortConnectionMode_t connection_mode;  //!< type of port to configure
    CredoPortMode_t port_mode;                  //!< how the port is connected
    uint32_t speed;                             //!< Speed of the port in Kb/s
    uint32_t line_start_lane;                   //!< initial lane of line side to use
    uint32_t host_start_lane;                   //!< initial lane of host side to use
    uint32_t line_no_of_lanes;                  //!< how many lanes are used on line side
    uint32_t host_no_of_lanes;                  //!< how many lanes are used on host side
    CredoFecType_t line_fec_type;               //!< line side fec type to use
    CredoFecType_t host_fec_type;               //!< host side fec type to use
} CredoPortConfig_t;

/**
 * @brief Slice Option
 * @ingroup SliceOption
 */
typedef struct {
#ifdef SWIG
%immutable;
#endif
    const char* name;         //!< option name
    const char* description;  //!< option description
#ifdef SWIG
%mutable;
#endif
} CredoSliceOption_t;

/**
 * @brief Port Option
 * @ingroup PortOption
 */
typedef struct {
#ifdef SWIG
%immutable;
#endif
    const char* name;         //!< option name
    const char* description;  //!< option description
#ifdef SWIG
%mutable;
#endif
} CredoPortOption_t;

/**
 * @brief Lane Option
 * @ingroup LaneOption
 */
typedef struct {
#ifdef SWIG
%immutable;
#endif
    const char* name;         //!< option name
    const char* description;  //!< option description
#ifdef SWIG
%mutable;
#endif
} CredoLaneOption_t;

/**
 * @brief MAC statistics counters
 * @ingroup MAC
 */
typedef struct {
    // RX statistic counters
    uint64_t etherStatsRxOctets;
    uint64_t OctetsReceivedOK;
    uint64_t aAlignmentErrors;
    uint64_t aPAUSEMACCtrlFramesReceived;
    uint64_t aFrameTooLongErrors;
    uint64_t aInRangeLengthErrors;
    uint64_t aFramesReceivedOK;
    uint64_t aFrameCheckSequenceErrors;
    uint64_t VLANReceivedOK;
    uint64_t ifInErrors;
    uint64_t ifInUcastPkts;
    uint64_t ifInMulticastPkts;
    uint64_t ifInBroadcastPkts;
    uint64_t etherStatsDropEvents;
    uint64_t etherStatsRxPkts;
    uint64_t etherStatsUndersizePkts;
    uint64_t etherStatsRxPkts64Octets;
    uint64_t etherStatsRxPkts65to127Octets;
    uint64_t etherStatsRxPkts128to255Octets;
    uint64_t etherStatsRxPkts256to511Octets;
    uint64_t etherStatsRxPkts512to1023Octets;
    uint64_t etherStatsRxPkts1024to1518Octets;
    uint64_t etherStatsRxPkts1519toMaxOctets;
    uint64_t etherStatsOversizePkts;
    uint64_t etherStatsJabbers;
    uint64_t etherStatsFragments;
    uint64_t aCBFCPAUSEFramesReceived_0;
    uint64_t aCBFCPAUSEFramesReceived_1;
    uint64_t aCBFCPAUSEFramesReceived_2;
    uint64_t aCBFCPAUSEFramesReceived_3;
    uint64_t aCBFCPAUSEFramesReceived_4;
    uint64_t aCBFCPAUSEFramesReceived_5;
    uint64_t aCBFCPAUSEFramesReceived_6;
    uint64_t aCBFCPAUSEFramesReceived_7;
    uint64_t aMACControlFramesReceived;

    // TX statistic counters
    uint64_t etherStatsTxOctets;
    uint64_t OctetsTransmittedOK;
    uint64_t aPAUSEMACCtrlFramesTransmitted;
    uint64_t aFramesTransmittedOK;
    uint64_t VLANTransmittedOK;
    uint64_t ifOutErrors;
    uint64_t ifOutUcastPkts;
    uint64_t ifOutMulticastPkts;
    uint64_t ifOutBroadcastPkts;
    uint64_t etherStatsTxPkts64Octets;
    uint64_t etherStatsTxPkts65to127Octets;
    uint64_t etherStatsTxPkts128to255Octets;
    uint64_t etherStatsTxPkts256to511Octets;
    uint64_t etherStatsTxPkts512to1023Octets;
    uint64_t etherStatsTxPkts1024to1518Octets;
    uint64_t etherStatsTxPkts1519toMaxOctets;
    uint64_t aCBFCPAUSEFramesTransmitted_0;
    uint64_t aCBFCPAUSEFramesTransmitted_1;
    uint64_t aCBFCPAUSEFramesTransmitted_2;
    uint64_t aCBFCPAUSEFramesTransmitted_3;
    uint64_t aCBFCPAUSEFramesTransmitted_4;
    uint64_t aCBFCPAUSEFramesTransmitted_5;
    uint64_t aCBFCPAUSEFramesTransmitted_6;
    uint64_t aCBFCPAUSEFramesTransmitted_7;
    uint64_t aMACControlFramesTransmitted;
    uint64_t etherStatsTxPkts;
} CredoMACStatistics_t;

/**
 * @brief MACsec data path for Egress and Ingress
 * @ingroup TCM
 */
typedef struct {
    uint32_t start_offset1;
    uint32_t end_offset1;
    uint32_t start_offset2;
    uint32_t end_offset2;
} CredoMACsecDataPath_t;

/**
 * @brief Indicate the packet inject mode
 * @ingroup TCM
 */
typedef enum { CR_PKTINJ_FUNC, CR_PKTINJ_DEBUG } CredoPacketInjectMode_t;

/**
 * @brief Packte inject configuration
 * @ingroup TCM
 */
typedef struct {
    CredoPacketInjectMode_t* mode;
    bool* infinite;
    uint32_t* packet_number;
    uint16_t* packet_len;
    uint8_t* packet_data;
#ifdef SWIG
%immutable;
#endif
    uint16_t packet_data_len;
#ifdef SWIG
%mutable;
#endif
} CredoPacketInjectConfig_t;
/**
 * @brief prbs lock status
 * @ingroup LanePRBS
 */
typedef enum { CR_PRBS_LOCK_NO, CR_PRBS_LOCK_YES, CR_PRBS_LOCK_INVALID } CredoPrbsLockStatus_t;

/**
 * @brief Indicates the return type of the serdes parameter
 * @ingroup SerdesParam
 *
 */
typedef enum {
    CR_PARAM_VAL_INT,
    CR_PARAM_VAL_UINT,  // user must cast to unsigned (I doubt it would be used often)
    CR_PARAM_VAL_FLOAT
} CredoParamValue_t;

/**
 * @brief Indicates the index type of the parameter
 * @ingroup SerdesParam
 * Some examples are lane, top (only 1), or side index type.
 *
 */
typedef enum { CR_PARAM_INDEX_TOP, CR_PARAM_INDEX_SIDE, CR_PARAM_INDEX_LANE, CR_PARAM_INDEX_PORT } CredoParamIndex_t;

/**
 * @addtogroup SerdesParam
 * @{
 */
#define CR_PARAM_TYPE_CONTROL       "control"
#define CR_PARAM_TYPE_CONTROL_DEBUG "control-debug"
#define CR_PARAM_TYPE_OPTION        "option"
#define CR_PARAM_TYPE_PARAM         "param"
#define CR_PARAM_TYPE_STATUS        "status"

/** @} */

/**
 * @addtogroup SerdesParam
 * @{
 */
#define CR_PARAM_FLAG_VAR_COUNT 0x1
/** @} */

/**
 * @brief SerdesParam Parameter definition
 * @ingroup SerdesParam
 */
typedef struct {
#ifdef SWIG
%immutable;
#endif
    const char* name;         //!< name of the serdes param
    const char* description;  //!< desription of serdes param
    const char* type;         //!< specific subdomain of parameters
#ifdef SWIG
%mutable;
#endif
    CredoParamIndex_t index_type;  //!< what kind of index does the parameter have
    CredoParamValue_t val_type;    //!< indicate value return type
    bool has_setter;               //!< user can set a value
    bool has_getter;               //!< user can get a value
    int count;                     //!< count 1= not multi, otherwise it is a multi parameter
    uint64_t flags;                //!< special flags about the lane param
} CredoParam_t;

#ifdef __cplusplus
}
#endif

#endif  // CREDO_BASE_H


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

// #include "credo/firmware.h"
#ifndef CREDO_FW_H
#define CREDO_FW_H

// #include "credo/types.h"


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

// #include "credo/lane.h"
#ifndef CREDO_LANE_H
#define CREDO_LANE_H

// #include "credo/types.h"


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

// #include "credo/port.h"
#ifndef CREDO_PORT_H
#define CREDO_PORT_H

// #include "credo/types.h"


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

// #include "credo/sdk.h"
#ifndef CREDO_SDK_H
#define CREDO_SDK_H

// #include "credo/types.h"


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

// #include "credo/serdes.h"
#ifndef CREDO_SERDES_H
#define CREDO_SERDES_H

// #include "credo/types.h"


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

// #include "credo/shell.h"
#ifndef CREDO_SHELL_H
#define CREDO_SHELL_H

// #include "credo/types.h"


#ifdef __cplusplus
extern "C" {
#endif

#define CR_CMDLINE_SIZE 1024

/**
 * @ingroup ShellIntegration
 * readline function signature that is custom readline function
 * @param[in] prompt prompt string
 * @param[out] input input string
 * @return Error Code
 */
typedef CredoErrorCodes_t (*CredoReadLine_t)(const char* prompt, char input[CR_CMDLINE_SIZE]);

/**
 * @brief Set custom shell readline function
 * @ingroup ShellIntegration
 * @param[in] func readline function pointer
 */
CREDOAPI void cr_shell_set_readline(CredoReadLine_t func);

/**
 * @brief Get all available shell commands
 *
 * Iterate through all the available shell commands with an increasing index until NULL is provided.
 *
 * @note there may be some empty "" strings that should be ignored.
 *
 * @ingroup ExShell
 * @param[in] index command index
 * @return command name string (returns NULL on out of index)
 */
CREDOAPI const char* cr_shell_get_command_name(int index);

/**
 * @brief Run a single shell command
 * @ingroup ShellCommands
 * @param[in] slice slice handle, if NULL it uses currently selected slice
 * @param[in] cmdline command string
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_run_command(CredoSlice_t* slice, const char* cmdline);

/**
 * @brief Run a lua slash command
 *
 * Doesn't enter the shell so you can run this in multiple threads.
 *
 * A more powerful replacement to cr_slice_display_info.
 *
 * Use "commands" to see the list of available commands.
 * @note Do not add the `/` prefix to your command
 * @ingroup ShellCommands
 * @param slice slice to run
 * @param command multi line slash command string to run
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_run_lua_command(CredoSlice_t* slice, const char* command);

/**
 * @brief Shell
 * @ingroup ShellSpawn
 *
 * @note Prefer using @ref cr_shell_spawn_enhanced unless using custom shell readline / logger hooks
 *
 * @param[in] slices slices to spawn shell. Use NULL to use current slices
 * @param[in] slice_count how many slices to spawn shell. Pass 0 for current slices
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_spawn(CredoSlice_t* slices[], int slice_count);

/**
 * @brief Spawn shell with battery included tools
 *
 * - Tab completion
 * - Line editing
 * - Command History
 *
 * @note Uses the currently selected slices for spawning the shell.

 * @ingroup ShellSpawn
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_spawn_enhanced(void);

/**
 * @brief Run a shell socket server
 *
 * Using Unix domain sockets, this will spawn a server that can be accessed via the crcli executable.
 * This is mainly for debugging a daemon process version of the sdk-- especially before the shell is integrated into
 * your own shell tool. It currently has no locking protection, so it should NOT be used in production environments.
 *
 * Sockets are stored in the /tmp/credo-*.sock.
 * @note Requires that libcrcli is linked.
 *
 * @ingroup ShellSpawn
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_spawn_server(void);

/**
 * @brief set shell slices
 * @ingroup ShellConfig
 * @param[in] slices slices to set
 * @param[in] slice_count how many slices
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_set_slices(CredoSlice_t* slices[], int slice_count);

/**
 * @brief Set the shell logger callback
 *
 * Separate optional logger for when running a shell command or a slice operation is being performed from a shell.
 * Otherwise it will default to normal sdk logger.
 * @ingroup ShellIntegration
 * @param shell_log_cb shell log call back function
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_set_logger(CredoLog_t shell_log_cb);

#ifdef __cplusplus
}
#endif

#endif  // CREDO_SHELL_H

// #include "credo/slice.h"
#ifndef CREDO_SLICE_H
#define CREDO_SLICE_H

// #include "credo/types.h"


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

// #include "credo/tcm.h"
#ifndef CREDO_TCM_H
#define CREDO_TCM_H

// #include "credo/types.h"


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

// #include "credo/types.h"


#endif
