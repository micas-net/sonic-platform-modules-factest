#include "cust_plp_common.h"
#include "crs.h"
#include "credo.h"

#ifdef EXTPHY_BINARY
#define PLP_CREDO_CFG_FILE    "cust_plp_credo_test.cfg"
#else
#define PLP_CREDO_CFG_FILE    "/usr/share/sonic/hwsku/cust_plp_credo.cfg"
#endif

#define CR_SLICE_NUM             2
#define CR_SLICE_MAX_PORT_NUM    8
#define CR_HOST_LANE_COUNT       8
#define CR_LANE_COUNT            16

#define MAX_PHY_NUM           64
#define SLOT_MAX_MDIO_NUM     8
#define MDIO_MAX_PHY_NUM      3
#define CUST_FW_RETRY_TIMES   3

#define FALSE false
#define TRUE true

#define CR_LINE_SIDE        0
#define CR_HOST_SIDE        1
#define CR_DISABLE          0
#define CR_ENABLE           1
#define CR_MEDIUM_COPPER    0
#define CR_MEDIUM_OPTICAL   1
#define CR_NO_FORCE         0
#define CR_FORCE            1

#define CUST_CR_ERR_LOG   PLP_LOG_ERR
#define CUST_CR_WARN_LOG  PLP_LOG_WARN
#define CUST_CR_INFO_LOG  PLP_LOG_INFO
#define CUST_CR_DBG_LOG   PLP_LOG_DBG
#define CUST_CR_TRACE_LOG PLP_LOG_DBG

typedef struct cust_slice_polarity_info_s {
    uint32_t txpol;
    uint32_t rxpol;
} cust_slice_polarity_info_t;

typedef struct cust_slice_info_s {
    CredoSlice_t *slice;
    uint8_t unit;
    bool inited;
    bool loaded;
    uint8_t card_id;    /* line card id */
    uint8_t mdio_id;
    uint8_t mdio_type;
    uint8_t profile_id;
    uint8_t lane_num;
    uint8_t port_num;
    uint32_t phy_addr;
    uint32_t reg_offset;
    int lanes[CR_SLICE_MAX_PORT_NUM];     /* bcm physical ports */
    int ports[CR_SLICE_MAX_PORT_NUM];     /* bcm logicl ports */
    int ports_lane_num[CR_SLICE_MAX_PORT_NUM];     /* bcm logicl ports */
    int speeds[CR_SLICE_MAX_PORT_NUM];
} cust_slice_info_t;

typedef struct cust_port_hdl_info_s {
    cust_slice_info_t *slice_info;
    CredoSlice_t *slice;
    CredoPortConfig_t port_config;
    int profile_id;
    int port_id;
    int speed;
    int split_num;
} cust_port_hdl_info_t;

static int cust_credo_platform_init(int unit, bool warm_boot);
static int cust_credo_platform_init_pthread_join(int unit);
static bool cust_credo_port_support(int unit, int port, int phy_lane0);
static int cust_credo_create_port(int unit, int port, cust_plp_port_resource_t *port_resource);
static int cust_credo_remove_port(int unit, int port);
static int cust_credo_speed_set(int unit, int port, int speed);
static int cust_credo_speed_get(int unit, int port, int *speed);
static int cust_credo_fec_set(int unit, int port, cust_plp_fec_e fec_type);

static int cust_cr_config_init(int unit);
static int cust_cr_init_slot(int unit, int slot_id);
static void *cust_cr_init_bus_thd(void *arg);
static int cust_cr_bus_init(int bus_id);
static int cust_cr_mdio_read(void* slice_context, unsigned reg_addr, unsigned* val);
static int cust_cr_mdio_write(void* slice_context, unsigned reg_addr, unsigned val);
static int cust_cr_reset_fw(CredoSlice_t *slices);
static int cust_cr_set_port_admin(int unit, int port, bool enable);
static int cust_cr_get_port_conf_hdl(int unit, int port, cust_port_hdl_info_t *cr_port_hdl);
static int cust_cr_get_port_conf_hdl_by_lane(int unit, int phy_lane0, cust_port_hdl_info_t *cr_port_hdl);
static int cust_cr_reconf_port(CredoSlice_t* slice, int port_id, CredoPortConfig_t *port_config);
static int cust_cr_get_slice_info(int unit, int port, cust_slice_info_t **slice_info);
static int cust_cr_get_slice_info_by_lane(int unit, int phy_lane0, cust_slice_info_t **slice_info);
static int cust_cr_get_profile_info(int profile_id, int speed, int split_num, int port_id, CredoPortConfig_t *conf);
static int cust_plp_fec_to_credo_fec(int speed, cust_plp_fec_e fec_type, CredoFecType_t *real_fec);
static int cust_cr_temperature_get(int unit, int port, double *temp);
static int cust_cr_highest_temperature_get(double *temp);
static int cust_cr_set_port_tx_taps(int unit, int port, int if_side, int lane, cust_plp_tx_fir_t tx_fir);

static int cust_cr_port_prbs_set(int unit, int port, int poly, int inv, int if_side, int lane);
static int cust_cr_port_prbs_get(int unit, int port, int if_side, cust_plp_prbs_status_t *prbs_info);
static int cust_cr_port_prbs_clear(int unit, int port, int if_side);
static int cust_cr_port_prbs_ber_get(int unit, int port, int if_side, int time_v, cust_plp_prbs_status_t *prbs_info);

static unsigned int g_fw_ver, g_fw_crc;
static int g_cfg_phy_num = 0;
static CredoDevice_t** g_cr_device = NULL;
static cust_slice_info_t* g_cr_slice_info = NULL;
static int g_cr_slice_info_len = 0;

/* order: m_nChipId m_nSliceId  m_nSysId */
static SliceContext_t* g_cr_slice_ctxs = NULL;
static cust_slice_polarity_info_t* g_slice_host_polarity_info = NULL;
static cust_slice_polarity_info_t* g_slice_line_polarity_info = NULL;

static int g_slot_num = 0;
char g_firmware_binary_path[PLP_CFG_FILE_STR_MAX];

static int cust_credo_platform_init(int unit, bool warm_boot)
{
    int slot_id;

    if (PLP_STATUS_SUCCESS != cust_cr_config_init(unit)) {
        CUST_CR_ERR_LOG("cust_cr_config_init fail");
        return PLP_STATUS_NOT_SUPPORTED;
    }

    for (slot_id = 0; slot_id < g_slot_num; slot_id++) {
        if (cust_cr_init_slot(unit, slot_id) != CR_SUCCESS) {
            return PLP_STATUS_FAILURE;
        }
    }
    
    return PLP_STATUS_SUCCESS;
}

static bool cust_credo_port_support(int unit, int port, int phy_lane0)
{
    int err;
    cust_slice_info_t *slice_info;

    if (g_cr_slice_info_len == 0) {
        return false;
    }

    if (port != -1) {
        err = cust_cr_get_slice_info(unit, port, &slice_info);
        if (err) {
            return false;
        }
    }

    if (phy_lane0 != -1) {
        err = cust_cr_get_slice_info_by_lane(unit, phy_lane0, &slice_info);
        if (err) {
            return false;
        }
    }

    return true;
}

static int cust_credo_create_port(int unit, int port, cust_plp_port_resource_t *port_resource)
{
    int err;
    int split_num;
    int port_id;
    int speed, phy_lane0, lanes;
    CredoPortConfig_t port_config;
    cust_slice_info_t *slice_info;

    speed = port_resource->speed;
    phy_lane0 = port_resource->phy_lane0;
    lanes = port_resource->host_lanes;

    err = cust_cr_get_slice_info_by_lane(unit, phy_lane0, &slice_info);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_slice_info_by_lane fail");
        return CR_FAIL;
    }

    split_num = slice_info->lane_num / lanes;

    for (port_id=0; port_id<slice_info->lane_num; port_id++) {
        if (slice_info->lanes[port_id] == phy_lane0) {
            break;
        }
    }

    err = cust_cr_get_profile_info(slice_info->profile_id, speed, split_num, port_id, &port_config);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_profile_info fail");
        return CR_FAIL;
    }

    port_config.port_id = port_id;
    err = cr_port_configure(slice_info->slice, &port_config, CR_FORCE);
    if (err) {
        CUST_CR_ERR_LOG("cr_port_configure fail, err:%d", err);
        return CR_FAIL;
    }

    slice_info->ports[port_id] = port;
    slice_info->speeds[port_id] = speed;
    slice_info->ports_lane_num[port_id] = lanes;
    slice_info->port_num++;

    return PLP_STATUS_SUCCESS;
}

