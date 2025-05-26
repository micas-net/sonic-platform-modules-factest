#include "cust_plp_if_api.h"
#include "cust_plp_common.h"

#define EXTPHY_APIS_NUMBER 2

#define EXTPHY_API_EXEC(_func, args...) \
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) { \
        if (g_platform_inited[i] == false) { \
            continue; \
        } \
        if (cust_plp_port_support(unit, port) == false) { \
            continue; \
        } \
        if (cust_extphy_apis[i]->##_func## == NULL) { \
            continue; \
        } \
        return cust_extphy_apis[i]->##_func##(##args); \
    }

// #define API_FILTER(_api) 

extern cust_plp_apis_t cust_extphy_credo_apis;
extern cust_plp_apis_t cust_extphy_aperta2_apis;

cust_plp_apis_t *cust_extphy_apis[EXTPHY_APIS_NUMBER] = {
    &cust_extphy_credo_apis,
    &cust_extphy_aperta2_apis
};

static bool g_platform_inited[EXTPHY_APIS_NUMBER] = {0};

int cust_plp_platform_init(int unit, bool warm_boot)
{
    int i, rv, err;
    /* init log module */
    cust_plp_common_init(unit, warm_boot);
    PLP_LOG_DBG("cust_plp_platform_init start.");

    rv = PLP_STATUS_SUCCESS;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (cust_extphy_apis[i]->cust_plp_platform_init == NULL) {
            continue;
        }
        err = cust_extphy_apis[i]->cust_plp_platform_init(unit, warm_boot);
        if (err == PLP_STATUS_SUCCESS) {
            g_platform_inited[i] = true;
        } else if (err == PLP_STATUS_FAILURE) {
            PLP_LOG_ERR("cust_plp_platform_init failed, platform_id: %d", i);
            rv = PLP_STATUS_FAILURE;
        }
    }

    PLP_LOG_DBG("cust_plp_platform_init end.");
    return rv;
}

bool cust_plp_port_support(int unit, int port)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_support == NULL) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_support(unit, port, -1)) {
            return true;
        }
    }
    return false;
}

bool cust_plp_physical_port_support(int unit, int physical_port)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_support == NULL) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_support(unit, -1, physical_port)) {
            return true;
        }
    }
    return false;
}

int cust_plp_port_speed_set(int unit, int port, int speed)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_speed_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_port_speed_set(unit, port, speed);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_shell_cmd_run(int unit, int port, char *cmdline)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_shell_cmd_run == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_shell_cmd_run(unit, port, cmdline);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

void cust_plp_techsupport(int unit, int port)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_techsupport == NULL) {
            continue;
        }
        cust_extphy_apis[i]->cust_plp_techsupport(unit, port);
    }
}

int cust_plp_temperature_get(int unit, int port, double *temp)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_temperature_get == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_temperature_get(unit, port, temp);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_highest_temperature_get(double *temp)
{
    int i;
    int err;
    double tmp_temp;

    *temp = 0;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_highest_temperature_get == NULL) {
            continue;
        }
        err = cust_extphy_apis[i]->cust_plp_highest_temperature_get(&tmp_temp);
        if (err) {
            PLP_LOG_ERR("highest_temperature_get failed, err: %d", err);
            return err;
        }
        if (tmp_temp > *temp) {
            *temp = tmp_temp;
        }
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_port_linktrain_set(int unit, int port, int if_side, int enable)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_linktrain_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_port_linktrain_set(unit, port, if_side, enable);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_port_prbs_set(int unit, int port, int poly, int inv, int if_side, int lane)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_prbs_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_port_prbs_set(unit, port, poly, inv, if_side, lane);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_port_prbs_get(int unit, int port, int if_side, cust_plp_prbs_status_t *prbs_info)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_prbs_get == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_port_prbs_get(unit, port, if_side, prbs_info);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_port_prbs_ber_get(int unit, int port, int if_side, int time_v, cust_plp_prbs_status_t *prbs_info)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_prbs_ber_get == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_port_prbs_ber_get(unit, port, if_side, time_v, prbs_info);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_loopback_set(int unit, int port, int if_side, int lb_dir, int enable)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_loopback_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_loopback_set(unit, port, if_side, lb_dir, enable);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_loopback_get(int unit, int port, int if_side, int lb_dir, int *enable)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_loopback_get == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_loopback_get(unit, port, if_side, lb_dir, enable);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_phy_status_dump(int unit, int port, int level)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_phy_status_dump == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_phy_status_dump(unit, port, level);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_dsc_dump(int unit, int port, int if_side, int flag, int lane)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_dsc_dump == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_dsc_dump(unit, port, if_side, flag, lane);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_port_prbs_clear(int unit, int port, int if_side)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_port_prbs_clear == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_port_prbs_clear(unit, port, if_side);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_squelch_set(int unit, int port, int if_side, int tx_rx, int enable, int lane)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_squelch_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_squelch_set(unit, port, if_side, tx_rx, enable, lane);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_polarity_set(int unit, int port, int if_side, int tx_rx, uint32_t polarity, bool override)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_polarity_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_polarity_set(unit, port, if_side, tx_rx, polarity, override);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_polarity_get(int unit, int port, int if_side, int tx_rx, uint32_t *polarity)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_polarity_get == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_polarity_get(unit, port, if_side, tx_rx, polarity);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_phy_reset(int unit, int port, int flags)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_phy_reset == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_phy_reset(unit, port, flags);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_tx_fir_set(int unit, int port, int if_side, int lane, cust_plp_tx_fir_t tx_fir)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_tx_fir_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_tx_fir_set(unit, port, if_side, lane, tx_fir);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_tx_fir_get(int unit, int port, int if_side, int lane, cust_plp_tx_fir_t *tx_fir)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_tx_fir_get == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_tx_fir_get(unit, port, if_side, lane, tx_fir);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_reg_set(int unit, int port, int if_side, int devaddr, int regaddr, int flag, long long unsigned int data)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_reg_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_reg_set(unit, port, if_side, devaddr, regaddr, flag, data);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_reg_get(int unit, int port, int if_side, int devaddr, int regaddr, int flag, long long unsigned int *data)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_reg_get == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_reg_get(unit, port, if_side, devaddr, regaddr, flag, data);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_mib_dump(int unit, int port, int if_side)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_mib_dump == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_mib_dump(unit, port, if_side);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_channel_reach_set(int unit, int port, int if_side, int lane, int nr_er)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_channel_reach_set == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_channel_reach_set(unit, port, if_side, lane, nr_er);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}

int cust_plp_version_get(int unit, int port, cust_plp_version_t *version_info)
{
    int i;
    for(i = 0; i < EXTPHY_APIS_NUMBER; i++) {
        if (g_platform_inited[i] == false) {
            continue;
        }
        if (cust_plp_port_support(unit, port) == false) {
            continue;
        }
        if (cust_extphy_apis[i]->cust_plp_version_get == NULL) {
            return PLP_STATUS_NOT_IMPLEMENTED;
        }
        return cust_extphy_apis[i]->cust_plp_version_get(unit, port, version_info);
    }
    return PLP_STATUS_NOT_SUPPORTED_PORT;
}