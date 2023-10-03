#ifndef _LHC_ACTION_H
#define _LHC_ACTION_H

enum lh_verb_t: int { INVALID_VERB, CONNECT };

class lh_action_t{
  public:
    struct options_t{
        bool echo_mode;
        bool udp_mode;

        options_t();
    }options;

    lh_verb_t verb;
    int port; // 9190
    unsigned int ip;

    lh_action_t(int argc, const char* argv[]);

  private:
    void connect_init(int argc, const char* argv[]);

    int add_option(int argc, const char* argv[], int now);
};

#endif