#ifndef _LHC_ACTION_H
#define _LHC_ACTION_H

enum lh_verb_t: int { UNDEFINED_VERB, CONNECT_VERB };
enum lh_protocol_t: int { TCP_PROTOCOL, UDP_PROTOCOL };
enum lh_mode_t: int { UNDEFINED_MODE, ECHO_MODE, PRINT_MODE };

class lh_action_t{
  public:
    struct options_t{
        lh_mode_t mode;
        lh_protocol_t protocol;
        char* print_content;

        options_t();
    }options;

    lh_verb_t verb;
    int port; // 9190
    unsigned int ip;

    lh_action_t(int argc, const char* argv[]);
    ~lh_action_t();

  private:
    void connect_init(int argc, const char* argv[]);

    int add_option(int argc, const char* argv[], int now);
};

#endif