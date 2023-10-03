#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "lhc_action.h"
#include "lhc_consts.h"
#include "lhc_error.h"

lh_action_t::options_t::options_t() {
    mode = UNDEFINED_MODE;
    protocol = TCP_PROTOCOL;
}

lh_action_t::lh_action_t(int argc, const char* argv[]): 
    options(), verb(UNDEFINED_VERB), port(0), ip(0) {

    if (argc < 2)
        lh_err("verb not found");

    if (!strcmp(argv[1], "connect")) {
        verb = CONNECT_VERB;
        connect_init(argc, argv);
    } else {
        lh_err("invalid verb");
    }
}

int lh_cut_string(const char* s, char div, char* fir, char* sec) {
    int len = strlen(s);

    for (int i = 0; i < len; i++)
        if (s[i] == div) {
            for (int j = 0; j < i; j++) {
                fir[j] = s[j];
            }
            fir[i] = 0;
            for (int j = i + 1; j < len; j++) {
                sec[j - i - 1] = s[j];
            }
            sec[len - i - 1] = 0;
        
            return 0;
        }

    return -1;
}

void lh_action_t::connect_init(int argc, const char* argv[]) {
    if (argc < 3)
        lh_err("<ip>:<port> not found");

    static char tmp_ip[32], tmp_port[32];
    static int tmp_stat;
    if ((tmp_stat = lh_cut_string(argv[2], ':', tmp_ip, tmp_port)) == -1)
        lh_err("invalid format <ip>:<port>");
    
    if ((ip = inet_addr(tmp_ip)) == INADDR_ANY)
        lh_err("invalid ip");
    port = atoi(tmp_port);
    if (port < 1024 || port > 65536)
        lh_err("invalid port number");

    for (int i = 3; i < argc; )
        i += add_option(argc, argv, i);
}

int lh_action_t::add_option(int argc, const char* argv[], int now) {
    const char* op = argv[now];
    
    if (!strcmp(op, "-e") || !strcmp(op, "--echo")) {
        puts("using echo mode");
        options.mode = ECHO_MODE;
        return 1;
    }
    if (!strcmp(op, "-p") || !strcmp(op, "--print")) {
        if (now + 1 >= argc)
            lh_err("[-p|--print]: content not found");
        int str_len = strlen(argv[now + 1]);
        options.print_content = new char[str_len + 3];
        memcpy(options.print_content, argv[now + 1], str_len);
        options.print_content[str_len] = 0;

        puts("using print mode");
        options.mode = PRINT_MODE;
        return 2; // get one argument
    }
    if (!strcmp(op, "--tcp") || !strcmp(op, "--nudp")) {
        puts("using tcp protocol");
        options.protocol = TCP_PROTOCOL;
        return 1;
    }
    if (!strcmp(op, "--udp") || !strcmp(op, "--ntcp")) {
        puts("using udp protocol");
        options.protocol = UDP_PROTOCOL;
        return 1;
    }

    static char buf[BUF_SIZE];
    sprintf(buf, "ignored option \'%s\'", op);
    lh_war(buf);
    return 1;
}

lh_action_t::~lh_action_t() {
    if (options.print_content != NULL)
        delete[] options.print_content;
}