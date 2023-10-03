#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "lhc_action.h"
#include "lhc_consts.h"
#include "lhc_error.h"

lh_action_t::options_t::options_t() {
    echo_mode = false;
    udp_mode = false;
}

lh_action_t::lh_action_t(int argc, const char* argv[]): 
    options(), verb(INVALID_VERB), port(0), ip(0) {

    if (argc < 2)
        lh_err("verb not found");

    if (!strcmp(argv[1], "connect")) {
        verb = CONNECT;
        connect_init(argc, argv);
    } else {
        lh_err("invalid verb");
    }
}

void lh_action_t::connect_init(int argc, const char* argv[]) {
    if (argc < 3)
        lh_err("ip not found");
    ip = inet_addr(argv[2]);
    if (ip == INADDR_ANY)
        lh_err("invaild ip");

    if (argc < 4)
        lh_err("port not found");
    port = atoi(argv[3]);
    if (port < 1024 || port > 65535)
        lh_err("invalid port number");
    // printf("port %d\n", port);

    for (int i = 4; i < argc; )
        i += add_option(argc, argv, i);
}

int lh_action_t::add_option(int argc, const char* argv[], int now) {
    const char* op = argv[now];
    
    if (!strcmp(op, "-e") || !strcmp(op, "--echo")) {
        puts("echo mode enabled");
        options.echo_mode = true;
        return 1;
    }
    if (!strcmp(op, "-udp")) {
        puts("using udp connection");
        options.udp_mode = true;
        return 1;
    }
    if (!strcmp(op, "-nudp")) {
        puts("udp connection disabled");
        options.udp_mode = false;
        return 1;
    }

    static char buf[BUF_SIZE];
    sprintf(buf, "ignored option \'%s\'", op);
    lh_war(buf);
    return 1;
}