#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <stdint.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>
#include "cust_plp_common.h"

#define PRINT_LEN 1024
#define PLP_LOG_FILE_DIR    "/var/log/saplog"
#define PLP_CFG_FILE_LINE_MAX   (1000)

#define CMD_MDIO_READ   _IOR('M', 1, struct mdio_dev_user_info)
#define CMD_MDIO_WRITE  _IOR('M', 2, struct mdio_dev_user_info)

static cust_plp_log_func_t g_cust_log_func = NULL;
static mdio_read_func g_mdio_read_funcs[PLP_MDIO_TYPE_MAX];
static mdio_write_func g_mdio_write_funcs[PLP_MDIO_TYPE_MAX];

struct mdio_dev_user_info {
    int mdio_index;
    int phyaddr;
    uint32_t regaddr;
    uint32_t regval;
};

static FILE* plp_log_file = NULL;
pthread_mutex_t mutex;
static int mutex_inited = 0;
static char time_buf[PRINT_LEN] = {0};
char* getCurrentTime()
{
    if (mutex_inited == 0) {
        mutex_inited = 1;
        pthread_mutex_init(&mutex, NULL);
    }
    pthread_mutex_lock(&mutex);
    time_t timetemp;
    struct tm *p;
    time(&timetemp);
    p = localtime(&timetemp);
    sprintf(time_buf, "%d-%d-%d %d:%02d:%02d", p->tm_year+1900, p->tm_mon + 1, p->tm_mday, 
                p->tm_hour, p->tm_min, p->tm_sec);

    pthread_mutex_unlock(&mutex);
    return time_buf;
}

void cust_plp_log_debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (g_cust_log_func != NULL) {
        g_cust_log_func(format, args);
        va_start(args, format);
    }
#if CUST_PLP_LOG_LEVEL < CUST_PLP_LOG_LEVEL_DEBUG
    return;
#endif
#ifdef EXTPHY_BINARY
    if (g_cust_log_func == NULL) {
        vprintf(format, args);
    }
#else
    vfprintf(plp_log_file, format, args);
    fflush(plp_log_file);
    va_end(args);
#endif
}

void cust_plp_log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (g_cust_log_func != NULL) {
        g_cust_log_func(format, args);
        va_start(args, format);
    }
#if CUST_PLP_LOG_LEVEL < CUST_PLP_LOG_LEVEL_INFO
    return;
#endif
#ifdef EXTPHY_BINARY
    if (g_cust_log_func == NULL) {
        vprintf(format, args);
    }
#else
    vfprintf(plp_log_file, format, args);
    fflush(plp_log_file);
    va_end(args);
#endif
}

void cust_plp_log_notice(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (g_cust_log_func != NULL) {
        g_cust_log_func(format, args);
        va_start(args, format);
    }
#if CUST_PLP_LOG_LEVEL < CUST_PLP_LOG_LEVEL_NOTICE
    return;
#endif
#ifdef EXTPHY_BINARY
    if (g_cust_log_func == NULL) {
        vprintf(format, args);
    }
#else
    vfprintf(plp_log_file, format, args);
    fflush(plp_log_file);
    va_end(args);
#endif
}

void cust_plp_log_warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (g_cust_log_func != NULL) {
        g_cust_log_func(format, args);
        va_start(args, format);
    }
#if CUST_PLP_LOG_LEVEL < CUST_PLP_LOG_LEVEL_WARN
    return;
#endif
#ifdef EXTPHY_BINARY
    if (g_cust_log_func == NULL) {
        vprintf(format, args);
    }
#else
    vfprintf(plp_log_file, format, args);
    fflush(plp_log_file);
    va_end(args);
#endif
}

void cust_plp_log_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (g_cust_log_func != NULL) {
        g_cust_log_func(format, args);
        va_start(args, format);
    }
#if CUST_PLP_LOG_LEVEL < CUST_PLP_LOG_LEVEL_ERROR
    return;
#endif
#ifdef EXTPHY_BINARY
    if (g_cust_log_func == NULL) {
        vprintf(format, args);
    }
#else
    vfprintf(plp_log_file, format, args);
    fflush(plp_log_file);
    va_end(args);
#endif
}