static int cust_credo_remove_port(int unit, int port)
{
    int err, port_id;
    cust_port_hdl_info_t cr_port_hdl;
    cust_slice_info_t *slice_info;

    err = cust_cr_get_port_conf_hdl(unit, port, &cr_port_hdl);
    if (err) {
        CUST_CR_ERR_LOG("cust_credo_remove_port destroy port fail, err:%d", err);
        return CR_FAIL;
    }

    err = cr_port_destroy(cr_port_hdl.slice, cr_port_hdl.port_id);
    if (err) {
        CUST_CR_ERR_LOG("cust_credo_remove_port destroy port fail, err:%d", err);
        return CR_FAIL;
    }

    slice_info = cr_port_hdl.slice_info;

    for (port_id = 0; port_id < CR_SLICE_MAX_PORT_NUM; port_id++) {
        if (slice_info->ports[port_id] == port) {
            slice_info->ports[port_id] = -1;
            slice_info->speeds[port_id] = -1;
            slice_info->ports_lane_num[port_id] = -1;
            slice_info->port_num--;
        }
    }
    
    return true;
}

static int cust_credo_speed_get(int unit, int port, int *speed)
{
    return PLP_STATUS_SUCCESS;
}

static int cust_credo_speed_set(int unit, int port, int speed)
{
    int err, port_id;
    cust_port_hdl_info_t cr_port_hdl;
    cust_slice_info_t *slice_info;

    err = cust_cr_get_port_conf_hdl(unit, port, &cr_port_hdl);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_port_conf_hdl fail, err:%d", err);
        return CR_FAIL;
    }

    slice_info = cr_port_hdl.slice_info;

    if (speed == (cr_port_hdl.port_config).speed) {
        CUST_CR_ERR_LOG("cust_credo_speed_set ignore same speed.");
        return PLP_STATUS_SUCCESS;
    }

    (cr_port_hdl.port_config).speed = speed;

    err = cust_cr_reconf_port(cr_port_hdl.slice, cr_port_hdl.port_id, &cr_port_hdl.port_config);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_reconf_port fail, err:%d", err);
        return CR_FAIL;
    }

    for (port_id = 0; port_id < CR_SLICE_MAX_PORT_NUM; port_id++) {
        if (slice_info->ports[port_id] == port) {
            slice_info->speeds[port_id] = speed;
        }
    }

    return true;
}

/* TODO */
static int cust_credo_fec_set(int unit, int port, cust_plp_fec_e fec_type)
{
    // int err, split_num, port_id, speed;
    // CredoFecType_t credo_fec;
    // cust_port_hdl_info_t cr_port_hdl;
    // cust_slice_info_t *slice_info;

    // err = cust_cr_get_port_conf_hdl(unit, port, &cr_port_hdl);
    // if (err) {
    //     CUST_CR_ERR_LOG("cust_cr_get_port_conf_hdl fail, err:%d", err);
    //     return CR_FAIL;
    // }
    // slice_info = cr_port_hdl.slice_info;

    // err = cust_credo_speed_get(unit, port, &speed);
    // if (err) {
    //     CUST_CR_ERR_LOG("cust_credo_speed_get fail, err:%d", err);
    //     return CR_FAIL;
    // }

    // err = cust_plp_fec_to_credo_fec(speed, fec_type, &credo_fec);
    // if (err != PLP_STATUS_SUCCESS) {
    //     CUST_CR_ERR_LOG("cust_plp_fec_to_credo_fec fail, err:%d", err);
    //     return CR_FAIL;
    // }

    // err = cust_cr_reconf_port(cr_port_hdl.slice, cr_port_hdl.port_id, &cr_port_hdl.port_config);
    // if (err) {
    //     CUST_CR_ERR_LOG("cust_cr_reconf_port fail, err:%d", err);
    //     return CR_FAIL;
    // }

    return true;
}

static int cust_credo_shell_cmd_run(int unit, int port, char *cmdline)
{
    int i, len, err;
    uint32_t speed;
    cust_slice_info_t *slice_info;

    len = strlen(cmdline); 
    for (i = 0; i < len; i++) {
        if (cmdline[i] == '/') {
            cmdline[i] = ' ';
        }
    }

    err = cust_cr_get_slice_info(unit, port, &slice_info);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_slice_info fail");
        return PLP_STATUS_FAILURE;
    }

    cr_shell_run_command(slice_info->slice, cmdline);
    return PLP_STATUS_SUCCESS;
}

cust_plp_apis_t cust_extphy_credo_apis = {
    .cust_plp_platform_init              = cust_credo_platform_init,
    .cust_plp_port_support               = cust_credo_port_support,
    .cust_plp_create_port                = cust_credo_create_port,
    .cust_plp_remove_port                = cust_credo_remove_port,
    .cust_plp_port_speed_set             = cust_credo_speed_set,
    .cust_plp_port_speed_get             = cust_credo_speed_get,
    .cust_plp_port_fec_set               = cust_credo_fec_set,
    .cust_plp_shell_cmd_run              = cust_credo_shell_cmd_run,
    .cust_plp_temperature_get            = cust_cr_temperature_get,
    .cust_plp_highest_temperature_get    = cust_cr_highest_temperature_get,
    .cust_plp_tx_fir_set                 = cust_cr_set_port_tx_taps,
    .cust_plp_port_prbs_set              = cust_cr_port_prbs_set,
    .cust_plp_port_prbs_get              = cust_cr_port_prbs_get,
    .cust_plp_port_prbs_clear            = cust_cr_port_prbs_clear,
    .cust_plp_port_prbs_ber_get          = cust_cr_port_prbs_ber_get
};

static int cust_plp_fec_to_credo_fec(int speed, cust_plp_fec_e fec_type, CredoFecType_t *real_fec)
{
    // typedef enum {
    //     CR_FEC_NONE,  //!< NO FEC
    //     CR_FEC_FIRE_CODE,
    //     CR_FEC_RS_528,
    //     CR_FEC_RS_544
    // } CredoFecType_t;

    return PLP_STATUS_SUCCESS;
}

static int cust_cr_get_slice_info_by_lane(int unit, int phy_lane0, cust_slice_info_t **slice_info)
{
    int found, slice_id, port_id;
    found = false;
    for (slice_id = 0; slice_id < g_cr_slice_info_len; slice_id++) {
        for (port_id = 0; port_id < CR_SLICE_MAX_PORT_NUM; port_id++) {
            if (phy_lane0 == g_cr_slice_info[slice_id].lanes[port_id]) {
                *slice_info = &g_cr_slice_info[slice_id];
                found = true;
                break;
            }
        }
    }
    if (found == true) {
        return CR_SUCCESS;
    }
    return CR_FAIL; 
}

static int cust_cr_get_slice_info(int unit, int port, cust_slice_info_t **slice_info)
{
    int found, slice_id, port_id;
    found = false;
    for (slice_id = 0; slice_id < g_cr_slice_info_len; slice_id++) {
        for (port_id = 0; port_id < CR_SLICE_MAX_PORT_NUM; port_id++) {
            if (port == g_cr_slice_info[slice_id].ports[port_id]) {
                *slice_info = &g_cr_slice_info[slice_id];
                found = true;
                break;
            }
        }
    }
    if (found == true) {
        return CR_SUCCESS;
    }
    return CR_FAIL; 
}

