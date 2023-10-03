#ifndef _LH_ERROR_H
#define _LH_ERROR_H

#define lh_err(message) lh_error(message, __LINE__)
#define lh_war(message) lh_warning(message, __LINE__)

void lh_error(const char* message, int line);
void lh_warning(const char* message, int line);

#endif