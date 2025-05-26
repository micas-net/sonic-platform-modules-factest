#include <sys/time.h>
#include "cust_plp_if_api.h"
#include "cust_plp_debug.h"
#include "cust_plp_common.h"
#include "list.h"

/*
#define PLP_CLI(fmt, args...) \
    do { \
        g_cli_log_func(fmt"\n", ##args); \
    } while (0)
*/

/*
#define PLP_CLI(fmt, args...) \
    do { \
        printf(fmt"\n", ##args); \
    } while (0)
*/

#define PLP_CLI(fmt, args...) \
    do { \
        printf(fmt"\n", ##args); \
    } while (0)

static bool g_cli_debug_mode = false;
#define PLP_CLI_DEBUG(fmt, args...) \
    if (g_cli_debug_mode) { \
        PLP_CLI(fmt"\n", ##args); \
    }

// typedef int (*cust_cli_log_func_t)(const char *format, ...);
// static cust_cli_log_func_t g_cli_log_func = NULL;
static cust_plp_log_func_t g_cust_debug_log = NULL;

#ifdef EXTPHY_BINARY
#define HELP_MSG_PREFIX "<unit> <port>"
#else
#define HELP_MSG_PREFIX "phy user_diag <port_range>"
#endif

#define PRBS_MAX_LANE_NUM 8

struct list_head g_prbsstat_info_list_head;
static int inited = 0;

typedef struct {
    int unit;
    int port;
    int if_side;
    int timestamp;
    struct list_head node;
} cust_prbsstat_info_t;

static void dump_help_msg()
{
    PLP_CLI("------usage help---------");

    PLP_CLI(HELP_MSG_PREFIX" dbg_on\n");
    PLP_CLI(HELP_MSG_PREFIX" dbg_off\n");
    PLP_CLI(HELP_MSG_PREFIX" dsc if_side=<?> [lane=<1-8>] [flag=<?>]\n");
    PLP_CLI(HELP_MSG_PREFIX" prbs_set if_side=<?> poly=<?> [inv=<?>] [lane=<1-8>]\n");
    PLP_CLI(HELP_MSG_PREFIX" prbs_get if_side=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" prbs_clear if_side=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" prbsstat_start if_side=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" prbsstat_ber if_side=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" prbsstat_clear if_side=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" phy_status [level=<?>]\n");
    PLP_CLI(HELP_MSG_PREFIX" polarity_set if_side=<?> tx_rx=<?> polarity=<?> [override=<?>]\n");
    PLP_CLI(HELP_MSG_PREFIX" polarity_get if_side=<?> tx_rx=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" phy_reset [flag=<?>]\n");
    PLP_CLI(HELP_MSG_PREFIX" squelch_set if_side=<?> tx_rx=<?> enable=<?> [lane=<1-8>]\n");
    PLP_CLI(HELP_MSG_PREFIX" loopback_set if_side=<?> lb_dir=<?> enable=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" loopback_get if_side=<?> [lb_dir=<?>]\n");
    PLP_CLI(HELP_MSG_PREFIX" temperature_get\n");
    PLP_CLI(HELP_MSG_PREFIX" highest_temperature_get\n");
    PLP_CLI(HELP_MSG_PREFIX" linktrain_set if_side=<?> enable=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" tx_fir_set if_side=<?> lane=<1-8> main=<?> pre=<?> post=<?> pre2=<?> post2=<?> pre3=<?> post3=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" tx_fir_get if_side=<?> lane=<1-8>\n");
    PLP_CLI(HELP_MSG_PREFIX" setreg if_side=<?> [devaddr=<?>] regaddr=<?> data64=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" getreg if_side=<?> [devaddr=<?>] regaddr=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" mibdump if_side=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" channel_reach if_side=<?> nr_er=<?> lane=<?>\n");
    PLP_CLI(HELP_MSG_PREFIX" techsupport\n");
    PLP_CLI(HELP_MSG_PREFIX" version\n");
    PLP_CLI(HELP_MSG_PREFIX" show_techsupport\n");

    fflush(NULL);
}

static int g_time_count = 0;

static uint32_t cust_timestamp_get()
{
    struct timeval time;  
    gettimeofday(&time, NULL );
    return time.tv_sec * 1000 + time.tv_usec/1000;
}

static int cust_debug_prbsstat_ber(int unit, int port, int if_side)
{
    int i, rv;
    int time_passed;
    bool entry_find = false;
    cust_prbsstat_info_t *prbsstat_info;
    cust_plp_prbs_status_t prbs_info[PRBS_MAX_LANE_NUM];

    if (!cust_plp_port_support(unit, port)) {
        return PLP_STATUS_NOT_SUPPORTED_PORT;
    }
    list_for_each_entry(prbsstat_info, &g_prbsstat_info_list_head, node, cust_prbsstat_info_t) {
        if (prbsstat_info->unit != unit) {
            continue;
        }
        if (prbsstat_info->port != port) {
            continue;
        }
        if (prbsstat_info->if_side != if_side) {
            continue;
        }
        entry_find = true;
        break;
    }
    if (entry_find == false) {
        PLP_CLI("prbsstat entry not found!");
        return PLP_STATUS_ITEM_NOT_FOUND;
    }
    time_passed = cust_timestamp_get() - prbsstat_info->timestamp + 1;
    printf("timepassed: %dms, side: %s\n", time_passed, (if_side == PLP_SYS_IF_SIDE) ? "SYS": "LINE");
    memset(prbs_info, 0, sizeof(cust_plp_prbs_status_t)*PRBS_MAX_LANE_NUM);
    rv =  cust_plp_port_prbs_ber_get(unit, port, if_side, time_passed, prbs_info);
    if (rv != 0) {
        PLP_CLI("cust_plp_port_prbs_ber_dump failed");
    }
    prbsstat_info->timestamp = cust_timestamp_get();
    PLP_CLI("====");
    for (i = 0; i < PRBS_MAX_LANE_NUM; i++) {
        if (prbs_info[i].is_get) {
            if (prbs_info[i].prbs_lock == 0) {
                PLP_CLI("%d[%d] : Nolock", port, i);
            } else if (prbs_info[i].lock_loss == 1) {
                PLP_CLI("%d[%d] : LossOfLock", port, i);
            } else if (prbs_info[i].ber == 0){
                PLP_CLI("%d[%d] : OK!", port, i);
            } else {
                PLP_CLI("%d[%d] : %4.2e", port, i, prbs_info[i].ber);
            }
        }
        //  else {
        //     PLP_CLI("%d[%d] : !chk_en", port, i);
        // }
    }
    PLP_CLI("====");
    return rv;
}

static int cust_debug_prbsstat_update(int unit, int port, int if_side)
{
    int rv;
    bool entry_find = false;
    cust_prbsstat_info_t *prbsstat_info;
    cust_plp_prbs_status_t prbs_info[PRBS_MAX_LANE_NUM];

    if (!cust_plp_port_support(unit, port)) {
        return PLP_STATUS_NOT_SUPPORTED_PORT;
    }
    list_for_each_entry(prbsstat_info, &g_prbsstat_info_list_head, node, cust_prbsstat_info_t) {
        if (prbsstat_info->unit != unit) {
            continue;
        }
        if (prbsstat_info->port != port) {
            continue;
        }
        if (prbsstat_info->if_side != if_side) {
            continue;
        }
        entry_find = true;
        break;
    }
    if (entry_find == false) {
        prbsstat_info = (cust_prbsstat_info_t *)malloc(sizeof(cust_prbsstat_info_t));
        if (prbsstat_info == NULL) {
            PLP_CLI("malloc failed!");
            return PLP_STATUS_NO_MEMORY;
        }
        list_add_tail(&prbsstat_info->node, &g_prbsstat_info_list_head);
    }
    prbsstat_info->unit = unit;
    prbsstat_info->port = port;
    prbsstat_info->if_side = if_side;
    memset(prbs_info, 0, sizeof(cust_plp_prbs_status_t)*PRBS_MAX_LANE_NUM);
    rv = cust_plp_port_prbs_get(unit, port, if_side, prbs_info);
    if (rv != 0) {
        PLP_CLI("cust_plp_port_prbs_get failed!");
        return PLP_STATUS_FAILURE;
    }
    prbsstat_info->timestamp = cust_timestamp_get();
    return PLP_STATUS_SUCCESS;
}

static int cust_debug_prbsstat_clear(int unit, int port, int if_side)
{
    bool entry_find = false;
    cust_prbsstat_info_t *prbsstat_info;

    if (!cust_plp_port_support(unit, port)) {
        return PLP_STATUS_NOT_SUPPORTED_PORT;
    }
    list_for_each_entry(prbsstat_info, &g_prbsstat_info_list_head, node, cust_prbsstat_info_t) {
        if (prbsstat_info->unit != unit) {
            continue;
        }
        if (prbsstat_info->port != port) {
            continue;
        }
        if (prbsstat_info->if_side != if_side) {
            continue;
        }
        entry_find = true;
        break;
    }
    if (entry_find == true) {
        list_del_init(&prbsstat_info->node);
        free(prbsstat_info);
    }
    return PLP_STATUS_SUCCESS;
}

static void diag_phy_control(int unit, int port, char *diag_func_name, char **diag_params, int n_param)
{
    int i, rv;
    int param_i=0;
    char *p, *shell_cmd;
    double temp;
    int if_side=0, enable=0, poly=0, lane=0, speed;
    int lb_dir=0, flag=0, level=1;
    int tx_rx=0, inv=0, override=0, nr_er=0;
    uint32_t polarity=0, data=0, devaddr=0, regaddr=0;
    long long unsigned int data_64=0;
    cust_plp_tx_fir_t tx_fir;
    cust_plp_version_t version_info;
    char func_name[128];
    
    rv = PLP_STATUS_SUCCESS;
    cust_plp_prbs_status_t prbs_info[PRBS_MAX_LANE_NUM];
    memset(&tx_fir, 0, sizeof(cust_plp_tx_fir_t));
    memset(prbs_info, 0, sizeof(cust_plp_prbs_status_t)*PRBS_MAX_LANE_NUM);
    memset(func_name, 0, sizeof(func_name));
    strncpy(func_name, diag_func_name, strlen(diag_func_name));
    if (inited != 1) {
        INIT_LIST_HEAD(&g_prbsstat_info_list_head);
        inited = 1;
    }
    p = diag_params[param_i];
    while ((p != NULL) && (param_i < n_param)) {
        if (!strncmp(p, "speed=", strlen("speed="))) {
            speed = _parse_param(p);
        } else if (!strncmp(p, "en=", strlen("en="))) {
            enable = _parse_param(p);
        } else if (!strncmp(p, "enable=", strlen("enable="))) {
            enable = _parse_param(p);
        } else if (!strncmp(p, "if_side=", strlen("if_side="))) {
            /* sys=1 line=0 */
            if_side = _parse_param(p);
        } else if (!strncmp(p, "lane=", strlen("lane="))) {
            /* bcm: begin with 1 */
            lane = _parse_param(p);
        } else if (!strncmp(p, "poly=", strlen("poly="))) {
            poly = _parse_param(p);
        } else if (!strncmp(p, "lb_dir=", strlen("lb_dir="))) {
            /* 1 - Digital PMD loopback
               2 - Remote PMD loopback
               3 - Analog internal loopback */
            lb_dir = _parse_param(p);
        } else if (!strncmp(p, "flag=", strlen("flag="))) {
            flag = _parse_param(p);
        } else if (!strncmp(p, "level=", strlen("level="))) {
            level = _parse_param(p);
        } else if (!strncmp(p, "tx_rx=", strlen("tx_rx="))) {
            /* tx=0 rx=1 */
            tx_rx = _parse_param(p);
        } else if (!strncmp(p, "override=", strlen("override="))) {
            override = (bool)_parse_param(p);
        } else if (!strncmp(p, "polarity=", strlen("polarity="))) {
            polarity = _parse_param(p);
        } else if (!strncmp(p, "inv=", strlen("inv="))) {
            inv = _parse_param(p);
        } else if (!strncmp(p, "pre3=", strlen("pre3="))) {
            tx_fir.pre3 = _parse_param(p);
        } else if (!strncmp(p, "pre2=", strlen("pre2="))) {
            tx_fir.pre2 = _parse_param(p);
        } else if (!strncmp(p, "pre=", strlen("pre="))) {
            tx_fir.pre = _parse_param(p);
        } else if (!strncmp(p, "main=", strlen("main="))) {
            tx_fir.main = _parse_param(p);
        } else if (!strncmp(p, "post=", strlen("post="))) {
            tx_fir.post = _parse_param(p);
        } else if (!strncmp(p, "post2=", strlen("post2="))) {
            tx_fir.post2 = _parse_param(p);
        } else if (!strncmp(p, "post3=", strlen("post3="))) {
            tx_fir.post3 = _parse_param(p);
        } else if (!strncmp(p, "nr_er=", strlen("nr_er="))) {
            nr_er = _parse_param(p);
        } else if (!strncmp(p, "devaddr=", strlen("devaddr="))) {
            devaddr = _parse_param(p);
        } else if (!strncmp(p, "regaddr=", strlen("regaddr="))) {
            regaddr = _parse_param(p);
        } else if (!strncmp(p, "data64=", strlen("data64="))) {
            data_64 = _parse_param(p);
        }
        p = diag_params[++param_i];
    }
    if (!strcmp(func_name, "speed_set")) {
        rv = cust_plp_port_speed_set(unit, port, speed);
    } else if (!strcmp(func_name, "psh")) {
        shell_cmd = diag_params[0];
        if (shell_cmd) {
            rv = cust_plp_shell_cmd_run(unit, port, shell_cmd);
        }
    } else if (!strcmp(func_name, "dbg_on")) {
        cust_plp_log_func_reg(g_cust_debug_log);
        g_cli_debug_mode = true;
        PLP_CLI_DEBUG("DEBUG ON");
    } else if (!strcmp(func_name, "dbg_off")) {
        PLP_CLI_DEBUG("DEBUG OFF");
        g_cli_debug_mode = false;
        cust_plp_log_func_reg(NULL);
    } else if (!strcmp(func_name, "prbs_set")) {
        PLP_CLI_DEBUG("cust_plp_port_prbs_set(unit=%d, port=%d, poly=%d, inv=%d, if_side=%d, lane=%d)", 
            unit, port, poly, inv, if_side, lane);
        rv = cust_plp_port_prbs_set(unit, port, poly, inv, if_side, lane);
    } else if (!strcmp(func_name, "prbs_get")) {
        PLP_CLI_DEBUG("cust_plp_port_prbs_get(unit=%d, port=%d, if_side=%d)", 
            unit, port, if_side);
        rv = cust_plp_port_prbs_get(unit, port, if_side, prbs_info);
        cust_debug_prbsstat_update(unit, port, if_side);
        for (i = 0; i < PRBS_MAX_LANE_NUM; i++) {
            if (prbs_info[i].is_get) {
                PLP_CLI("port: %d, lane: %d, side: %d, prbs_lock: %d, lock_loss: %d, lane_err_cnt: %d", port, i, if_side,
                prbs_info[i].prbs_lock, prbs_info[i].lock_loss, prbs_info[i].lane_err_cnt);
            }
        }
    } else if (!strcmp(func_name, "prbs_clear")) {
        PLP_CLI_DEBUG("cust_plp_port_prbs_clear(unit=%d, port=%d, if_side=%d)", 
            unit, port, if_side);
        if (cust_debug_prbsstat_clear(unit, port, if_side) != 0) {
            PLP_CLI("cust_debug_prbsstat_clear failed!");
        }
        rv = cust_plp_port_prbs_clear(unit, port, if_side);
    } else if (!strcmp(func_name, "prbsstat_start")) {
        PLP_CLI_DEBUG("cust_debug_prbsstat_update(unit=%d, port=%d, if_side=%d)", 
            unit, port, if_side);
        rv = cust_debug_prbsstat_update(unit, port, if_side);
    } else if (!strcmp(func_name, "prbsstat_ber")) {
        PLP_CLI_DEBUG("cust_debug_prbsstat_ber(unit=%d, port=%d, if_side=%d)", 
            unit, port, if_side);
        rv = cust_debug_prbsstat_ber(unit, port, if_side);
    } else if (!strcmp(func_name, "prbsstat_clear")) {
        PLP_CLI_DEBUG("cust_debug_prbsstat_clear(unit=%d, port=%d, if_side=%d)", 
            unit, port, if_side);
        rv = cust_debug_prbsstat_clear(unit, port, if_side);
    } else if (!strcmp(func_name, "squelch_set")) {
        PLP_CLI_DEBUG("cust_plp_squelch_set(unit=%d, port=%d, if_side=%d, tx_rx=%d, enable=%d, lane=%d)", 
            unit, port, if_side, tx_rx, enable, lane);
        rv = cust_plp_squelch_set(unit, port, if_side, tx_rx, enable, lane);
    } else if (!strcmp(func_name, "loopback_set")) {
        PLP_CLI_DEBUG("cust_plp_loopback_set(unit=%d, port=%d, if_side=%d, lb_dir=%d, enable=%d)", 
            unit, port, if_side, lb_dir, enable);
        rv = cust_plp_loopback_set(unit, port, if_side, lb_dir, enable);
    } else if (!strcmp(func_name, "loopback_get")) {
        if (lb_dir == 0) {
            for (lb_dir =1; lb_dir <=3; lb_dir++) {
                rv = cust_plp_loopback_get(unit, port, if_side, lb_dir, &enable);
                PLP_CLI("%s, %s loopback: %s, rv: %d", ((if_side == PLP_SYS_IF_SIDE) ? "system side" : "line side"), 
                            (lb_dir == 1) ? "Digital PMD" : ((lb_dir == 2) ? "Remote PMD" : "Analog internal"), ((enable) ? "enable" : "disable"), rv);
            }
        } else {
            rv = cust_plp_loopback_get(unit, port, if_side, lb_dir, &enable);
            PLP_CLI("%s, %s loopback: %s", ((if_side == PLP_SYS_IF_SIDE) ? "system side" : "line side"), 
                        (lb_dir == 1) ? "Digital PMD" : ((lb_dir == 2) ? "Remote PMD" : "Analog internal"), ((enable) ? "enable" : "disable"));
        }
    } else if (!strcmp(func_name, "dsc")) {
        rv = cust_plp_dsc_dump(unit, port, if_side, flag, lane);
    } else if (!strcmp(func_name, "phy_status")) {
        rv = cust_plp_phy_status_dump(unit, port, level);
    } else if (!strcmp(func_name, "polarity_set")) {
        PLP_CLI_DEBUG("cust_plp_polarity_set(unit=%d, port=%d, if_side=%d, tx_rx=%d, polarity=0x%x, override=%d)", 
            unit, port, if_side, tx_rx, polarity, override);
        rv = cust_plp_polarity_set(unit, port, if_side, tx_rx, polarity, override);
    } else if (!strcmp(func_name, "polarity_get")) {
        rv = cust_plp_polarity_get(unit, port, if_side, tx_rx, &polarity);
        if (rv == PLP_STATUS_SUCCESS) {
            PLP_CLI("unit: %d, port: %d, %s_%s_polarity: 0x%x", unit, port, (if_side==1)? "sys": "line", (tx_rx==PLP_TX_DIR)? "tx": "rx", polarity);
        }
    } else if (!strcmp(func_name, "temperature_get")) {
        rv = cust_plp_temperature_get(unit, port, &temp);
        PLP_CLI("temperature: %f", temp);
    } else if (!strcmp(func_name, "highest_temperature_get")) {
        rv = cust_plp_highest_temperature_get(&temp);
        PLP_CLI("highest_temperature: %f", temp);
    } else if (!strcmp(func_name, "phy_reset")) {
        rv = cust_plp_phy_reset(unit, port, flag);
    } else if (!strcmp(func_name, "linktrain_set")) {
        rv = cust_plp_port_linktrain_set(unit, port, if_side, enable);
    } else if (!strcmp(func_name, "tx_fir_set")) {
        PLP_CLI_DEBUG("cust_plp_tx_fir_set(unit=%d, port=%d, if_side=%d, lane=%d, tx_fir(pre3=%d, pre2=%d, pre=%d, main=%d, post=%d, post2=%d, post3=%d))", 
                unit, port, if_side, lane, tx_fir.pre3, tx_fir.pre2, tx_fir.pre, tx_fir.main, tx_fir.post, tx_fir.post2, tx_fir.post3);
        rv = cust_plp_tx_fir_set(unit, port, if_side, lane, tx_fir);
    } else if (!strcmp(func_name, "tx_fir_get")) {
        PLP_CLI_DEBUG("cust_plp_tx_fir_get(unit=%d, port=%d, if_side=%d, lane=%d)", unit, port, if_side, lane);
        rv = cust_plp_tx_fir_get(unit, port, if_side, lane, &tx_fir);
        PLP_CLI("port: %d, pre3=%d, pre2=%d pre=%d main=%d post=%d post2=%d post3=%d", 
                 port, tx_fir.pre3, tx_fir.pre2, tx_fir.pre, tx_fir.main, tx_fir.post, tx_fir.post2, tx_fir.post3);
    } else if (!strcmp(func_name, "setreg")) {
        PLP_CLI_DEBUG("plp_phy_dbg_set_reg(unit=%d, port=%d, if_side=%d, devaddr=0x%x, regaddr=0x%x, flag=%d, data=0x%llx)", 
                unit, port, if_side, devaddr, regaddr, flag, data_64);
        rv = cust_plp_reg_set(unit, port, if_side, devaddr, regaddr, flag, data_64);
    } else if (!strcmp(func_name, "getreg")) {
        PLP_CLI_DEBUG("plp_phy_dbg_get_reg(unit=%d, port=%d, if_side=%d, devaddr=0x%x, regaddr=0x%x, flag=%d)", 
                unit, port, if_side, devaddr, regaddr, flag);
        rv = cust_plp_reg_get(unit, port, if_side, devaddr, regaddr, flag, &data_64);
        PLP_CLI("devaddr=0x%x, regaddr=0x%x, data=0x%llx", devaddr, regaddr, data_64);
    } else if (!strcmp(func_name, "techsupport")) {
        cust_plp_techsupport(unit, port);
    } else if (!strcmp(func_name, "show_techsupport")) {
        cust_plp_techsupport(unit, -1);
    } else if (!strcmp(func_name, "mib_dump")) {
        rv = cust_plp_mib_dump(unit, port, if_side);
    } else if (!strcmp(func_name, "channel_reach")) {
        rv = cust_plp_channel_reach_set(unit, port, if_side, lane, nr_er);
    } else if (!strcmp(func_name, "version")) {
        memset(&version_info, 0, sizeof(version_info));
        rv = cust_plp_version_get(unit, port, &version_info);
        if (rv == PLP_STATUS_SUCCESS) {
            PLP_CLI("phy_id:0x%x, fw_ver:0x%x fw_crc:0x%x (rev_id:0x%x), drv_ver:%s.%d.%d (built %s %s)", 
                version_info.phy_id, version_info.fw_ver, version_info.fw_crc, version_info.rev_id, version_info.chip_name, 
                version_info.drv_major_ver, version_info.drv_minor_ver, __DATE__, __TIME__);
        }
    } else {
        PLP_CLI("Invalid command %s!", func_name);
        dump_help_msg();
    }
    if (rv != PLP_STATUS_NOT_SUPPORTED_PORT) {
        PLP_CLI("unit: %d, port: %d, %s %s, rv: %d", unit, port, func_name, (rv == PLP_STATUS_SUCCESS) ? "success" : "failed", rv);
    }
    return;
}

// void cust_plp_cli_log_reg(cust_cli_log_func_t cli_log_func)
// {
//     g_cli_log_func = cli_log_func;
// }

void cust_plp_debug_log_reg(cust_plp_log_func_t debug_log_func)
{
    g_cust_debug_log = debug_log_func;
}

int cust_plp_debug_diag(int unit, int port, char** args, int n_arg)
{
    char *diag_func_name;
    if (n_arg <= 0) {
        PLP_CLI("n_arg error, n_arg:%d", n_arg);
        dump_help_msg();
        return -1;
    }
    diag_func_name = args[0];
    if (diag_func_name == NULL) {
        PLP_CLI("diag_func_name is missing!");
        dump_help_msg();
        return -1;
    }
    if (!strcmp(diag_func_name, "help")) {
        dump_help_msg();
        return -1;
    }
    diag_phy_control(unit, port, diag_func_name, &args[1], (n_arg - 1));
    fflush(NULL);
    return 0;
}

#ifdef EXTPHY_BINARY
#include "credo/shell.h"
#define MAX_DEBUG_STR_LEN 256
#define MAX_DEBUG_ARG_NUM 16
int main(void)
{
    char cString[MAX_DEBUG_STR_LEN];
    char tmp[MAX_DEBUG_STR_LEN];
    char delim[] = " ";

    cust_plp_platform_init(0, false);
    do {
        int unit, port, n_arg;
        char *args[MAX_DEBUG_ARG_NUM];

        cust_plp_log_debug("debug>");
        fgets(tmp, MAX_DEBUG_STR_LEN, stdin);
        if (strlen(tmp) == 1) {
            continue;
        } else {
            snprintf(cString, strlen(tmp), "%s", tmp);
        }
        cust_plp_log_debug("%s\n", cString);
        if (!strcmp(cString, "exit")) {
            break;
        } else if (!strcmp(cString, "ps")) {
            cust_plp_log_debug("TODO\n");
            continue;
        } else if (!strcmp(cString, "cr")) {
            cust_plp_log_debug("cr_shell exit: %d\n", cr_shell_spawn_enhanced());
            continue;
        } else if (!strcmp(cString, "show techsupport")) {
            cust_plp_techsupport(0, -1);
            continue;
        }

        n_arg = 0;
        char *saveptr;
        char *token = strtok_r(cString, delim, &saveptr);
        while (token != NULL) {
            if (n_arg == 0) {
                unit = strtol(token, NULL, 10);
            }
            if (n_arg == 1) {
                port = strtol(token, NULL, 10);
            }
            if (n_arg >= 2) {
                args[n_arg-2] = token;
            }

            token = strtok_r(NULL, delim, &saveptr);
            n_arg++;
        }
        if (n_arg > 2) {
            cust_plp_debug_diag(unit, port, args, n_arg-2);
        } else {
            cust_plp_log_debug("invalid cmd\n");
            dump_help_msg();
        }
    } while (1);
}
#endif