static int cust_cr_mdio_read(void* slice_context, unsigned reg_addr, unsigned* val)
{
    int rv;
    unsigned int slice_id, phy_addr;
    SliceContext_t *context;
    mdio_read_func cr_phy_read = NULL;

    context = (SliceContext_t *)slice_context;
    slice_id = context->m_nChipId * CR_SLICE_NUM + context->m_nSliceId;
    /* no verification for read/write faster */
    phy_addr = g_cr_slice_info[slice_id].phy_addr;
    reg_addr = g_cr_slice_info[slice_id].reg_offset | reg_addr;

    rv = plp_get_mdio_read_func(g_cr_slice_info[slice_id].mdio_type, &cr_phy_read);
    if (rv) {
        CUST_CR_ERR_LOG("cr read func not reg!");
        return CR_FAIL;
    }

    rv = cr_phy_read(NULL, phy_addr, reg_addr, val);
    if (rv) {
        CUST_CR_ERR_LOG("read slice_id:%d reg:0x%x fail, rv:%d", slice_id, reg_addr, rv);
    }
    return rv;
}

static int cust_cr_mdio_write(void* slice_context, unsigned reg_addr, unsigned val)
{
    int rv;
    unsigned int slice_id, phy_addr;
    mdio_write_func cr_phy_write = NULL;
    SliceContext_t *context;

    context = (SliceContext_t *)slice_context;
    slice_id = context->m_nChipId * CR_SLICE_NUM + context->m_nSliceId;
    /* no verification for read/write faster */
    phy_addr = g_cr_slice_info[slice_id].phy_addr;
    reg_addr = g_cr_slice_info[slice_id].reg_offset | reg_addr;

    rv = plp_get_mdio_write_func(g_cr_slice_info[slice_id].mdio_type, &cr_phy_write);
    if (rv) {
        CUST_CR_ERR_LOG("cr write func not reg!");
        return CR_FAIL;
    }

    rv = cr_phy_write(NULL, phy_addr, reg_addr, val);
    if (rv) {
        CUST_CR_ERR_LOG("write slice_id:%d reg:0x%x val:0x%x fail, rv:%d", slice_id, reg_addr, val, rv);
    }
    return rv;
}

static int cust_cr_set_port_admin(int unit, int port, bool enable)
{
    int err, lane;
    cust_port_hdl_info_t cr_port_hdl;
    
    err = cust_cr_get_port_conf_hdl(unit, port, &cr_port_hdl);
    if (err) {
        goto cust_cr_set_port_admin_err;
    }

    for (lane = 0; lane < CR_LANE_COUNT; lane++) {
        if (!((lane >= cr_port_hdl.port_config.host_start_lane && lane < (cr_port_hdl.port_config.host_start_lane + cr_port_hdl.port_config.host_no_of_lanes)) ||
                (lane >= cr_port_hdl.port_config.line_start_lane && lane < (cr_port_hdl.port_config.line_start_lane + cr_port_hdl.port_config.line_no_of_lanes)))) {
            continue;
        }

        if (enable) {
            err = cr_lane_rx_no_disable(cr_port_hdl.slice, lane);
            if (err) {
                CUST_CR_ERR_LOG("cr set lane rx enable fail, lane:%d err:%d", lane, err);
                goto cust_cr_set_port_admin_err;
            }

            err = cr_lane_tx_no_disable(cr_port_hdl.slice, lane);
            if (err) {
                CUST_CR_ERR_LOG("cr set lane tx enable fail, lane:%d err:%d", lane, err);
                goto cust_cr_set_port_admin_err;
            }
        } else {
            err = cr_lane_rx_disable(cr_port_hdl.slice, lane);
            if (err) {
                CUST_CR_ERR_LOG("cr set lane rx disable fail, lane:%d err:%d", lane, err);
                goto cust_cr_set_port_admin_err;
            }

            err = cr_lane_tx_disable(cr_port_hdl.slice, lane);
            if (err) {
                CUST_CR_ERR_LOG("cr set lane tx disable fail, lane:%d err:%d", lane, err);
                goto cust_cr_set_port_admin_err;
            }
        }
    }
    
    CUST_CR_INFO_LOG("cust_cr_set_port_admin success, unit:%d port:%d enable:%d", unit, port, enable);
    return CR_SUCCESS;

cust_cr_set_port_admin_err:
    CUST_CR_ERR_LOG("cust_cr_set_port_admin fail, unit:%d port:%d enable:%d", unit, port, enable);
    return CR_FAIL;
}

static int cust_cr_get_port_conf_hdl(int unit, int port, cust_port_hdl_info_t *cr_port_hdl)
{
    int port_id, err;
    cust_slice_info_t *slice_info;

    if (!cr_port_hdl) {
        CUST_CR_ERR_LOG("invalid param!");
        return CR_FAIL;
    }

    err = cust_cr_get_slice_info(unit, port, &slice_info);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_slice_info fail, err:%d", err);
        return CR_FAIL;
    }

    cr_port_hdl->slice = slice_info->slice;
    cr_port_hdl->slice_info = slice_info;
    cr_port_hdl->profile_id = slice_info->profile_id;

    for (port_id=0; port_id<CR_SLICE_MAX_PORT_NUM; port_id++) {
        if (port == slice_info->ports[port_id]) {
            cr_port_hdl->port_id = port_id;
            break;
        }
    }
    
    cr_port_hdl->speed = slice_info->speeds[port_id];
    cr_port_hdl->split_num = slice_info->lane_num / slice_info->ports_lane_num[port_id];

    err = cr_port_query(cr_port_hdl->slice, cr_port_hdl->port_id, &cr_port_hdl->port_config);
    if (err) {
        CUST_CR_ERR_LOG("cr query port fail, port_id:%d err:%d", cr_port_hdl->port_id, err);
        return CR_FAIL;
    }

    return CR_SUCCESS;
}

static int cust_cr_get_port_conf_hdl_by_lane(int unit, int port, cust_port_hdl_info_t *cr_port_hdl)
{
    int port_id, err;
    cust_slice_info_t *slice_info;

    if (!cr_port_hdl) {
        CUST_CR_ERR_LOG("invalid param!");
        return CR_FAIL;
    }

    err = cust_cr_get_slice_info(unit, port, &slice_info);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_slice_info fail, err:%d", err);
        return CR_FAIL;
    }

    cr_port_hdl->slice = slice_info->slice;
    cr_port_hdl->slice_info = slice_info;
    cr_port_hdl->profile_id = slice_info->profile_id;

    for (port_id=0; port_id<CR_SLICE_MAX_PORT_NUM; port_id++) {
        if (port == slice_info->ports[port_id]) {
            cr_port_hdl->port_id = port_id;
            break;
        }
    }
    
    cr_port_hdl->speed = slice_info->speeds[port_id];
    cr_port_hdl->split_num = slice_info->lane_num / slice_info->ports_lane_num[port_id];

    err = cr_port_query(cr_port_hdl->slice, cr_port_hdl->port_id, &cr_port_hdl->port_config);
    if (err) {
        CUST_CR_ERR_LOG("cr query port fail, port_id:%d err:%d", cr_port_hdl->port_id, err);
        return CR_FAIL;
    }

    return CR_SUCCESS;
}

static int cust_cr_get_profile_info(int profile_id, int speed, int split_num, int port_id, CredoPortConfig_t *conf)
{
    int rv, val;
    char config_prefix[PLP_CFG_FILE_STR_MAX];

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:connection_mode", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->connection_mode = val;
    }

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:port_mode", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->port_mode = val;
    }

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:flags", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->flags = val;
    }

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:host_fec_type", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->host_fec_type = val;
    }

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:line_fec_type", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->line_fec_type = val;
    }

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:line_no_of_lanes", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->line_no_of_lanes = val;
    }

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:host_no_of_lanes", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->host_no_of_lanes = val;
    }

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:line_start_lane", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->line_start_lane = val;
    }

    snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_speed_%d_split_%d.%d:host_start_lane", profile_id, speed, split_num, port_id);
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("%s get fail.", config_prefix);
        return CR_FAIL; 
    } else {
        conf->host_start_lane = val;
    }

    conf->speed = speed;
    conf->port_id = port_id;

    return CR_SUCCESS;
}

