#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;
#define lh_err(message) lh_error(message, __LINE__)
#define lh_war(message) lh_warning(message, __LINE__);

void lh_error(const char* message, int line);
void lh_warning(const char* message, int line);

enum verb_t: int { INVALID_VERB, START };

class lh_action_t{
  public:
    struct options_t{
        bool echo_mode;
        bool udp_support;

        options_t() {
            echo_mode = false;
            udp_support = false;
        }
    }options;

    verb_t verb;
    int port; // 9190

    lh_action_t(int argc, const char* argv[]): options(),
        verb(INVALID_VERB), port(0) {

        if (argc < 2)
            lh_err("verb not found");

        if (!strcmp(argv[1], "start")) {
            verb = START;
            start_init(argc, argv);
        } else {
            lh_err("invalid verb");
        }
    }

  private:
    void start_init(int argc, const char* argv[]) {
        if (argc < 3)
            lh_err("port not found");
        port = atoi(argv[2]);
        if (port < 1024 || port > 65535)
            lh_err("invalid port number");
        // printf("set port %d\n", port);

        for (int i = 3; i < argc; )
            i += add_option(argc, argv, i);
    }

    int add_option(int argc, const char* argv[], int now) {
        const char* op = argv[now];
        
        if (!strcmp(op, "-e") || !strcmp(op, "--echo")) {
            puts("echo mode enabled");
            options.echo_mode = true;
            return 1;
        }
        if (!strcmp(op, "-udp")) {
            puts("udp enabled");
            options.udp_support = true;
            return 1;
        }
        if (!strcmp(op, "-nudp")) {
            puts("udp disabled");
            options.udp_support = false;
            return 1;
        }

        static char buf[BUF_SIZE];
        sprintf(buf, "ignored option \'%s\'", op);
        lh_war(buf);
        return 1;
    }
};

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

int main(int argc, const char* argv[]) {
    lh_action_t act(argc, argv); // analyze command

    if (act.verb == START) {
        if (act.options.echo_mode)
            lh_start_echo_server(act);
    }
}

void lh_error(const char* message, int line) {
    fprintf(stderr,
        "#clh: error(%d): %s\n", line, message);
    exit(1);
}

void lh_warning(const char* message, int line) {
    fprintf(stderr,
        "#clh: warning(%d): %s\n", line, message);
}