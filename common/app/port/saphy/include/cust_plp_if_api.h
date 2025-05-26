#ifndef _CUST_EXTPHY_H_
#define _CUST_EXTPHY_H_

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define PLP_SYS_IF_SIDE (1)
#define PLP_LINE_IF_SIDE (0)
#define PLP_TX_DIR (0)
#define PLP_RX_DIR (1)
#define PLP_PORT_ENABLE (1)
#define PLP_PORT_DISABLE (0)

typedef enum {
    PLP_STATUS_SUCCESS                =  0,
    PLP_STATUS_FAILURE                = -1,
    PLP_STATUS_NOT_SUPPORTED          = -2,
    PLP_STATUS_NO_MEMORY              = -3,
    PLP_STATUS_INSUFFICIENT_RESOURCES = -4,
    PLP_STATUS_INVALID_PARAMETER      = -5,
    PLP_STATUS_ITEM_ALREADY_EXISTS    = -6,
    PLP_STATUS_ITEM_NOT_FOUND         = -7,
    PLP_STATUS_NOT_IMPLEMENTED        = -8,
    PLP_STATUS_NOT_SUPPORTED_PORT     = -9,
} cust_plp_status_type_e;

typedef enum {
    CUST_PLP_FEC_INVALID = 0, /* invalid */
    CUST_PLP_FEC_NONE, /* no fec */
    CUST_PLP_FEC_BASER, /* CL74/Base-R. 64/66b KR FEC for fabric */
    CUST_PLP_FEC_RSFEC, /* CL91/RS-FEC */
    CUST_PLP_FEC_RS544, /* Rs544, using 1xN RS FEC architecture */
    CUST_PLP_FEC_RS272, /* Rs272, using 1xN RS FEC architecture */
    CUST_PLP_FEC_RS206, /* Rs206. 64/66b 5T RS FEC for fabric */
    CUST_PLP_FEC_RS108, /* Rs108. 64/66b 5T low latency RS FEC for fabric */
    CUST_PLP_FEC_RS545, /* Rs545. 64/66b 15T RS FEC for fabric */
    CUST_PLP_FEC_RS304, /* Rs304. 64/66b 15T low latency RS FEC for fabric */ 
    CUST_PLP_FEC_RS544_2XN, /* Rs544, using 2xN RS FEC architecture */
    CUST_PLP_FEC_RS272_2XN, /* Rs272, using 2xN RS FEC architecture */
    CUST_PLP_FEC_MAX
} cust_plp_fec_e;

typedef enum {
    CUST_PLP_MEDIUM_INVALID = 0,
    CUST_PLP_MEDIUM_BACKPLANE,
    CUST_PLP_MEDIUM_COPPER,
    CUST_PLP_MEDIUM_FIBER,
    CUST_PLP_MEDIUM_MAX
} cust_plp_medium_e;

typedef struct {
    uint8_t  is_get;
    uint32_t prbs_lock;
    uint32_t lock_loss;
    uint32_t lane_err_cnt;
    double ber;
} cust_plp_prbs_status_t;

typedef struct {
    int phy_lane0;
    int speed;
    int host_lanes;
    int line_lanes;
    cust_plp_fec_e host_fec_type;
    cust_plp_fec_e line_fec_type;
    int link_training;
} cust_plp_port_resource_t;

typedef struct {
    int pre3;
    int pre2;
    int pre;
    int main;
    int post;
    int post2;
    int post3;
} cust_plp_tx_fir_t;

typedef struct {
    uint32_t phy_id;
    char chip_name[32];
    uint32_t fw_ver;
    uint32_t fw_crc;
    uint32_t rev_id;
    uint32_t drv_major_ver;
    uint32_t drv_minor_ver;
} cust_plp_version_t;

int cust_plp_platform_init(int unit, bool warm_boot);
int cust_plp_port_speed_set(int unit, int port, int speed);
int cust_plp_port_linktrain_set(int unit, int port, int if_side, int enable);
int cust_plp_squelch_set(int unit, int port, int if_side, int tx_rx, int enable, int lane);
int cust_plp_port_prbs_set(int unit, int port, int poly, int inv, int if_side, int lane);
int cust_plp_port_prbs_get(int unit, int port, int if_side, cust_plp_prbs_status_t *prbs_info);
int cust_plp_port_prbs_clear(int unit, int port, int if_side);
int cust_plp_port_prbs_ber_get(int unit, int port, int if_side, int time_v, cust_plp_prbs_status_t *prbs_info);
int cust_plp_loopback_set(int unit, int port, int if_side, int lb_dir, int enable);
int cust_plp_loopback_get(int unit, int port, int if_side, int lb_dir, int *enable);
int cust_plp_polarity_set(int unit, int port, int if_side, int tx_rx, uint32_t polarity, bool override);
int cust_plp_polarity_get(int unit, int port, int if_side, int tx_rx, uint32_t *polarity);
int cust_plp_tx_fir_set(int unit, int port, int if_side, int lane, cust_plp_tx_fir_t tx_fir);
int cust_plp_tx_fir_get(int unit, int port, int if_side, int lane, cust_plp_tx_fir_t *tx_fir);
int cust_plp_reg_set(int unit, int port, int if_side, int devaddr, int regaddr, int flag, long long unsigned int data);
int cust_plp_reg_get(int unit, int port, int if_side, int devaddr, int regaddr, int flag, long long unsigned int *data);
int cust_plp_phy_status_dump(int unit, int port, int level);
int cust_plp_dsc_dump(int unit, int port, int if_side, int flag, int lane);
int cust_plp_shell_cmd_run(int unit, int port, char *cmdline);
int cust_plp_phy_reset(int unit, int port, int flags);
int cust_plp_temperature_get(int unit, int port, double *temp);
int cust_plp_highest_temperature_get(double *temp);
bool cust_plp_port_support(int unit, int port);
bool cust_plp_physical_port_support(int unit, int physical_port);
int  cust_plp_mib_dump(int unit, int port, int if_side);
int  cust_plp_version_get(int unit, int port, cust_plp_version_t *version_info);
void cust_plp_techsupport(int unit, int port);
int  cust_plp_channel_reach_set(int unit, int port, int if_side, int lane, int nr_er);

#endif /* _CUST_EXTPHY_H_ */