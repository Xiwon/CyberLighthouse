#ifndef _LH_ACTION_H
#define _LH_ACTION_H

enum lh_verb_t: int { UNDEFINED_VERB, START_VERB };
enum lh_mode_t: int { UNDEFINED_MODE, ECHO_MODE, PRINT_MODE };
enum lh_protocol_t: int { TCP_PROTOCOL, UDP_PROTOCOL };

class lh_action_t{
  public:
    struct options_t{
        lh_mode_t mode;
        lh_protocol_t protocol;

        options_t();
    }options;

    lh_verb_t verb;
    int port; // 9190

    lh_action_t(int argc, const char* argv[]);

  private:
    void start_init(int argc, const char* argv[]);

    int add_option(int argc, const char* argv[], int now);
};

#endif