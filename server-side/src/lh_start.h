#ifndef _LH_START_H
#define _LH_START_H

#include "lh_action.h"

void lh_start_echo_service(int clnt_sock);
void lh_start_print_service(int clnt_sock);

void lh_start_tcp_listener(lh_action_t& act);
void lh_start_cmdline(lh_action_t& act);

#endif