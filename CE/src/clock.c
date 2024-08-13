#include <time.h>
#include <limits.h>

#include "clock.h"

clock_t clock_current_clock;

inline void clock_SetClock() { clock_current_clock = clock(); }

clock_t clock_ElapsedTimeSince(clock_t oldTime) {
    if (clock_current_clock < oldTime) {
        // Handle wrapping (untested)
        return (LONG_MAX-oldTime)+clock_current_clock+1;
    } else {
        // No wrapping
        return clock_current_clock-oldTime;
    }
}