static int cust_cr_reset_fw(CredoSlice_t *slices)
{
    int i, soft_ntime;
    int err;
    unsigned int fw_ver, fw_crc;

    soft_ntime = CUST_FW_RETRY_TIMES;

    for (i = 0; i < soft_ntime; i++) {
        err = cr_firmware_hash(slices, &fw_ver);
        if (err) {
            CUST_CR_ERR_LOG("cr firmware version get fail, err:%d", err);
            cr_firmware_load(slices, g_firmware_binary_path, 1);
            continue;
        }
        err = cr_firmware_crc(slices, &fw_crc);
        if (err) {
            CUST_CR_ERR_LOG("cr firmware crc get fail, err:%d", err);
            cr_firmware_load(slices, g_firmware_binary_path, 1);
            continue;
        }
        if (fw_ver == g_fw_ver && fw_crc == g_fw_crc) {
            return CR_SUCCESS;
        }
        CUST_CR_ERR_LOG("cr firmware crc get fail, err:%d fw_ver 0x%x fw_crc 0x%x", err, fw_ver, fw_crc);
        cr_firmware_load(slices, g_firmware_binary_path, 1);
    }

    return CR_FAIL;
}

static void cust_cr_log(void* slice_context, void* user_data, CredoLogLevel_t level, const char* scope, const char* message)
{
    switch(level) {
    case CR_LOG_ERROR:
        CUST_CR_ERR_LOG("scope: %s, message: %s", scope, message);
        break;
    case CR_LOG_WARN:
        CUST_CR_WARN_LOG("scope: %s, message: %s", scope, message);
        break;
    case CR_LOG_INFO:
        CUST_CR_INFO_LOG("scope: %s, message: %s", scope, message);
        break;
    case CR_LOG_DEBUG:
        CUST_CR_DBG_LOG("scope: %s, message: %s", scope, message);
        break;
    case CR_LOG_TRACE:
        CUST_CR_TRACE_LOG("scope: %s, message: %s", scope, message);
        break;
    }
}

