#include <time.h>

#define DEFAULT_TIMEOUT_TIMEVAL                                                \
    {                                                                          \
        .tv_sec = 1, .tv_usec = 0                                              \
    }

long long timespec_diff_ms(struct timespec* start, struct timespec* end);
