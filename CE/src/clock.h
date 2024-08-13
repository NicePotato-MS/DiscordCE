#pragma once

#include <time.h>
#include <limits.h>

extern clock_t clock_current_clock;

extern void clock_SetClock();

clock_t clock_ElapsedTimeSince(clock_t oldTime);