#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

#define lh_err(message) lh_error(message, __LINE__)
#define lh_war(message) lh_warning(message, __LINE__)

void lh_error(const char* message, int line);
void lh_warning(const char* message, int line);

void lh_print_hex(const u8* buf, int len);

int lh_cut_string(const char* s, char div, char* fir, char* sec) {
    int len = strlen(s);

    for (int i = 0; i < len; i++)
        if (s[i] == div) {
            for (int j = 0; j < i; j++)
                fir[j] = s[j];
            fir[i] = 0;
            for (int j = i + 1; j < len; j++)
                sec[j - i - 1] = s[j];
            sec[len - i - 1] = 0;
        
            return 0;
        }
    return -1;
}

class lh_qry;
class lh_res;

const char* type_names[] = {
    "A", "NS", "CNAME", "MX", "TXT"
};
const u16 qtype_num[] = { // value
    1, 2, 5, 15, 16
};
class lh_qry{
    friend class lh_res;

    std::vector<std::string> qname;
    char server[64];
    int port;
    bool recur;

    enum {A, NS, CNAME, MX, TXT};
    int type;

  public:
    lh_qry(int argc, const char* argv[]) {
        if (argc < 2)
            lh_err("domain name not found");

        { // domain name
            int len = strlen(argv[1]);
            const char* s = argv[1];
            int p = len - 1;
            if (s[p] == '.') p--;

            do {
                std::string label;
                while ((p >= 0) && (s[p] != '.')) {
                    label += s[p];
                    p--;
                }
                p--;
                std::reverse(label.begin(), label.end());
                label += '.';
                qname.push_back(label);
            } while (p >= 0);

            while ((qname.size() > 1) && ((*qname.rbegin()) == "."))
                qname.pop_back();

            // printf("[qname] = \n");
            // for (auto ss: qname) {
            //     printf("> %s\n", ss.c_str());
            // }
            // puts("==========");
        }

        recur = true;
        type = A;
        memset(server, 0, 64);
        memcpy(server, "8.8.8.8", 7);
        port = 53; // default

        for (int i = 2; i < argc; i++) {
            if (argv[i][0] == '@') {
                static char tmp_ip[32], tmp_port[32];

                if (lh_cut_string(&argv[i][1], ':', tmp_ip, tmp_port) == -1)
                    lh_err("invalid format @<ip>:<port>");
                
                if (inet_addr(tmp_ip) == INADDR_ANY)
                    lh_err("invalid ip");
                memset(server, 0, 64);
                memcpy(server, tmp_ip, strlen(tmp_ip));

                port = atoi(tmp_port);
                if (port < 0 || port > 65536)
                    lh_err("invalid port number");
            }

            else if (argv[i][0] == '-') {
                if (!strcmp(&argv[i][1], "A")) {
                    type = A;
                }
                if (!strcmp(&argv[i][1], "NS")) {
                    type = NS;
                }
                if (!strcmp(&argv[i][1], "CNAME")) {
                    type = CNAME;
                }
                if (!strcmp(&argv[i][1], "MX")) {
                    type = MX;
                }
                if (!strcmp(&argv[i][1], "TXT")) {
                    type = TXT;
                }
                if (!strcmp(&argv[i][1], "nr")) {
                    recur = false;
                }
            }
        }

        { // debug
            puts("-- Destination Info --");
            printf("[dst ip]: %s\n", server);
            printf("[dst port]: %d\n", port);
            printf("[recursive]: %d\n", (int)recur);
            printf("[qry type]: %s\n", type_names[type]);
            puts("---------------");
        }
    }

    void makeraw() {
        raw.clear();

        push2bytes(11451); // ID
        { // flags
            u16 flags = 
                (recur ? 0x0100 : 0);
            push2bytes(flags); // flags
        }
        push2bytes(1); // QDCOUNT
        push2bytes(0); // ANCOUNT
        push2bytes(0); // NSCOUNT
        push2bytes(0); // ARCOUNT

        { // question section
            // name
            int sz = qname.size();
            for (int i = sz - 1; i >= 0; i--) {
                u8 len = qname[i].length() - 1;
                push1byte(len);
                for (int j = 0; j < len; j++)
                    raw.push_back((u8)qname[i][j]);
            }
            push1byte(0);

            // qtype
            push2bytes(qtype_num[type]);

            // qclass
            push2bytes(1);
        }
    }
    std::vector<u8> raw; // bin data
    friend void lh_makeqry(lh_qry& qry, lh_res& res);

  private:

    void push2bytes(u16 x) {
        x &= 0xffff;
        u8 b1 = x >> 8;
        u8 b2 = x & 0xff;
        raw.push_back(b1);
        raw.push_back(b2);
    }

    void push1byte(u8 x) {
        raw.push_back(x);
    }
};

struct lh_rr{
    std::string rname;
    u16 rtype, rclass;
    u32 ttl;
    u16 rdlength;
    std::vector<u8> rdata;
};

class lh_res{
    u8 raw[BUF_SIZE];
    int rawlen;

    u16 id, flags;
    u16 qdcount, ancount, nscount, arcount;
    std::string qname; // query name (assume question == 1)
    u16 qtype, qclass;
    std::vector<lh_rr> rrs;

  public:
    lh_res() {
        memset(raw, 0, BUF_SIZE);
        rawlen = 0;
    }