static int cust_cr_bus_init(int bus_id)
{
    int slice_id, lane, mdio_slice_num, pol;
    int port_id;
    CredoErrorCodes_t err;
    CredoSliceConfig_t slice_cfg;
    CredoSlice_t *slices[g_cr_slice_info_len];
    CredoSdk_t* sdk;
    CredoSdkConfig_t sdk_config;
    CredoSlice_t *slices_broadcast[g_cr_slice_info_len];
    CredoPortConfig_t port_config;

    CUST_CR_INFO_LOG("cr bus:%d init start...", bus_id);

    /* ------soft data prepare------- */
    err = CR_SUCCESS;
    memset(&sdk_config, 0, sizeof(CredoSdkConfig_t));
    sdk_config.log = cust_cr_log;
    sdk_config.max_log_level = CR_LOG_TRACE;
    sdk_config.read_register = cust_cr_mdio_read;
    sdk_config.write_register = cust_cr_mdio_write;
    err = cr_sdk_create(&sdk_config, &sdk);
    if (err) {
        CUST_CR_ERR_LOG("cr sdk create fail, err:%d", err);
        return CR_FAIL;
    }

    for (slice_id = 0; slice_id < g_cr_slice_info_len; slice_id++) {
        if (g_cr_slice_info[slice_id].mdio_id != bus_id) {
            continue;
        }
        if (!g_cr_device[slice_id / 2]) {
            err = cr_device_create(sdk, CREDO_BLACKHAWK_800, &g_cr_device[slice_id / 2]);
            if (err) {
                CUST_CR_ERR_LOG("cr slice_id:%d device create fail, err:%d", slice_id, err);
                return CR_FAIL;
            }
        }

        err = cr_device_get_slice(g_cr_device[slice_id / 2], (slice_id % 2), &slices[slice_id]);
        if (err) {
            CUST_CR_ERR_LOG("cr bus_id:%d slice_id:%d get slice fail, err:%d", bus_id, slice_id, err);
            return CR_FAIL;
        }

        slice_cfg.init_type = CR_INIT_NO_FIRMWARE;
        slice_cfg.slice_id = slice_id % 2;
        slice_cfg.slice_context = (void*)&g_cr_slice_ctxs[slice_id];
        err = cr_slice_init(slices[slice_id], &slice_cfg);
        if (err) {
            CUST_CR_ERR_LOG("cr bus_id:%d slice_id:%d init slice fail, err:%d", bus_id, slice_id, err);
            return CR_FAIL;
        }
        g_cr_slice_info[slice_id].slice = slices[slice_id];
    }

    CUST_CR_INFO_LOG("cr bus:%d fw load start...", bus_id);
    /* ------fw broadcast------- */
    mdio_slice_num = 0;
    for (slice_id = 0; slice_id < g_cr_slice_info_len; slice_id++) {
        if (g_cr_slice_info[slice_id].mdio_id != bus_id) {
            continue;
        }

        /* before fw load, reset init info */
        g_cr_slice_info[slice_id].loaded = false;
        g_cr_slice_info[slice_id].inited = false;

        slices_broadcast[mdio_slice_num] = slices[slice_id];
        mdio_slice_num += 1;
    }
    
    cr_sdk_set_broadcast_write(sdk, cust_cr_mdio_write);
    err = cr_firmware_load_broadcast(slices_broadcast, mdio_slice_num, g_firmware_binary_path, 2, 1);
    if (err) {
        CUST_CR_ERR_LOG("cr firmware load fail r, mdio_id:%d mdio_slice_num:%d err:%d", bus_id, mdio_slice_num, err);
    } else {
        CUST_CR_INFO_LOG("cr firmware load success r, mdio_id:%d mdio_slice_num:%d", bus_id, mdio_slice_num);
    }

    CUST_CR_INFO_LOG("cr bus:%d fw check start...", bus_id);
    /* ------fw check------- */
    for (slice_id = 0; slice_id < g_cr_slice_info_len; slice_id++) {
        if (g_cr_slice_info[slice_id].mdio_id != bus_id) {
            continue;
        }

        if (cust_cr_reset_fw(slices[slice_id]) == CR_SUCCESS) {
            CUST_CR_INFO_LOG("cr firmware r check success, slice_id:%d", slice_id);
            g_cr_slice_info[slice_id].loaded = true;
        } else {
            CUST_CR_ERR_LOG("cr firmware r check fail, slice_id:%d", slice_id);
        }
    }

    /* ------configure port------- */
    for (slice_id = 0; slice_id < g_cr_slice_info_len; slice_id++) {
        if (g_cr_slice_info[slice_id].mdio_id != bus_id) {
            continue;
        }

        if (!g_cr_slice_info[slice_id].loaded) {
            CUST_CR_ERR_LOG("cr firmware not loaded, fail to configure port, slice_id:%d", slice_id);
            continue;
        }

        /* disable all lanes before confige lane/port */
        for (lane = 0; lane < CR_LANE_COUNT; lane++) {
            err = cr_lane_rx_disable(slices[slice_id], lane);
            if (err) {
                CUST_CR_ERR_LOG("cr set rx disable fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                return CR_FAIL;
            }

            err = cr_lane_tx_disable(slices[slice_id], lane);
            if (err) {
                CUST_CR_ERR_LOG("cr set tx disable fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                return CR_FAIL;
            }
        }

        for (lane = 0; lane < CR_LANE_COUNT; lane++) {
            err= cr_serdes_set_rx_coupling(slices[slice_id], lane, CR_COUPLING_AC);
            if (err) {
                CUST_CR_ERR_LOG("cr set rx coupling AC fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                return CR_FAIL;
            }

            if (lane < CR_HOST_LANE_COUNT) {
                pol = (g_slice_host_polarity_info[slice_id].txpol & (1 << (lane))) ? 1 : 0;
                err = cr_serdes_set_tx_polarity(slices[slice_id], lane, pol);
                if (err) {
                    CUST_CR_ERR_LOG("cr set host tx polarity fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                    return CR_FAIL;
                }

                pol = (g_slice_host_polarity_info[slice_id].rxpol & (1 << (lane))) ? 1 : 0;
                err = cr_serdes_set_rx_polarity(slices[slice_id], lane, pol);
                if (err) {
                    CUST_CR_ERR_LOG("cr set host rx polarity fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                    return CR_FAIL;
                }

                err = cr_serdes_set_tx_gray_code(slices[slice_id], lane, CR_ENABLE);
                if (err) {
                    CUST_CR_ERR_LOG("cr open host tx gray code fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                    return CR_FAIL;
                }

                err = cr_serdes_set_rx_gray_code(slices[slice_id], lane, CR_ENABLE);
                if (err) {
                    CUST_CR_ERR_LOG("cr open host gray rx code fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                    return CR_FAIL;
                }
            } else {
                pol = (g_slice_line_polarity_info[slice_id].txpol & (1 << (lane - CR_HOST_LANE_COUNT))) ? 1 : 0;
                err = cr_serdes_set_tx_polarity(slices[slice_id], lane, pol);
                if (err) {
                    CUST_CR_ERR_LOG("cr set line tx polarity fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                    return CR_FAIL;
                }

                pol = (g_slice_line_polarity_info[slice_id].rxpol & (1 << (lane - CR_HOST_LANE_COUNT))) ? 1 : 0;
                err = cr_serdes_set_rx_polarity(slices[slice_id], lane, pol);
                if (err) {
                    CUST_CR_ERR_LOG("cr set line rx polarity fail, slice_id:%d lane:%d err:%d", slice_id, lane, err);
                    return CR_FAIL;
                }
            }
        }

        for (port_id = 0; port_id < CR_SLICE_MAX_PORT_NUM; port_id++) {
            if (g_cr_slice_info[slice_id].ports[port_id] == -1) {
                continue;
            }
            int profile_id, speed, split_num;
        
            profile_id = g_cr_slice_info[slice_id].profile_id;
            speed = g_cr_slice_info[slice_id].speeds[port_id];
            split_num = g_cr_slice_info[slice_id].lane_num / g_cr_slice_info[slice_id].ports_lane_num[port_id];

            err = cust_cr_get_profile_info(profile_id, speed, split_num, port_id, &port_config);
            if (err) {
                CUST_CR_ERR_LOG("cust_cr_get_profile_info fail");
                return CR_FAIL;
            }

            port_config.port_id = port_id;
            err = cr_port_configure(slices[slice_id], &port_config, CR_FORCE);
            if (err) {
                CUST_CR_ERR_LOG("cr config port fail, slice_id:%d port_id:%d err:%d", slice_id, port_id, err);
                return CR_FAIL;
            }
        }

        /* enable all lanes belong to this port */
        for (port_id = 0; port_id < CR_SLICE_MAX_PORT_NUM; port_id++) {
            if (g_cr_slice_info[slice_id].ports[port_id] == -1) {
                continue;
            }
            err = cust_cr_set_port_admin(0, g_cr_slice_info[slice_id].ports[port_id], CR_ENABLE);
            if (err) {
                CUST_CR_ERR_LOG("cr config port admin fail, err:%d", err);
                return CR_FAIL;
            }
        }

        g_cr_slice_info[slice_id].inited = true;

        /* for custom init soft info check */
        // for (port = 0; port < g_cr_slice_info[slice_id].port_num; port++) {
        //     err = cust_cr_get_port_conf_hdl(0, g_cr_slice_info[slice_id].ports[port], &cr_port_hdl);
        //     if (!err) {
        //         CUST_CR_INFO_LOG("<T-MAP> cr port mapping: uport:%d slice_id:%d port_id:%d", 
        //             g_cr_slice_info[slice_id].ports[port], cr_port_hdl.slice_id, cr_port_hdl.port_id);
        //     }
        // }

    }

#ifdef EXTPHY_BINARY
    err = cr_shell_set_slices(slices, g_cr_slice_info_len);
    if (err) {
        CUST_CR_ERR_LOG("cr_shell_set_slices fail, err:%d", err);
    }
#endif

    CUST_CR_INFO_LOG("cr bus:%d init success", bus_id);
    return CR_SUCCESS;
}

static void *cust_cr_init_bus_thd(void *arg)
{
    int bus_id, err;
    bus_id = *(int *)arg;
    free(arg);
    err =  cust_cr_bus_init(bus_id);
    if (err) {
        CUST_CR_ERR_LOG("cr bus:%d init fail", bus_id);
    }
    return NULL;
}

static int cust_cr_init_slot(int unit, int slot_id)
{
    int rv;
    int slice_id, mdio_num, i, exist, err;
    int mdio_ids[SLOT_MAX_MDIO_NUM];
    int *args[SLOT_MAX_MDIO_NUM];
    pthread_t cr_bus_init_thread_id[SLOT_MAX_MDIO_NUM];
    
    memset(mdio_ids, -1, sizeof(mdio_ids));
    mdio_num = 0;
    for (slice_id = 0; slice_id < g_cr_slice_info_len; slice_id++) {
        if (g_cr_slice_info[slice_id].card_id != slot_id) {
            continue;
        }

        exist = false;
        for (i = 0; i < SLOT_MAX_MDIO_NUM; i++) {
            if (mdio_ids[i] == g_cr_slice_info[slice_id].mdio_id) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            mdio_ids[mdio_num] = g_cr_slice_info[slice_id].mdio_id;
            mdio_num++;
        }
    }

    for (i = 0; i < mdio_num; i++) {
        args[i] = (int *)malloc(sizeof(int));
        if (!args[i]) {
            CUST_CR_ERR_LOG("mdio:%d malloc args failed", mdio_ids[i]);
            return PLP_STATUS_FAILURE;
        }
        *args[i] = mdio_ids[i];

        CUST_CR_INFO_LOG("cr bus:%d init pthread create start", mdio_ids[i]);
        err = pthread_create(&cr_bus_init_thread_id[i], NULL, cust_cr_init_bus_thd, (void *)args[i]);
        if (err) {
            CUST_CR_ERR_LOG("cr pthread create fail, err:%d", err);
            free(args[i]);
            return PLP_STATUS_FAILURE;
        }
    }

    rv = PLP_STATUS_SUCCESS;
    for (i = 0; i < mdio_num; i++) {
        CUST_CR_INFO_LOG("cr bus:%d init pthread join start", mdio_ids[i]);
        if (!cr_bus_init_thread_id[i]) {
            CUST_CR_ERR_LOG("pthread_join join error, thread_id null");
        }
        err = pthread_join(cr_bus_init_thread_id[i], NULL);
        if (err)  {
            CUST_CR_ERR_LOG("pthread_join join error, err:%d, mdio_id: %d", err, mdio_ids[i]);
            continue;
        }
    }

    CUST_CR_INFO_LOG("cust_cr_init_slot end, slot_id:%d", slot_id);
    return rv;
}

static int cust_cr_config_init(int unit)
{
    int i, j, rv, val;
    int vals[PLP_CFG_FILE_ARR_MAX];
    int ports[CR_SLICE_MAX_PORT_NUM];
    int speeds[CR_SLICE_MAX_PORT_NUM];
    int ports_lane_num[CR_SLICE_MAX_PORT_NUM];
    int phy_addrs[MAX_PHY_NUM];
    char config_prefix[PLP_CFG_FILE_STR_MAX];
    int index;

    if (unit != 0) {
        return PLP_STATUS_SUCCESS;
    }

    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, "plp_slot_num", &g_slot_num, false);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("plp_slot_num get fail.It think no support when cfg get fail.");
        return PLP_STATUS_ITEM_NOT_FOUND;
    }

    rv = plp_get_cfg_info_str(PLP_CREDO_CFG_FILE, "firmware_binary_path", g_firmware_binary_path);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("firmware_binary_path get fail.It think no support when cfg get fail.");
        return PLP_STATUS_ITEM_NOT_FOUND;
    }

    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, "plp_fw_ver", &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("plp_fw_ver get fail.");
        return PLP_STATUS_ITEM_NOT_FOUND;
    } else {
        g_fw_ver = val;
    }

    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, "plp_fw_crc", &val, FALSE);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("plp_fw_crc get fail.");
        return PLP_STATUS_ITEM_NOT_FOUND;
    } else {
        g_fw_crc = val;
    }

    /* phy_addrs */
    memset(phy_addrs, -1, sizeof(phy_addrs));
    rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, "plp_phy_addrs", phy_addrs, true);
    if (rv != PLP_STATUS_SUCCESS) {
        CUST_CR_DBG_LOG("plp_phy_addrs get fail.");
        return PLP_STATUS_ITEM_NOT_FOUND;
    }

    g_cfg_phy_num = 0;
    for (i=0; i<MAX_PHY_NUM; i++) {
        if (phy_addrs[i] == -1) {
            break;
        }
        g_cfg_phy_num++;
    }

    // g_cfg_phy_num = phy_num;

    // g_cr_slice_info_len = g_cfg_phy_num * CR_SLICE_NUM;
    g_cr_slice_info_len = g_cfg_phy_num;
    g_cr_slice_info = (cust_slice_info_t *)malloc(sizeof(cust_slice_info_t) * g_cr_slice_info_len);

    for (i=0; i<g_cfg_phy_num; i++) {
        /* phy_addr */
        g_cr_slice_info[i].phy_addr = (unsigned int)phy_addrs[i];

        /* card_id */
        snprintf(config_prefix, sizeof(config_prefix), "plp_phy_card_id:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }
        g_cr_slice_info[i].card_id = val;

        /* mdio_id */
        snprintf(config_prefix, sizeof(config_prefix), "plp_phy_mdio_id:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }
        g_cr_slice_info[i].mdio_id = val;

        /* mdio_type */
        snprintf(config_prefix, sizeof(config_prefix), "plp_mdio_accs_type:%d", g_cr_slice_info[i].mdio_id);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }
        g_cr_slice_info[i].mdio_type = val;

        /* reg_offset */
        snprintf(config_prefix, sizeof(config_prefix), "plp_phy_reg_offset:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }
        g_cr_slice_info[i].reg_offset = val;

        /* unit */
        snprintf(config_prefix, sizeof(config_prefix), "plp_phy_unit_id:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }else{
            g_cr_slice_info[i].unit = val;
        }

        /* profile_id */
        snprintf(config_prefix, sizeof(config_prefix), "plp_phy_init_profile_id:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }else{
            g_cr_slice_info[i].profile_id = val;
        }

        /* lanes */
        memset(vals, -1, sizeof(vals));
        memset(g_cr_slice_info[i].lanes, -1, sizeof(g_cr_slice_info[i].lanes));
        snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_init_lanes:0x%x", g_cr_slice_info[i].profile_id, phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, vals, true);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }
        g_cr_slice_info[i].lane_num = 0;
        for (j = 0; j < CR_SLICE_MAX_PORT_NUM; j++) {
            if (vals[j] == -1) {
                break;
            }
            g_cr_slice_info[i].lanes[j] = vals[j];
            /* lane_num */
            g_cr_slice_info[i].lane_num++;
        }

        /* lports */
        memset(ports, -1, sizeof(ports));
        snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_init_ports:0x%x", g_cr_slice_info[i].profile_id, phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, ports, true);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }

        /* ports_lane_num */
        memset(ports_lane_num, -1, sizeof(ports_lane_num));
        snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_init_ports_lane_num:0x%x", g_cr_slice_info[i].profile_id, phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, ports_lane_num, true);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }

        /* speeds */
        memset(speeds, -1, sizeof(speeds));
        snprintf(config_prefix, sizeof(config_prefix), "plp_profile_%d_init_speed_mode:0x%x", g_cr_slice_info[i].profile_id, phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, speeds, true);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        }

        memset(g_cr_slice_info[i].ports, -1, sizeof(g_cr_slice_info[i].ports));
        memset(g_cr_slice_info[i].speeds, -1, sizeof(g_cr_slice_info[i].speeds));
        memset(g_cr_slice_info[i].ports_lane_num, -1, sizeof(g_cr_slice_info[i].ports_lane_num));
        for (j = 0; j<CR_SLICE_MAX_PORT_NUM; j++) {
            if (ports[j] == -1) {
                break;
            }
            index = (j>0) ? ports_lane_num[j-1] : j;

            g_cr_slice_info[i].ports[index] = ports[j];
            CUST_CR_DBG_LOG("ports[%d] = %d", index, ports[j]);

            g_cr_slice_info[i].ports_lane_num[index] = ports_lane_num[j];
            CUST_CR_DBG_LOG("ports_lane_num[%d] = %d", index, ports[j]);
            
            g_cr_slice_info[i].speeds[index] = speeds[j];
            CUST_CR_DBG_LOG("speeds[%d] = %d", index, ports[j]);
        }

    }

    g_cr_device = (CredoDevice_t**)calloc(g_cfg_phy_num, sizeof(CredoDevice_t*));
    g_cr_slice_ctxs = (SliceContext_t *)calloc(g_cr_slice_info_len, sizeof(SliceContext_t));

    g_slice_host_polarity_info = (cust_slice_polarity_info_t *)calloc(g_cr_slice_info_len, sizeof(cust_slice_polarity_info_t));
    g_slice_line_polarity_info = (cust_slice_polarity_info_t *)calloc(g_cr_slice_info_len, sizeof(cust_slice_polarity_info_t));

    for (i=0; i<g_cfg_phy_num; i++) {
        snprintf(config_prefix, sizeof(config_prefix), "plp_tx_polarity_flip_host:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        } else {
            g_slice_host_polarity_info[i].txpol = val;
        }

        snprintf(config_prefix, sizeof(config_prefix), "plp_rx_polarity_flip_host:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        } else {
            g_slice_host_polarity_info[i].rxpol = val;
        }

        snprintf(config_prefix, sizeof(config_prefix), "plp_tx_polarity_flip_line:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        } else {
            g_slice_line_polarity_info[i].txpol = val;
        }

        snprintf(config_prefix, sizeof(config_prefix), "plp_rx_polarity_flip_line:0x%x", phy_addrs[i]);
        rv = plp_get_cfg_info(PLP_CREDO_CFG_FILE, config_prefix, &val, false);
        if (rv != PLP_STATUS_SUCCESS) {
            CUST_CR_DBG_LOG("%s get fail.", config_prefix);
            return PLP_STATUS_ITEM_NOT_FOUND;
        } else {
            g_slice_line_polarity_info[i].rxpol = val;
        }
    }

    return PLP_STATUS_SUCCESS;
}

static int cust_cr_reconf_port(CredoSlice_t* slice, int port_id, CredoPortConfig_t *port_config)
{
    int err;

    /* port config chg need del port first */
    err = cr_port_destroy(slice, port_id);
    if (err) {
        CUST_CR_ERR_LOG("cr_reconf destroy port fail, err:%d", err);
        return CR_FAIL;
    }

    err = cr_port_configure(slice, port_config, CR_FORCE);
    if (err) {
        CUST_CR_ERR_LOG("cr_reconf configure port fail, err:%d", err);
        return CR_FAIL;
    }

    CUST_CR_INFO_LOG("cr_reconf port success");
    return CR_SUCCESS;
}

static int cust_cr_temperature_get(int unit, int port, double *temp)
{
    int err;
    cust_slice_info_t *slice_info;

    if (temp == NULL) {
        return CR_FAIL;
    }

    err = cust_cr_get_slice_info(unit, port, &slice_info);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_slice_info fail, err:%d", err);
        return CR_FAIL;
    }

    err = cr_slice_get_temperature(slice_info->slice, temp);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_slice_info fail, err:%d", err);
        return CR_FAIL;
    }

    return CR_SUCCESS;
}

