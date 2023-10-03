#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "src/lh_action.h"
#include "src/lh_consts.h"
#include "src/lh_start.h"
#include "src/lh_error.h"

int main(int argc, const char* argv[]) {
    lh_action_t act(argc, argv); // init command

    if (act.verb == START_VERB) { // start
        lh_start_cmdline(act);
    }
}