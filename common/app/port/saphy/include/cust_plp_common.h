#ifndef _CUST_EXTPHY_COMMON_H_
#define _CUST_EXTPHY_COMMON_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include "cust_plp_if_api.h"

#define CUST_PLP_LOG_LEVEL_ERROR   (0x1)
#define CUST_PLP_LOG_LEVEL_WARN    (0x2)
#define CUST_PLP_LOG_LEVEL_NOTICE  (0x3)
#define CUST_PLP_LOG_LEVEL_INFO    (0x4)
#define CUST_PLP_LOG_LEVEL_DEBUG   (0x5)
#define CUST_PLP_LOG_LEVEL_ALL     (0xf)

#ifndef CUST_PLP_LOG_LEVEL
// #define CUST_PLP_LOG_LEVEL CUST_PLP_LOG_LEVEL_INFO
#define CUST_PLP_LOG_LEVEL CUST_PLP_LOG_LEVEL_DEBUG
#endif

#define PLP_CFG_FILE_STR_MAX   (128)
#define PLP_CFG_FILE_ARR_MAX   (32)

#ifdef SYSLOG_SUPPORT
/*LOG_EMERG */
/*LOG_ALERT */
/*LOG_CRIT */
#define PLP_LOG_ERR(fmt, args...)     syslog(LOG_ERR, fmt, ##args)
#define PLP_LOG_WARN(fmt, args...)    syslog(LOG_WARNING, fmt, ##args)
#define PLP_LOG_NOTICE(fmt, args...)  syslog(LOG_NOTICE, fmt, ##args)
#define PLP_LOG_INFO(fmt, args...)    syslog(LOG_INFO, fmt, ##args)
#define PLP_LOG_DBG(fmt, args...)     syslog(LOG_DEBUG, fmt, ##args)
#else
#define PLP_LOG_ERR(fmt, args...)     cust_plp_log_error("%s ERROR Func=[%s], Line=[%d] " fmt"\n", \
                                    getCurrentTime(), __FUNCTION__, __LINE__, ##args)
#define PLP_LOG_WARN(fmt, args...)    cust_plp_log_warn("%s WARN Func=[%s], Line=[%d] " fmt"\n", \
                                    getCurrentTime(), __FUNCTION__, __LINE__, ##args)
#define PLP_LOG_NOTICE(fmt, args...)  cust_plp_log_notice("%s NOTICE Func=[%s], Line=[%d] " fmt"\n", \
                                    getCurrentTime(), __FUNCTION__, __LINE__, ##args)
#define PLP_LOG_INFO(fmt, args...)    cust_plp_log_info("%s INFO Func=[%s], Line=[%d] " fmt"\n", \
                                    getCurrentTime(), __FUNCTION__, __LINE__, ##args)
#define PLP_LOG_DBG(fmt, args...)     cust_plp_log_debug("%s DEBUG Func=[%s], Line=[%d] " fmt"\n", \
                                    getCurrentTime(), __FUNCTION__, __LINE__, ##args)
#endif

#define TX_MODE_NRZ_LP_3TAP  1
#define TX_MODE_NRZ_6TAP     2 
#define TX_MODE_PAM4_LP_3TAP 3
#define TX_MODE_PAM4_6TAP    4

typedef enum {
    PLP_MDIO_TYPE_MAC = 0,
    PLP_MDIO_TYPE_SYSFS = 1,
    PLP_MDIO_TYPE_SYSFS_CUST = 2,
    PLP_MDIO_TYPE_MAX
} plp_mdio_type_e;

typedef int (*mdio_read_func)(void *user_acc, unsigned int mdio_addr, unsigned int reg_addr, unsigned int *data);
typedef int (*mdio_write_func)(void *user_acc, unsigned int mdio_addr, unsigned int reg_addr, unsigned int data);
typedef int (*cust_plp_log_func_t)(const char *format, va_list args);

typedef enum {
    PHY_TYPE_MILLENIO = 0,
    PHY_TYPE_CREDO
} plp_phy_type_e;

