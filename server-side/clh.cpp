#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "src/lh_action.h"
#include "src/lh_consts.h"
#include "src/lh_start.h"
#include "src/lh_error.h"

int main(int argc, const char* argv[]) {
    lh_action_t act(argc, argv); // analyze command

    if (act.verb == START_VERB) {
        if (act.options.mode == ECHO_MODE)
            lh_start_echo_server(act);
        else if (act.options.mode == PRINT_MODE)
            lh_start_print_server(act);
    }
}