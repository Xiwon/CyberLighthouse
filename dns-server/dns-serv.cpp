
#define LH_CACHE_FILE "dns_cache"


class lh_dns_cachefile{
    std::vector<lh_rr> cache_rr;
    int rr_sz;

  public:
    lh_dns_cachefile() {
        // std::ifstream in(LH_CACHE_FILE);
        // in >> rr_sz;

        // for (int i = 1; i <= rr_sz; i++) {
        //     // in >> timstmp >> ttl; // invalid RR
        //     // if (now > timstmp + ttl)
        //     //     continue;

        //     lh_rr res;
        //     std::string s_type;
        //     in >> res.rname;
        //     in >> s_type;
        //     res.rtype = gettype(s_type);
        //     res.rclass = 1;
        //     res.ttl = ttl;
            
        // }
    }
    ~lh_dns_cachefile() { // write back

    }

  private:
    int gettype(std::string s_type) {
        if (s_type == "A") return 1;
        return 0;
    }
};

class lh_dns_serv_action{
  public:
    lh_dns_serv_action(int argc, const char* argv[]) {
        // if (!strcmp(argv[1], "start")) {
            
        // }
        // if (!strcmp(argv[1], "flash")) {
        //     verb = FLASH_VERB;
        // }
    }
};

void lh_dns_server(int argc, const char* argv[]) {
    lh_dns_cachefile cache;
    lh_dns_serv_action act(argc, argv);

    // if (act.verb == FLASH_VERB) {
    //     if (act.options.force_flash) { // clear cache
    //         cache.clear();
    //     }
    //     return;
    // }

    // START server

}