typedef struct {
    int  (*cust_plp_platform_init)(int unit, bool warm_boot);
    bool (*cust_plp_port_support)(int unit, int port, int physical_port);
    int  (*cust_plp_create_port)(int unit, int port, cust_plp_port_resource_t *port_resource); 
    int  (*cust_plp_remove_port)(int unit, int port); 
    int  (*cust_plp_port_speed_set)(int unit, int port, int speed);
    int  (*cust_plp_port_speed_get)(int unit, int port, int *speed);
    int  (*cust_plp_port_fec_support)(int unit, int port);
    int  (*cust_plp_port_fec_set)(int unit, int port, cust_plp_fec_e fec_type);
    int  (*cust_plp_port_fec_get)(int unit, int port, cust_plp_fec_e *fec_type);
    int  (*cust_plp_port_autoneg_set)(int unit, int port, int enable);
    int  (*cust_plp_port_autoneg_get)(int unit, int port, int *enable);
    int  (*cust_plp_port_linktrain_set)(int unit, int port, int if_side, int enable);
    int  (*cust_plp_port_prbs_set)(int unit, int port, int poly, int inv, int if_side, int lane);
    int  (*cust_plp_port_prbs_get)(int unit, int port, int if_side, cust_plp_prbs_status_t *prbs_info);
    int  (*cust_plp_port_prbs_clear)(int unit, int port, int if_side);
    int  (*cust_plp_port_prbs_ber_get)(int unit, int port, int if_side, int time_v, cust_plp_prbs_status_t *prbs_info);
    int  (*cust_plp_squelch_set)(int unit, int port, int if_side, int tx_rx, int enable, int lane);
    int  (*cust_plp_loopback_set)(int unit, int port, int if_side, int lb_dir, int enable);
    int  (*cust_plp_loopback_get)(int unit, int port, int if_side, int lb_dir, int *enable);
    int  (*cust_plp_polarity_set)(int unit, int port, int if_side, int tx_rx, uint32_t polarity, bool override);
    int  (*cust_plp_polarity_get)(int unit, int port, int if_side, int tx_rx, uint32_t *polarity);
    int  (*cust_plp_tx_fir_set)(int unit, int port, int if_side, int lane, cust_plp_tx_fir_t tx_fir);
    int  (*cust_plp_tx_fir_get)(int unit, int port, int if_side, int lane, cust_plp_tx_fir_t *tx_fir);
    int  (*cust_plp_phy_status_dump)(int unit, int port, int level);
    int  (*cust_plp_dsc_dump)(int unit, int port, int if_side, int flag, int lane);
    int  (*cust_plp_shell_cmd_run)(int unit, int port, char *shell_cmd);
    int  (*cust_plp_temperature_get)(int unit, int port, double *temp);
    int  (*cust_plp_highest_temperature_get)(double *temp);
    int  (*cust_plp_phy_reset)(int unit, int port, int flags);
    int  (*cust_plp_reg_set)(int unit, int port, int if_side, int devaddr, int regaddr, int flag, long long unsigned int data);
    int  (*cust_plp_reg_get)(int unit, int port, int if_side, int devaddr, int regaddr, int flag, long long unsigned int *data);
    void (*cust_plp_techsupport)(int unit, int port);
    int  (*cust_plp_mib_dump)(int unit, int port, int if_side);
    int  (*cust_plp_channel_reach_set)(int unit, int port, int if_side, int lane, int nr_er);
    int  (*cust_plp_version_get)(int unit, int port, cust_plp_version_t *version_info);
} cust_plp_apis_t;

int cust_plp_common_init(int unit, bool warm_boot);
void cust_plp_log_func_reg(cust_plp_log_func_t log_func);
void cust_plp_log_debug(const char *format, ...);
void cust_plp_log_info(const char *format, ...);
void cust_plp_log_notice(const char *format, ...);
void cust_plp_log_warn(const char *format, ...);
void cust_plp_log_error(const char *format, ...);
int plp_get_cfg_info(char *file_path, char *config_buf, int *val, bool is_arr);
int plp_get_cfg_info_str(char *file_path, char *config_buf, char *val);
int plp_get_mdio_read_func(plp_mdio_type_e type, mdio_read_func *func);
int plp_get_mdio_write_func(plp_mdio_type_e type, mdio_write_func *func);
int cust_plp_mdio_func_reg(plp_mdio_type_e type, mdio_read_func read_func, mdio_write_func write_func);

char* getCurrentTime(void);
int _parse_param(char *str);
char* _dump_int_array(int *input, int input_size, char *output, int output_size);
int tx_fir_cfg_parse(char *cfg_file, int phy_addr, int if_side, int lane, int serdes_mode, cust_plp_medium_e medium, cust_plp_tx_fir_t *tx_fir);
// char* _dump_int_array(int *input, int input_size, char output[], int output_size);

#endif /* _CUST_EXTPHY_COMMON_H_ */