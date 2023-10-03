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

    int sock;
    static char buf[BUF_SIZE];
    int str_len;

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

    while (1) {
        printf("#echo-client input> ");
        fgets(buf, BUF_SIZE, stdin);

        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
            break;
            
        str_len = write(sock, buf, strlen(buf));
        for (int recv_len = 0, cnt; recv_len < str_len; ) {
            // expecting a string of length `str_len`
            cnt = read(sock, &buf[recv_len], BUF_SIZE - recv_len - 1);
            if (cnt == -1)
                lh_err("read() error");
            recv_len += cnt;
        }

        buf[str_len] = 0;
        printf("#message from server$ %s", buf);
    }
    close(sock);
}