static int cust_cr_highest_temperature_get(double *temp)
{
    int err;
    int slice_id;
    double tmp_temp;
    cust_slice_info_t *slice_info;

    *temp = 0;
    for (slice_id = 0; slice_id < g_cr_slice_info_len; slice_id++) {
        slice_info = &g_cr_slice_info[slice_id];

        err = cr_slice_get_temperature(slice_info->slice, &tmp_temp);
        if (err) {
            CUST_CR_ERR_LOG("cust_cr_get_slice_info fail, err:%d", err);
            return CR_FAIL;
        }

        if (tmp_temp > *temp) {
            *temp = tmp_temp;
        }
    }

    return CR_SUCCESS; 
}

/* lane_mask show lane index info, 0 represent all lanes belong the port */
static int cust_cr_set_port_tx_taps(int unit, int port, int if_side, int lane, cust_plp_tx_fir_t tx_fir)
{
    int err;
    cust_slice_info_t *slice_info;
    cust_port_hdl_info_t cr_port_hdl;
    int taps[7];
    int i, lane_corr;
    err = cust_cr_get_slice_info(unit, port, &slice_info);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_slice_info fail, err:%d", err);
        return CR_FAIL;
    }
    err = cust_cr_get_port_conf_hdl(unit, port, &cr_port_hdl);
    if (err) {
        CUST_CR_ERR_LOG("cust_cr_get_port_conf_hdl fail, err:%d", err);
        return CR_FAIL;
    }
    taps[0] = 0;
    taps[1] = tx_fir.pre2;
    taps[2] = tx_fir.pre;
    taps[3] = tx_fir.main;
    taps[4] = tx_fir.post;
    taps[5] = tx_fir.post2;
    taps[6] = tx_fir.post3;
    if (lane != 0) {
        if (if_side == PLP_SYS_IF_SIDE) {
            if (lane > cr_port_hdl.port_config.host_no_of_lanes) {
                return CR_FAIL;
            }
            lane_corr = cr_port_hdl.port_config.host_start_lane + (lane-1);
        } else {
            if (lane > cr_port_hdl.port_config.line_no_of_lanes) {
                return CR_FAIL;
            }
            lane_corr = cr_port_hdl.port_config.line_start_lane + (lane-1);
        }
        err = cr_serdes_set_tx_taps(slice_info->slice, lane_corr, taps);
        if (err) {
            CUST_CR_ERR_LOG("cr set lane txtaps enable fail, lane:%d err:%d\n", lane_corr, err);
            return CR_FAIL;
        }
    } else {
        if (if_side == PLP_SYS_IF_SIDE) {
            for (i = 0; i < cr_port_hdl.port_config.host_no_of_lanes; i++) {
                lane_corr = cr_port_hdl.port_config.host_start_lane + i;
                err = cr_serdes_set_tx_taps(slice_info->slice, lane_corr, taps);
                if (err) {
                    CUST_CR_ERR_LOG("cr set lane txtaps enable fail, lane:%d err:%d\n", lane_corr, err);
                    return CR_FAIL;
                }
            }
        } else {
            for (i = 0; i < cr_port_hdl.port_config.line_no_of_lanes; i++) {
                lane_corr = cr_port_hdl.port_config.line_start_lane + i;
                err = cr_serdes_set_tx_taps(slice_info->slice, lane_corr, taps);
                if (err) {
                    CUST_CR_ERR_LOG("cr set lane txtaps enable fail, lane:%d err:%d\n", lane_corr, err);
                    return CR_FAIL;
                }
            }
        }
    }
    return CR_SUCCESS;
}

