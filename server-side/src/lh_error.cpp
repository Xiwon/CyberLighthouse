#include <bits/stdc++.h>

#include "lh_error.h"


void lh_error(const char* message, int line) {
    fprintf(stderr,
        "#clh: error(%d): %s\n", line, message);
    exit(1);
}

void lh_warning(const char* message, int line) {
    fprintf(stderr,
        "#clh: warning(%d): %s\n", line, message);
}