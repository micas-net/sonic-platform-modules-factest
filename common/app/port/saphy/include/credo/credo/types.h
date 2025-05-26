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
