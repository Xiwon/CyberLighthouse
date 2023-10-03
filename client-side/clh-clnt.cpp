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

    if (act.verb == CONNECT) {
        if (act.options.echo_mode)
            lh_connect_echo_client(act);
    }
}