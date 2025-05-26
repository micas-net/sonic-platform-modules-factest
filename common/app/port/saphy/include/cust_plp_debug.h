#ifndef _CUST_PLP_DBG_H_
#define _CUST_PLP_DBG_H_

#include <stdio.h>
#include "cust_plp_common.h"

typedef int (*cust_plp_diag_func_t)(int unit, int lport, char** args, int n_arg);

int  cust_plp_debug_diag(int unit, int port, char** args, int n_arg);
void cust_plp_debug_log_reg(cust_plp_log_func_t debug_log_func);
// void cust_plp_cli_log_reg(cust_plp_log_func_t cli_log_func);

#endif /* _CUST_PLP_DBG_H_ */