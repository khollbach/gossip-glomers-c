#include "stopwatch.h"
#include <time.h>

// Function to calculate the time difference between two timespec values in
// milliseconds
long long timespec_diff_ms(struct timespec* start, struct timespec* end)
{
    long long diff_sec = (long long)(end->tv_sec - start->tv_sec);
    long long diff_nsec = (long long)(end->tv_nsec - start->tv_nsec);

    // Convert nanoseconds to milliseconds and add to seconds
    return diff_sec * 1000LL + diff_nsec / 1000000LL;
}
