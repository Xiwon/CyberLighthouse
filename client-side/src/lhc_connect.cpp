#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "lhc_connect.h"
#include "lhc_consts.h"
#include "lhc_action.h"
#include "lhc_error.h"

void lh_connect_echo_client(lh_action_t& act) {
    printf("start echo-client at %s, port %d\n", 
        inet_ntoa((in_addr){act.ip}), act.port);

    int sock, str_len;
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

    while (1) {
        printf("#echo-client input> ");
        fgets(buf, BUF_SIZE, stdin);

        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
            break;
        
        str_len = write(sock, buf, strlen(buf));
        char get_len;
        read(sock, &get_len, 1);

        for (int recv_len = 0, cnt; recv_len < get_len; ) {
            // expecting a string of length `get_len`
            cnt = read(sock, &buf[recv_len], BUF_SIZE - recv_len - 1);
            if (cnt == -1)
                lh_err("read() error");
            if (cnt == 0) {
                lh_err("server disconnected");
            }

            recv_len += cnt;
        }

        buf[get_len] = 0;
        printf("#message from server> %s", buf);

        if (str_len != get_len) // echo not supported
            break;
    }

    close(sock);
}