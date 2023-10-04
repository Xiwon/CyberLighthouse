#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "lhc_connect.h"
#include "lhc_consts.h"
#include "lhc_action.h"
#include "lhc_error.h"

void lh_read_len(const int sock, char* buf, const int buf_sz, const int len) {
    for (int recv_len = 0, cnt; recv_len < len; ) {
        // expecting len bytes data to buf
        cnt = read(sock, &buf[recv_len], buf_sz - recv_len - 1);
        if (cnt == -1)
            lh_err("read() error");
        if (cnt == 0)
            lh_err("server disconnected");

        recv_len += cnt;
    }
    buf[len] = 0;
}

void lh_connect_echo_client(lh_action_t& act) {
    printf("start echo-client connected to %s, port %d\n", 
        inet_ntoa((in_addr){act.ip}), act.port);

    int sock;
    static char buf[BUF_SIZE];
    sockaddr_in serv_adr;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        lh_err("socket() error");

    memset(&serv_adr, 0, sizeof serv_adr);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = act.ip;
    serv_adr.sin_port = htons(act.port);

    if (connect(sock, (sockaddr*)&serv_adr, sizeof serv_adr) == -1)
        lh_err("connect() error");
    puts("connected..........");
    puts("Q/q to quit connection");

    write(sock, "e", 1); // notify connection type
    read(sock, buf, 1); // get server answer

    if (buf[0] == 'N') { // connection rejected
        char get_len;
        read(sock, &get_len, 1);
        lh_read_len(sock, buf, BUF_SIZE, get_len);
        printf("#message from server> %s", buf);
    }
    else { // buf[0] == 'Y'
        while (1) {
            printf("#echo-client input> ");
            fgets(buf, BUF_SIZE, stdin);

            if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
                break;
            
            write(sock, buf, strlen(buf));

            char get_len;
            read(sock, &get_len, 1);
            lh_read_len(sock, buf, BUF_SIZE, get_len);
            printf("#message from server> %s", buf);
        }
    }

    close(sock);
}

void lh_connect_print_client(lh_action_t& act) {
    printf("start print-client connected to %s, port %d\n", 
        inet_ntoa((in_addr){act.ip}), act.port);

    int sock;
    static char buf[BUF_SIZE], str_len;
    sockaddr_in serv_adr;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        lh_err("socket() error");

    memset(&serv_adr, 0, sizeof serv_adr);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = act.ip;
    serv_adr.sin_port = htons(act.port);

    if (connect(sock, (sockaddr*)&serv_adr, sizeof serv_adr) == -1)
        lh_err("connect() error");
    puts("connected..........");

    write(sock, "p", 1); // notify connection type
    read(sock, buf, 1); // get server answer

    if (buf[0] == 'Y') { // write data while accepted
        str_len = strlen(act.options.print_content);
        write(sock, &str_len, 1);
        write(sock, act.options.print_content, str_len);
    }

    read(sock, &str_len, 1);
    lh_read_len(sock, buf, BUF_SIZE, str_len);

    printf("#message from server> %s", buf);

    close(sock);
}

void lh_connect_udp_echo_client(lh_action_t& act) {
    printf("start udp-print-client connected to %s, port %d\n", 
        inet_ntoa((in_addr){act.ip}), act.port);

    int sock;
    static char buf[BUF_SIZE];
    sockaddr_in serv_adr, from_adr;
    socklen_t adr_sz;

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        lh_err("socket() error");

    memset(&serv_adr, 0, sizeof serv_adr);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = act.ip;
    serv_adr.sin_port = htons(act.port);

    sendto(sock, "e", 1, 0, (sockaddr*)&serv_adr, sizeof serv_adr);
    recvfrom(sock, buf, 1, 0, (sockaddr*)&from_adr, &adr_sz);

    if (buf[0] == 'N') {
        char get_len;
        recvfrom(sock, &get_len, 1, 0, (sockaddr*)&from_adr, &adr_sz);
        recvfrom(sock, buf, get_len, 0, (sockaddr*)&from_adr, &adr_sz);
        buf[get_len] = 0;
        printf("#message from server> %s", buf);
    }
    else { // buf[0] == 'Y'
        while (1) {
            printf("#echo-client input> ");
            fgets(buf, BUF_SIZE, stdin);

            if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
                break;

            sendto(sock, buf, strlen(buf), 0, (sockaddr*)&serv_adr, sizeof serv_adr);

            char get_len;
            recvfrom(sock, &get_len, 1, 0, (sockaddr*)&from_adr, &adr_sz);
            recvfrom(sock, buf, get_len, 0, (sockaddr*)&from_adr, &adr_sz);
            buf[get_len] = 0;
            printf("#message from server> %s", buf);
        }
    }
}