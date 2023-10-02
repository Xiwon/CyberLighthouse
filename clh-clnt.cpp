#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;
#define lh_err(message) lh_error(message, __LINE__)
#define lh_war(message) lh_warning(message, __LINE__);

void lh_error(const char* message, int line);
void lh_warning(const char* message, int line);

enum verb_t: int { INVALID_VERB, CONNECT };

class lh_action_t{
  public:
    struct options_t{
        bool echo_mode;
        bool udp_mode;

        options_t() {
            echo_mode = false;
            udp_mode = false;
        }
    }options;

    verb_t verb;
    int port; // 9190
    unsigned int ip;

    lh_action_t(int argc, const char* argv[]): options(),
        verb(INVALID_VERB), port(0), ip(0) {

        if (argc < 2)
            lh_err("verb not found");

        if (!strcmp(argv[1], "connect")) {
            verb = CONNECT;
            connect_init(argc, argv);
        } else {
            lh_err("invalid verb");
        }
    }

  private:
    void connect_init(int argc, const char* argv[]) {
        if (argc < 3)
            lh_err("ip not found");
        ip = inet_addr(argv[2]);
        if (ip == INADDR_ANY)
            lh_err("invaild ip");

        if (argc < 4)
            lh_err("port not found");
        port = atoi(argv[3]);
        if (port < 1024 || port > 65535)
            lh_err("invalid port number");
        // printf("port %d\n", port);

        for (int i = 4; i < argc; )
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
            puts("using udp connection");
            options.udp_mode = true;
            return 1;
        }
        if (!strcmp(op, "-nudp")) {
            puts("udp connection disabled");
            options.udp_mode = false;
            return 1;
        }

        static char buf[BUF_SIZE];
        sprintf(buf, "ignored option \'%s\'", op);
        lh_war(buf);
        return 1;
    }
};

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

int main(int argc, const char* argv[]) {
    lh_action_t act(argc, argv); // analyze command

    if (act.verb == CONNECT) {
        if (act.options.echo_mode)
            lh_connect_echo_client(act);
    }
}

void lh_error(const char* message, int line) {
    fprintf(stderr,
        "#clh-clnt: error(%d): %s\n", line, message);
    exit(1);
}

void lh_warning(const char* message, int line) {
    fprintf(stderr,
        "#clh-clnt: warning(%d): %s\n", line, message);
}