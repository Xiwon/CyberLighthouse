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

lh_action_t::lh_action_t(int argc, const char* argv[], const lh_action_t& before): 
    options(before.options), verb(UNDEFINED_VERB), port(before.port) {
    // copy before settings

    if (argc < 2) {
        lh_war("verb not found");
        return;
    }

    if (!strcmp(argv[1], "config")) {
        verb = CONFIG_VERB;
        config_init(argc, argv);
    } else {
        lh_war("invaild verb while server is running");
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

void lh_action_t::config_init(int argc, const char* argv[]) {
    for (int i = 2; i < argc; )
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

int lh_diff_set(bool& b1, const bool& b2, const char* name) {
    if (b1 != b2) {
        printf("[%s]: %d -> %d\n", name, (int)b1, (int)b2);
        b1 = b2;
        return 1;
    }
    return 0;
}

void lh_action_t::set_to(const lh_action_t& to) {
    int chg_num = 0;
    chg_num += lh_diff_set(options.echo_service, to.options.echo_service, "echo service");
    chg_num += lh_diff_set(options.print_service, to.options.print_service, "print service");
    chg_num += lh_diff_set(options.tcp_listener, to.options.tcp_listener, "tcp protocol");
    chg_num += lh_diff_set(options.udp_listener, to.options.udp_listener, "udp protocol");
    port = to.port;
    verb = to.verb;

    if (chg_num == 0)
        puts("no setting had changed");
}