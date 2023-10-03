#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "lh_start.h"
#include "lh_consts.h"
#include "lh_action.h"
#include "lh_error.h"

void lh_start_echo_service(int clnt_sock) {
    static char buf[BUF_SIZE];
    static char str_len;

    while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) {
        puts("\ndata package retransmitted");
        write(clnt_sock, &str_len, 1);
        write(clnt_sock, buf, str_len);
    }
}

void lh_start_print_service(int clnt_sock) {
    
}

std::mutex lh_tcp_mtx, lh_udp_mtx;
std::condition_variable lh_cmdline_broadcast;
bool lh_end_start_flag; // true => exit program

void lh_start_tcp_listener(lh_action_t& act) {
    bool& enable = std::ref(act.options.tcp_listener); // listener enable flag
    std::unique_lock<std::mutex> lck(lh_tcp_mtx);

    while (lh_end_start_flag == false) {
        while (!enable && !lh_end_start_flag)
            lh_cmdline_broadcast.wait(lck); // waiting for settings flashed


        int serv_sock, clnt_sock; // build socket

        serv_sock = socket(PF_INET, SOCK_STREAM, 0);
        if (serv_sock == -1)
            lh_err("socket() error");

        sockaddr_in serv_adr, clnt_adr;
        socklen_t adr_sz = sizeof clnt_adr;

        memset(&serv_adr, 0, sizeof serv_adr);
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_adr.sin_port = htons(act.port);

        if (bind(serv_sock, (sockaddr*)&serv_adr, sizeof serv_adr) == -1)
            lh_err("bind() error");

        if (listen(serv_sock, 5) == -1)
            lh_err("listen() error");

        while (enable) { // accept until tcp disabled
            char link_type;

            clnt_sock = accept(serv_sock, (sockaddr*)&clnt_adr, &adr_sz);
            if (clnt_sock == -1) {
                lh_war("accept() error");
                continue;
            }

            read(clnt_sock, &link_type, 1);

            static char reject[32];
            if (link_type == 'e') { // echo service
                if (act.options.echo_service) {
                    puts("\necho service started");
                    std::thread thd(lh_start_echo_service, clnt_sock);
                    thd.join();
                }
                else {
                    sprintf(reject, "echo service not supported\n");
                    char len = strlen(reject);
                    write(clnt_sock, &len, 1);
                    write(clnt_sock, reject, strlen(reject));
                }
            }

            close(clnt_sock);
        }

        close(serv_sock);
    }

    return;
}

void lh_cmdline_prompt() {
    printf("#clh cmd> ");
}

void lh_start_cmdline(lh_action_t& act) {
    puts("command line running");

    lh_action_t curact; // all false
    std::thread tcp_thd(lh_start_tcp_listener, std::ref(curact)); // start tcp listener thread

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // wait for 20ms

    curact.set_to(act); // write settings
    lh_cmdline_broadcast.notify_all(); // flash & run

    puts("`shutdown` to close server");
    while (1) {
        static char buf[BUF_SIZE];
        lh_cmdline_prompt();
        fgets(buf, BUF_SIZE, stdin);
        
        if (!strcmp(buf, "shutdown\n")) {
            lh_end_start_flag = true; // end flag
            lh_action_t end_act; // all false;
            curact.set_to(end_act); // close all service
            lh_cmdline_broadcast.notify_all(); // flash settings

            puts("detach all connections and shutdown...");
            tcp_thd.detach();

            return;
        }

        
    }
}