static int cust_cr_port_prbs_set(int unit, int port, int poly, int inv, int if_side, int lane)
{
//     int rv, port_id;
//     int lane_index, lane_start, lane_end, lane_speed;
//     int wait_cnt;
//     uint32_t reg_state;
//     CredoLaneMode_t lane_mode;
//     cust_port_hdl_info_t cr_port_hdl;
//     cust_slice_info_t *slice_info;

//     rv = cust_cr_get_port_conf_hdl(unit, port, &cr_port_hdl);
//     if (rv) {
//         PLP_LOG_ERR("cust_cr_get_port_conf_hdl fail, rv:%d", rv);
//         return PLP_STATUS_FAILURE;
//     }
//     slice_info = cr_port_hdl.slice_info;

//     if (if_side == PLP_SYS_IF_SIDE) {
//         lane_start = cr_port_hdl.port_config.host_start_lane;
//         lane_end = cr_port_hdl.port_config.host_start_lane + cr_port_hdl.port_config.host_no_of_lanes;
//         lane_mode = CR_LMODE_NRZ;
//         lane_speed = 25000;
//     } else {
//         lane_start = cr_port_hdl.port_config.line_start_lane;
//         lane_end = cr_port_hdl.port_config.line_start_lane + cr_port_hdl.port_config.line_no_of_lanes;
//         lane_mode = CR_LMODE_NRZ;
//         lane_speed = 25000;
//     }

//     /* if prbs enable,destroy port and configure phy mode first */
//     rv = cr_port_destroy(cr_port_hdl.slice, cr_port_hdl.port_id);
//     if (rv) {
//         PLP_LOG_ERR("cr destroy port fail, port_id:%d rv:%d\n", cr_port_hdl.port_id, rv);
//         goto cust_cr_set_port_prbs_err;
//     } else {
//         PLP_LOG_INFO("cr destroy port success, port_id:%d\n", cr_port_hdl.port_id);
//     }
//     for (lane_index = lane_start; lane_index < lane_end; lane++) {
//         rv = cr_lane_destroy_mode(cr_port_hdl.slice, lane_index);
//         if (rv) {
//             PLP_LOG_ERR("cr destroy lane mode fail, lane:%d rv:%d\n", lane_index, rv);
//             goto cust_cr_set_port_prbs_err;
//         }
//         rv = cr_lane_configure_mode(cr_port_hdl.slice, lane_index, lane_mode, lane_speed);
//         if (rv) {
//             PLP_LOG_ERR("cr conf lane mode fail, lane:%d lane_mode:%d lane_speed:%d rv:%d\n", lane_index, lane_mode, lane_speed, rv);
//             goto cust_cr_set_port_prbs_err;
//         } else {
//             PLP_LOG_INFO("cr conf lane mode success, lane:%d lane_mode:%d lane_speed:%d\n", lane_index, lane_mode, lane_speed);
//         }
//     }

//     usleep(200000);
//     for (wait_cnt = 0; wait_cnt < 10; wait_cnt) {
//         for (lane_index = lane_start; lane_index < lane_end; lane++) {
//             reg_state = 0;
//             /* check operation of lane config is done */
//             rv = cr_firmware_debug_cmd(cr_port_hdl.slice, lane, 0, 15, &reg_state);
//             if (rv) {
//                 PLP_LOG_ERR("cr_firmware_debug_cmd fail, lane:%d err:%d\n", lane_index, rv);
//                 goto cust_cr_set_port_prbs_err;
//             } else {
//                 if (reg_state == 1) {
//                     PLP_LOG_INFO("lane%d reg refresh end, cnt:%d\n", lane_index, wait_cnt);
//                 } else {
//                     break;
//                 }
//             }
//         }
//         if (reg_state == 1) {
//             break;
//         } else {
//             usleep(200000);
//         }
//     }

//     if (wait_cnt >= 10) {
//         PLP_LOG_ERR("reg is not ready\n");
//     }

//     for (lane = lane_start; lane < lane_end; lane++) {
//         rv = cr_prbs_set_tx_generator(cr_port_hdl.slice, lane, 1, mode);
//         if (rv) {
//             PLP_LOG_ERR("cr set prbs generator fail, lane:%d err:%d\n", lane, rv);
//             goto cust_cr_set_port_prbs_err;
//         }
//         rv = cr_prbs_set_rx_checker(cr_port_hdl.slice, lane, 1, mode);
//         if (rv) {
//             PLP_LOG_ERR("cr set prbs checker fail, lane:%d err:%d\n", lane, rv);
//             goto cust_cr_set_port_prbs_err;
//         }
//     }

//     PLP_LOG_INFO("cust_cr_set_port_prbs success, unit:%d port:%d side:%d mode:%d enable:%d\n", 
//         unit, port, side, mode, 1);
//     return PLP_STATUS_SUCCESS;
    
// cust_cr_set_port_prbs_err:
//     PLP_LOG_ERR("cust_cr_set_port_prbs fail, unit:%d port:%d side:%d mode:%d enable:%d err:%d\n", 
//         unit, port, side, mode, 1, err);
    return PLP_STATUS_FAILURE;
}

