#ifndef _LH_ACTION_H
#define _LH_ACTION_H

enum lh_verb_t: int { UNDEFINED_VERB, START_VERB, CONFIG_VERB };

class lh_action_t{
  public:
    struct options_t{
        bool echo_service;
        bool print_service;
        bool tcp_listener;
        bool udp_listener;

        options_t();
        options_t(bool fill); // filled with one boolean
    }options;

    lh_verb_t verb;
    int port; // 9190

    lh_action_t(); // all false constructor
    lh_action_t(int argc, const char* argv[]);
    lh_action_t(int argc, const char* argv[], const lh_action_t& before);

    void set_to(const lh_action_t& to);

  private:
    void start_init(int argc, const char* argv[]);
    void config_init(int argc, const char* argv[]);

    int add_option(int argc, const char* argv[], int now);
};

#endif