#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "lh_start.h"
#include "lh_consts.h"
#include "lh_action.h"
#include "lh_error.h"

void lh_start_echo_server(lh_action_t& act) {
    int serv_sock, clnt_sock;

    static char buf[BUF_SIZE];
    int str_len;

    sockaddr_in serv_adr, clnt_adr;
    socklen_t adr_sz = sizeof clnt_adr;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        lh_err("socket() error");

    memset(&serv_adr, 0, sizeof serv_adr);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(act.port);

    if (bind(serv_sock, (sockaddr*)&serv_adr, sizeof serv_adr) == -1)
        lh_err("bind() error");

    if (listen(serv_sock, 5) == -1)
        lh_err("listen() error");

    printf("echo-server started at %s, port %d\n", 
        inet_ntoa(serv_adr.sin_addr), act.port);

    for (int i = 1; i <= 5; i++) {
        clnt_sock = accept(serv_sock, (sockaddr*)&clnt_adr, &adr_sz);
        if (clnt_sock == -1)
            lh_err("accept() error");

        printf("accepted client #%d\n", i);
        while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) {
            puts("data package retransmitted");
            write(clnt_sock, buf, str_len);
        }

        close(clnt_sock);
        puts("connection closed");
    }
    close(serv_sock);
}

void lh_start_print_server(lh_action_t& act) {
    
}