static int cust_cr_port_prbs_get(int unit, int port, int if_side, cust_plp_prbs_status_t *prbs_info)
{
    // int rv = 0;
    // uint32_t prbs_lock = 0, lock_loss = 0, err_cnt = 0;
    // bcm_plp_access_t phy_access;
    // cust_bcm_phy_info_t *phy_info;
    // uint32_t lane_i, lane_map, lane_num;
    // if (prbs_info == NULL) {
    //     return PLP_STATUS_INVALID_PARAMETER;
    // }
    // memset(&phy_access, 0, sizeof(bcm_plp_access_t));
    // rv = cust_phy_access_get(unit, port, if_side, &phy_access);
    // if (rv != PLP_STATUS_SUCCESS) {
    //     PLP_LOG_ERR("unit[%d] port[%d], cust_phy_access_get failed with rv:%d", unit, port, rv);
    //     return PLP_STATUS_ITEM_NOT_FOUND;
    // }
    // rv = cust_phy_info_get(unit, port, &phy_info);
    // if (rv != PLP_STATUS_SUCCESS) {
    //     PLP_LOG_ERR("unit[%d] port[%d], cust_phy_access_get failed with rv:%d", unit, port, rv);
    //     return PLP_STATUS_ITEM_NOT_FOUND;
    // }
    // lane_map = phy_access.lane_map;
    // GET_PORT_LANE_NUM(lane_map, lane_num);
    // for (lane_i = 1; lane_i <= lane_num; lane_i++) {
    //     prbs_lock = 0;
    //     lock_loss = 0;
    //     err_cnt = 0;
    //     GET_PORT_SINGLE_LANE_MAP(lane_map, lane_i, phy_access.lane_map);
    //     PLP_LOG_DBG("unit[%d] port[%d], lane[%d] if_side[%d], lane_map is 0x%x", unit, port, lane_i, if_side, phy_access.lane_map);
    //     rv = bcm_plp_prbs_status_get(phy_info->chip_name, phy_access, &prbs_lock, &lock_loss, &err_cnt);
    //     if (rv != 0) {
    //         PLP_LOG_ERR("unit[%d] port[%d], bcm_plp_prbs_status_get failed with rv:%d", unit, port, rv);
    //     }
    //     prbs_info[lane_i - 1].is_get = true;
    //     prbs_info[lane_i - 1].lane_err_cnt = err_cnt;
    //     prbs_info[lane_i - 1].lock_loss = lock_loss;
    //     prbs_info[lane_i - 1].prbs_lock = prbs_lock;
    // }
    return PLP_STATUS_SUCCESS;
}
/* time_v 毫秒*/
static int cust_cr_port_prbs_ber_get(int unit, int port, int if_side, int time_v, cust_plp_prbs_status_t *prbs_info)
{
    // int i;
    // int rv = 0;
    // bcm_plp_access_t phy_access;
    // cust_bcm_phy_info_t *phy_info;
    // int speed = 0, if_type = 0, ref_clk = 0, if_mode = 0;
    // BCM_PLP_DEVICE_AUX_MODE aux_mode;
    // int lane_num;
    // double ber;
    // uint32_t count;

    // if (prbs_info == NULL) {
    //     return PLP_STATUS_INVALID_PARAMETER;
    // }
    // memset(&phy_access, 0, sizeof(bcm_plp_access_t));
    // rv = cust_phy_access_get(unit, port, if_side, &phy_access);
    // if (rv != PLP_STATUS_SUCCESS) {
    //     PLP_LOG_ERR("unit[%d] port[%d], cust_phy_access_get failed with rv:%d", unit, port, rv);
    //     return PLP_STATUS_ITEM_NOT_FOUND;
    // }
    // rv = cust_phy_info_get(unit, port, &phy_info);
    // if (rv != PLP_STATUS_SUCCESS) {
    //     PLP_LOG_ERR("unit[%d] port[%d], cust_phy_access_get failed with rv:%d", unit, port, rv);
    //     return PLP_STATUS_ITEM_NOT_FOUND;
    // }
    // cust_bcm_port_prbs_get(unit, port, if_side, prbs_info);
    // memset(&aux_mode, 0, sizeof(BCM_PLP_DEVICE_AUX_MODE));
    // rv = bcm_plp_mode_config_get(phy_info->chip_name, phy_access, &speed, &if_type, &ref_clk,
    //     &if_mode, (void*)&aux_mode);
    // if (rv != 0) {
    //     PLP_LOG_ERR("unit[%d] port[%d], bcm_plp_mode_config_get failed with rv:%d", unit, port, rv);
    //     return PLP_STATUS_FAILURE;
    // }
    // for (i = 0; i < BCM_PHY_MAX_LANE; i++) {
    //     if (prbs_info[i].is_get != true) {
    //         continue;
    //     }
    //     ber = (((double)prbs_info[i].lane_err_cnt)/((double)time_v/1000))/((double)aux_mode.lane_data_rate*1024*1024);
    //     PLP_LOG_DBG("Unit[%d] Port[%3d] Lane[%d] err_cnt[%10d] Ber %4.2e, time_v[%d]", unit, port, i, prbs_info[i].lane_err_cnt, ber, time_v);
    //     prbs_info[i].ber = ber;
    // }
    return PLP_STATUS_SUCCESS;
}

static int cust_cr_port_prbs_clear(int unit, int port, int if_side)
{
    // int rv = 0;
    // uint32_t tx_rx = 0;
    // cust_bcm_phy_info_t *phy_info;
    // bcm_plp_access_t  phy_access;
    // memset(&phy_access, 0, sizeof(bcm_plp_access_t));
    // rv = cust_phy_access_get(unit, port, if_side, &phy_access);
    // if (rv != PLP_STATUS_SUCCESS) {
    //     PLP_LOG_ERR("unit[%d] port[%d], cust_phy_access_get failed with rv:%d", unit, port, rv);
    //     return PLP_STATUS_ITEM_NOT_FOUND;
    // }
    // rv = cust_phy_info_get(unit, port, &phy_info);
    // if (rv != PLP_STATUS_SUCCESS) {
    //     PLP_LOG_ERR("unit[%d] port[%d], cust_phy_access_get failed with rv:%d", unit, port, rv);
    //     return PLP_STATUS_ITEM_NOT_FOUND;
    // }
    // /* tx_rx == 0 means both dir */
    // rv = bcm_plp_prbs_clear(phy_info->chip_name, phy_access, tx_rx);
    // if (rv != 0) {
    //     PLP_LOG_ERR("unit[%d] port[%d], bcm_plp_prbs_clear failed with rv:%d", unit, port, rv);
    // }
    return PLP_STATUS_SUCCESS;
}