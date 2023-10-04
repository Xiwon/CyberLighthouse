#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "src/lhc_action.h"
#include "src/lhc_consts.h"
#include "src/lhc_connect.h"
#include "src/lhc_error.h"

int main(int argc, const char* argv[]) {
    lh_action_t act(argc, argv); // analyze command

    if (act.verb == CONNECT_VERB) {
        if (act.options.protocol == TCP_PROTOCOL) {
            if (act.options.mode == ECHO_MODE)
                lh_connect_echo_client(act);
            if (act.options.mode == PRINT_MODE)
                lh_connect_print_client(act);
        }
        if (act.options.protocol == UDP_PROTOCOL) {
            if (act.options.mode == ECHO_MODE)
                lh_connect_udp_echo_client(act); // udp echo client
            if (act.options.mode == PRINT_MODE)
                lh_connect_udp_print_client(act);
        }
    }
}