    void readraw() {
        int p = 0; // pointer

        id = read2bytes(p);
        flags = read2bytes(p);
        qdcount = read2bytes(p);
        ancount = read2bytes(p);
        nscount = read2bytes(p);
        arcount = read2bytes(p);
        { // debug
            puts("-- Response Info --");
            printf("Transaction ID: 0x%04x\n", id);
            printf("Flags: 0x%04x\n", flags);
            printf("Questions: %u\n", qdcount);
            printf("Answer RRs: %u\n", ancount);
            printf("Authority RRs: %u\n", nscount);
            printf("Additional RRs: %u\n", arcount);
            puts("---------------");
        }

        assert(qdcount == 1);
        { // queries section. assume question num always == 1
            qname = readname(p);
            qtype = read2bytes(p);
            qclass = read2bytes(p);
        }
        { // debug
            puts("-- Queries Info --");
            printf("Name: %s\n", qname.c_str());
            printf("Type: %u\n", qtype);
            printf("Class: %u\n", qclass);
            puts("---------------");
        }

        { // RRs collect
            int tot = ancount + nscount + arcount;
            for (int tot_i = 1; tot_i <= tot; tot_i++) {
                lh_rr res;
                res.rname = readname(p);
                res.rtype = read2bytes(p);
                res.rclass = read2bytes(p);
                res.ttl = read4bytes(p);
                res.rdlength = read2bytes(p);
                for (int i = 1; i <= res.rdlength; i++) {
                    res.rdata.push_back(raw[p]);
                    p++;
                }

                rrs.push_back(res);
            }

            assert(p == rawlen);
        }
        { // debug
            int tot = rrs.size();
            printf("[rr number]> %d\n", tot);
            puts("---------------");
            for (auto res: rrs) {
                printf("[rname]> %s\n", res.rname.c_str());
                printf("[rtype]> %u\n", res.rtype);
                printf("[rclass]> %u\n", res.rclass);
                printf("[ttl]> %u\n", res.ttl);
                printf("[rdlength]> %u\n", res.rdlength);
                printf("[rdata]>\n");
                lh_print_hex(res.rdata.data(), res.rdlength);
            }
        }
    }

    friend void lh_makeqry(lh_qry& qry, lh_res& res);

  private:

    u32 read4bytes(int& p) {
        assert(sizeof(u32) == 4);
        u32 ret =
            (((u32)raw[p]) << 24) + (((u32)raw[p + 1]) << 16) +
            (((u32)raw[p + 2]) << 8) + ((u32)raw[p + 3]);
        p += 4;
        return ret;
    }

    u16 read2bytes(int& p) {
        u16 ret = 
            (((u16)raw[p]) << 8) + ((u16)(raw[p + 1]));
        p += 2;
        return ret;
    }

    u8 read1byte(int& p) {
        u8 ret = (u8)raw[p];
        p++;
        return ret;
    }

    char read1char(int& p) {
        char ret = (char)raw[p];
        p++;
        return ret;
    }

    std::string readname(int& p) {
        // get a domain name from offset p
        int tmp = p;
        if ((read1byte(tmp) >> 6) == 3) { // end with a compressed pointer
            int offset = read2bytes(p) & 0x3fff;
            return readname(offset);
        }

        int len = read1byte(p);
        if (len == 0)
            return "";

        std::string ret;
        for (int i = 1; i <= len; i++)
            ret += read1char(p);

        return ret + "." + readname(p); // recursivly readname.
    }
};

void lh_makeqry(lh_qry& qry, lh_res& res) {
    int serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1)
        lh_err("socket() error");

    sockaddr_in serv_adr, clnt_adr;
    socklen_t adr_sz = sizeof clnt_adr;

    memset(&serv_adr, 0, sizeof serv_adr);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(9190); // port 9190

    memset(&clnt_adr, 0, sizeof clnt_adr);
    clnt_adr.sin_family = AF_INET;
    clnt_adr.sin_addr.s_addr = inet_addr(qry.server);
    clnt_adr.sin_port = htons(qry.port);

    if (bind(serv_sock, (sockaddr*)&serv_adr, sizeof serv_adr) == -1)
        lh_err("bind() error");

    int len = qry.raw.size();
    const u8* buf = qry.raw.data();
    sendto(serv_sock, buf, len, 0, (sockaddr*)&clnt_adr, adr_sz);
    sleep(1);

    res.rawlen = recvfrom(serv_sock, res.raw, 
        BUF_SIZE, 0, (sockaddr*)&clnt_adr, &adr_sz);
    sleep(1);
    close(serv_sock);

    // lh_print_hex(res.raw, res.rawlen);
}

int main(int argc, const char* argv[]) {
    puts("#>>> Query Construct Section <<<#");

    lh_qry qry(argc, argv);
    qry.makeraw(); // construct bin packet

    // lh_print_hex(qry.raw.data(), qry.raw.size());

    puts("#>>> Response Get Section <<<#");

    lh_res res;
    lh_makeqry(qry, res);
    res.readraw();
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

void lh_print_hex(const u8* buf, int len) {
    static char hex[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    puts("#hex>");
    for (int i = 0; i < len; i++) {
        int val = buf[i];
        printf("%c%c ", hex[(val >> 4) & 15], hex[val & 15]);
        if ((i + 1) % 8 == 0) // 8 bytes per line
            putchar('\n');
    }
    puts("\n==========");
}