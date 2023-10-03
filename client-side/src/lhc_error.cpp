#include <bits/stdc++.h>

#include "lhc_error.h"

void lh_error(const char* message, int line) {
    fprintf(stderr,
        "#clh-clnt: error(%d): %s\n", line, message);
    exit(1);
}

void lh_warning(const char* message, int line) {
    fprintf(stderr,
        "#clh-clnt: warning(%d): %s\n", line, message);
}