void cust_plp_log_func_reg(cust_plp_log_func_t log_func)
{
    g_cust_log_func = log_func;
}

static void _c2int(char *str, int *val)
{
    int base, flag;
    if (str == NULL) {
        PLP_LOG_ERR("in parameter check fail, exist nullpointer");
        return;
    }
    if (val == NULL) {
        PLP_LOG_ERR("in parameter check fail, exist nullpointer");
        return;
    }
    base = 0;
    flag = 1;
    if (val) {
        if (*str == '-') {
            flag = -1;
            str += 1;
        }
        if (*str == '0') {
            if (str[1] == 'b' || str[1] == 'B') {
                base = 2;
                str += 2;
            } else if (str[1] == 'x' || str[1] == 'X') {
                base = 16;
                str += 2;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
        *val = flag * (int)strtol(str, NULL, base);
    }
}

int _parse_param(char *str)
{
    char *param_val;
    int value;
    value = 0;
    param_val = strrchr(str, '=');
    param_val++;
    _c2int(param_val, &value);
    return value;
}

int plp_get_cfg_info(char *file_path, char *config_buf, int *val, bool is_arr)
{
    int i;
    FILE *fp = NULL;
    char *sp = NULL;
    char *find = NULL;
    char *buf = NULL;
    char *out_buf = NULL;
    char config_line[PLP_CFG_FILE_LINE_MAX] = {0};
    char config_prefix[PLP_CFG_FILE_LINE_MAX] = {0};
    char *outer_ptr = NULL;
    char *inner_ptr = NULL;
    int arr_s, arr_e;
    if ((fp = fopen((char *)file_path, "r")) == NULL) {
        PLP_LOG_ERR("%s open fail.", file_path);
        return PLP_STATUS_FAILURE;
    }
    memset(config_line, 0, sizeof(config_line));
    memset(config_prefix, 0, sizeof(config_prefix));

    snprintf(config_prefix, sizeof(config_prefix), "%s=", config_buf);
    while (fgets(config_line, PLP_CFG_FILE_LINE_MAX, fp) != NULL) {
        if (config_line[0] == '#' || config_line[0] == '\r'
            || config_line[0] == '\n' || config_line[0] == ' ') {
            continue;
        }
        find = strchr(config_line, '\n');
        if (find) {
            *find = '\0';
        }
        find = strchr(config_line, '\r');
        if (find) {
            *find = '\0';
        }
        if (strncmp(config_line, config_prefix, strlen(config_prefix)) != 0) {
            continue;
        }
        if (is_arr == true) {
            buf = config_line;
            sp = strtok_r(buf, "=", &out_buf);
            if (sp == NULL) {
                continue;
            }
            outer_ptr = NULL;
            inner_ptr = NULL;
            while ((buf = strtok_r(out_buf, ",", &outer_ptr)) != NULL)
            {
                find = strchr(buf, '-');
                if (find && (find != buf)) {
                    sp = strtok_r(buf, "-", &inner_ptr);
                    _c2int(sp, &arr_s);
                    _c2int(inner_ptr, &arr_e);
                    for (i = arr_s; i<=arr_e; i++) {
                        *(val++) = i;
                    }
                } else {
                    _c2int(buf, val++);
                }
                out_buf = outer_ptr;
            }
        } else {
            char *temp = config_line;
            sp = strsep(&temp, "=");
            _c2int(temp, val);
        }
        fclose(fp);
        return PLP_STATUS_SUCCESS;
    }
    fclose(fp);
    PLP_LOG_DBG("get %s empty", config_buf);
    return PLP_STATUS_ITEM_NOT_FOUND;
}

int plp_get_cfg_info_str(char *file_path, char *config_buf, char *val)
{
    FILE *fp = NULL;
    char *sp = NULL;
    char *find = NULL;
    char config_line[PLP_CFG_FILE_LINE_MAX] = {0};
    char config_prefix[PLP_CFG_FILE_LINE_MAX] = {0};

    if ((fp = fopen((char *)file_path, "r")) == NULL) {
        PLP_LOG_DBG("%s open fail", file_path);
        return PLP_STATUS_FAILURE;
    }
    memset(config_line, 0, sizeof(config_line));
    memset(config_prefix, 0, sizeof(config_prefix));

    snprintf(config_prefix, sizeof(config_prefix), "%s=", config_buf);
    while (fgets(config_line, PLP_CFG_FILE_LINE_MAX, fp) != NULL) {
        if (config_line[0] == '#' || config_line[0] == '\r'
            || config_line[0] == '\n' || config_line[0] == ' ') {
            continue;
        }
        find = strchr(config_line, '\n');
        if (find) {
            *find = '\0';
        }
        find = strchr(config_line, '\r');
        if (find) {
            *find = '\0';
        }
        if (strncmp(config_line, config_prefix, strlen(config_prefix)) == 0) {
            sp = strtok(config_line, "=");
            sp = strtok(NULL, "=");
            // PLP_LOG_DBG("value : %s\n", sp);
            memcpy(val, sp, strlen(sp));
            fclose(fp);
            return PLP_STATUS_SUCCESS;
        }
    }
    fclose(fp);
    return PLP_STATUS_ITEM_NOT_FOUND;
}

static int dfd_utest_mdiodev_rd(int mdio_index, int phy_addr, uint32_t regaddr, uint32_t *regval)
{
    struct mdio_dev_user_info mdio_info;
    long int ret;
    int fd;
    mdio_info.mdio_index = mdio_index;
    mdio_info.phyaddr = phy_addr;
    mdio_info.regaddr = regaddr | 0x40000000;
    mdio_info.regval = 0;
    fd = open("/dev/dram_test", O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd < 0) {
        fprintf(stderr, "Error: Could not open file "
                "/dev/dram_test: %s\n", strerror(errno));
        return -1;
    }
    ret = ioctl(fd, CMD_MDIO_READ, &mdio_info);
    if (ret < 0) {
        fprintf(stderr, "Error: mdio read error : %s\n", strerror(errno));
        close(fd);
        return  -1;
    }
    close(fd);
    *regval = mdio_info.regval;
    return 0;
}

static int dfd_utest_mdiodev_wr(int mdio_index, int phy_addr, uint32_t regaddr, uint32_t regval)
{
    struct mdio_dev_user_info mdio_info;
    long int ret;
    int fd;

    mdio_info.mdio_index = mdio_index;
    mdio_info.phyaddr = phy_addr;
    mdio_info.regaddr = regaddr | 0x40000000;
    mdio_info.regval = regval;
    fd = open("/dev/dram_test", O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd < 0) {
        fprintf(stderr, "Error: Could not open file "
                "/dev/dram_test: %s\n", strerror(errno));
        return -1;
    }
    ret = ioctl(fd, CMD_MDIO_WRITE, &mdio_info);
    if (ret < 0) {
        fprintf(stderr, "Error: mdio read error : %s\n", strerror(errno));
        close(fd);
        return  -1;
    }
    close(fd);
    return 0;
}

// static int dfd_mdio_read(void *user_acc, unsigned int phy_addr, unsigned int reg_addr, unsigned int *data)
// {
//     int mdio_index;
//     int bus_id = 2;
//     mdio_index = ((phy_addr & 0xe0)>>5) + ((phy_addr & 0xf00) >> 6) + bus_id;
//     phy_addr = phy_addr & 0x1f;
//     return dfd_utest_mdiodev_rd(1, phy_addr, reg_addr, data);
// }

// static int dfd_mdio_write(void *user_acc, unsigned int phy_addr, unsigned int reg_addr, unsigned int data)
// {
//     int mdio_index;
//     int bus_id = 2;
//     mdio_index = ((phy_addr & 0xe0)>>5) + ((phy_addr & 0xf00) >> 6) + bus_id;
//     phy_addr = phy_addr & 0x1f;
//     return dfd_utest_mdiodev_wr(1, phy_addr, reg_addr, data);
// }

static int dfd_mdio_read(void *user_acc, unsigned int phy_addr, unsigned int reg_addr, unsigned int *data)
{
    int mdio_index;
    int bus_id = 2;
    mdio_index = ((phy_addr & 0xe0)>>5) + ((phy_addr & 0xf00) >> 6) + bus_id;
    phy_addr = phy_addr & 0x1f;
    return dfd_utest_mdiodev_rd(mdio_index, phy_addr, reg_addr, data);
}

static int dfd_mdio_write(void *user_acc, unsigned int phy_addr, unsigned int reg_addr, unsigned int data)
{
    int mdio_index;
    int bus_id = 2;
    mdio_index = ((phy_addr & 0xe0)>>5) + ((phy_addr & 0xf00) >> 6) + bus_id;
    phy_addr = phy_addr & 0x1f;
    return dfd_utest_mdiodev_wr(mdio_index, phy_addr, reg_addr, data);
}

static int dfd_mdio_read_cust(void *user_acc, unsigned int phy_addr, unsigned int reg_addr, unsigned int *data)
{
    int mdio_index, bus_id;
    bus_id = (phy_addr & 0x7c00)>>10;
    mdio_index = (phy_addr & 0x3e0)>>5;
    phy_addr = phy_addr & 0x1f;
    return dfd_utest_mdiodev_rd(mdio_index+bus_id, phy_addr, reg_addr, data);
}

static int dfd_mdio_write_cust(void *user_acc, unsigned int phy_addr, unsigned int reg_addr, unsigned int data)
{
    int mdio_index, bus_id;
    bus_id = (phy_addr & 0x7c00)>>10;
    mdio_index = (phy_addr & 0x3e0)>>5;
    phy_addr = phy_addr & 0x1f;
    return dfd_utest_mdiodev_wr(mdio_index+bus_id, phy_addr, reg_addr, data);
}

int plp_get_mdio_read_func(plp_mdio_type_e type, mdio_read_func *func)
{
    if (type >= PLP_MDIO_TYPE_MAX) {
        return PLP_STATUS_FAILURE;
    }
    *func = g_mdio_read_funcs[type];
    if (*func == NULL) {
        return PLP_STATUS_FAILURE;
    }
    return PLP_STATUS_SUCCESS;
}

int plp_get_mdio_write_func(plp_mdio_type_e type, mdio_write_func *func)
{
    if (type >= PLP_MDIO_TYPE_MAX) {
        return PLP_STATUS_FAILURE;
    }
    *func = g_mdio_write_funcs[type];
    if (*func == NULL) {
        return PLP_STATUS_FAILURE;
    }
    return PLP_STATUS_SUCCESS;
}

int cust_plp_mdio_func_reg(plp_mdio_type_e type, mdio_read_func read_func, mdio_write_func write_func)
{
    if (type >= PLP_MDIO_TYPE_MAX) {
        return PLP_STATUS_FAILURE;
    }
    if (read_func != NULL) {
        g_mdio_read_funcs[type] = read_func;
    }
    if (write_func != NULL) {
        g_mdio_write_funcs[type] = write_func;
    }
    return PLP_STATUS_SUCCESS;
}

char* _dump_int_array(int *input, int input_size, char *output, int output_size)
{
    int i;
    char number_str[32];
    memset(output, 0, output_size);
    for (i = 0; i<input_size; i++) {
        if (i != 0) {
            strncat(output, ",", strlen(output));
        }
        snprintf(number_str, sizeof(number_str), "%d", input[i]);
        strncat(output, number_str, strlen(number_str));
    }
    return output;
}

int tx_fir_cfg_parse(char *cfg_file, int phy_addr, int if_side, int lane, int serdes_mode, cust_plp_medium_e medium, cust_plp_tx_fir_t *tx_fir)
{
    int rv;
    int vals[PLP_CFG_FILE_ARR_MAX];
    char config_prefix[PLP_CFG_FILE_STR_MAX];
    char serdes_mode_string[16];
    char dump_string[PLP_CFG_FILE_STR_MAX];

    if (tx_fir == NULL) {
        return PLP_STATUS_INVALID_PARAMETER;
    }
    switch (serdes_mode) {
    case TX_MODE_PAM4_6TAP:
        snprintf(serdes_mode_string, sizeof(serdes_mode_string), "%s", "pam4_6tap");
        break;
    case TX_MODE_PAM4_LP_3TAP:
        snprintf(serdes_mode_string, sizeof(serdes_mode_string), "%s", "pam4_3tap");
        break;
    case TX_MODE_NRZ_6TAP:
        snprintf(serdes_mode_string, sizeof(serdes_mode_string), "%s", "nrz_6tap");
        break;
    case TX_MODE_NRZ_LP_3TAP:
        snprintf(serdes_mode_string, sizeof(serdes_mode_string), "%s", "nrz_3tap");
        break;
    default:
        return PLP_STATUS_INVALID_PARAMETER;
    }
    snprintf(config_prefix, sizeof(config_prefix), "serdes_pree_%s_lane%d_%s:0x%x", 
                            (if_side == PLP_SYS_IF_SIDE)?"sys":"line", lane, serdes_mode_string, phy_addr);
    rv = plp_get_cfg_info(cfg_file, config_prefix, vals, true);
    if (rv != PLP_STATUS_SUCCESS) {
        // PLP_LOG_NOTICE("%s get empty.", config_prefix);
        return PLP_STATUS_ITEM_NOT_FOUND;
    }
    switch (serdes_mode) {
    case TX_MODE_PAM4_6TAP:
    case TX_MODE_NRZ_6TAP:
        PLP_LOG_DBG("%s: %s", config_prefix, _dump_int_array(vals, 7, dump_string, sizeof(dump_string)));
        tx_fir->pre3  = vals[0];
        tx_fir->pre2  = vals[1];
        tx_fir->pre   = vals[2];
        tx_fir->main  = vals[3];
        tx_fir->post  = vals[4];
        tx_fir->post2 = vals[5];
        tx_fir->post3 = vals[6];
        break;
    case TX_MODE_PAM4_LP_3TAP:
    case TX_MODE_NRZ_LP_3TAP:
        break;
    }
    return PLP_STATUS_SUCCESS;
}

char *getCurrentTime2(char *buffer, size_t bufferSize) {
    time_t current_time;
    struct tm *local_time;
    const char *format = "%Y_%m_%d_%H_%M_%S";
    // 获取当前时间戳
    current_time = time(NULL);
    if (current_time == (time_t)-1) {
        // 处理错误，返回NULL
        return NULL;
    }
    // 转换为本地时间
    local_time = localtime(&current_time);
    if (local_time == NULL) {
        return NULL;
    }
    // 格式化时间字符串
    size_t len = strftime(buffer, bufferSize, format, local_time);
    if (len == 0) {
        return NULL;
    }
    return buffer;
}

int cust_plp_common_init(int unit, bool warm_boot)
{
    if (g_mdio_read_funcs[PLP_MDIO_TYPE_SYSFS] == NULL) {
        g_mdio_read_funcs[PLP_MDIO_TYPE_SYSFS] = dfd_mdio_read;
    }
    if (g_mdio_write_funcs[PLP_MDIO_TYPE_SYSFS] == NULL) {
        g_mdio_write_funcs[PLP_MDIO_TYPE_SYSFS] = dfd_mdio_write;
    }
    if (g_mdio_read_funcs[PLP_MDIO_TYPE_SYSFS_CUST] == NULL) {
        g_mdio_read_funcs[PLP_MDIO_TYPE_SYSFS_CUST] = dfd_mdio_read_cust;
    }
    if (g_mdio_write_funcs[PLP_MDIO_TYPE_SYSFS_CUST] == NULL) {
        g_mdio_write_funcs[PLP_MDIO_TYPE_SYSFS_CUST] = dfd_mdio_write_cust;
    }
#ifndef EXTPHY_BINARY
    char log_path_buffer[128] = PLP_LOG_FILE_DIR;
    char buffer[20]; // 确保有足够的空间存储时间字符串
    char *time_str = getCurrentTime2(buffer, sizeof(buffer));

    if (time_str != NULL) {
        strncat(log_path_buffer, "_", 2);
        strncat(log_path_buffer, time_str, strlen(time_str));
        printf("Current time: %s\n", time_str);
    } else {
        printf("Failed to get current time.\n");
    }
    plp_log_file = fopen(log_path_buffer, "w+");
    if (plp_log_file == NULL) {
        PLP_LOG_ERR("open plp_log_file failed");
        return PLP_STATUS_FAILURE;
    }
#endif
    return PLP_STATUS_SUCCESS;
}