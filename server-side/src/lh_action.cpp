#include <bits/stdc++.h>

#include "lh_action.h"
#include "lh_consts.h"
#include "lh_error.h"

lh_action_t::options_t::options_t() {
    echo_service = false;
    print_service = false;
    tcp_listener = true;
    udp_listener = false;
}

lh_action_t::options_t::options_t(bool filled) {
    echo_service = filled;
    print_service = filled;
    tcp_listener = filled;
    udp_listener = filled;
}

lh_action_t::lh_action_t():
    options(false), verb(UNDEFINED_VERB), port(0) {
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

    for (int i = 3; i < argc; )
        i += add_option(argc, argv, i);
}

int lh_action_t::add_option(int argc, const char* argv[], int now) {
    const char* op = argv[now];
    
    if (!strcmp(op, "-e") || !strcmp(op, "--echo")) {
        puts("echo service enabled");
        options.echo_service = true;
        return 1;
    }
    if (!strcmp(op, "-ne") || !strcmp(op, "--necho")) {
        puts("echo service disabled ");
        options.echo_service = false;
        return 1;
    }
    if (!strcmp(op, "-p") || !strcmp(op, "--print")) {
        puts("print service enabled");
        options.print_service = true;
        return 1;
    }
    if (!strcmp(op, "-np") || !strcmp(op, "--nprint")) {
        puts("print service disabled");
        options.print_service = false;
        return 1;
    }
    if (!strcmp(op, "--tcp")) {
        puts("tcp protocol enabled");
        options.tcp_listener = true;
        return 1;
    }
    if (!strcmp(op, "--ntcp")) {
        puts("tcp protocol disabled");
        options.tcp_listener = false;
        return 1;
    }
    if (!strcmp(op, "--udp")) {
        puts("udp protocol enabled");
        options.udp_listener = true;
        return 1;
    }
    if (!strcmp(op, "--nudp")) {
        puts("udp protocol disabled");
        options.udp_listener = false;
        return 1;
    }

    static char buf[BUF_SIZE];
    sprintf(buf, "ignored option \'%s\'", op);
    lh_war(buf);
    return 1;
}

void lh_action_t::set_to(const lh_action_t& to) {
    options.echo_service = to.options.echo_service;
    options.print_service = to.options.print_service;
    options.tcp_listener = to.options.tcp_listener;
    options.udp_listener = to.options.udp_listener;

    verb = to.verb;
    port = to.port;
}