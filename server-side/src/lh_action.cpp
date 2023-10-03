#include <bits/stdc++.h>

#include "lh_action.h"
#include "lh_consts.h"
#include "lh_error.h"

lh_action_t::options_t::options_t() {
    mode = UNDEFINED_MODE;
    protocol = TCP_PROTOCOL;
}

lh_action_t::lh_action_t(int argc, const char* argv[]): 
    options(), verb(UNDEFINED_VERB), port(0) {

    if (argc < 2)
        lh_err("verb not found");

    if (!strcmp(argv[1], "start")) {
        verb = START_VERB;
        start_init(argc, argv);
    } else {
        lh_err("invalid verb");
    }
}

void lh_action_t::start_init(int argc, const char* argv[]) {
    if (argc < 3)
        lh_err("port not found");
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535)
        lh_err("invalid port number");
    // printf("set port %d\n", port);

    for (int i = 3; i < argc; )
        i += add_option(argc, argv, i);
}

int lh_action_t::add_option(int argc, const char* argv[], int now) {
    const char* op = argv[now];
    
    if (!strcmp(op, "-e") || !strcmp(op, "--echo")) {
        puts("echo mode enabled");
        options.mode = ECHO_MODE;
        return 1;
    }
    if (!strcmp(op, "-p") || !strcmp(op, "--print")) {
        puts("print mode enabled");
        options.mode = PRINT_MODE;
        return 1;
    }
    if (!strcmp(op, "--ntcp") || !strcmp(op, "--udp")) {
        puts("using udp protocol at present");
        options.protocol = UDP_PROTOCOL;
        return 1;
    }
    if (!strcmp(op, "--nudp") || !strcmp(op, "--tcp")) {
        puts("using tcp protocol at present");
        options.protocol = TCP_PROTOCOL;
        return 1;
    }

    static char buf[BUF_SIZE];
    sprintf(buf, "ignored option \'%s\'", op);
    lh_war(buf);
    return 1;
}