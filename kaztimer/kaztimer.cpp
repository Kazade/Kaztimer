/*
Copyright (c) 2011, Luke Benstead.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdint>
#include <chrono>
#include <tr1/memory>
#include <map>
#include "kaztimer.h"

#ifdef _WIN32

#include <windows.h>

// From <sys/timeb.h>
#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
struct timespec {
  time_t  tv_sec;   /* Seconds */
  long    tv_nsec;  /* Nanoseconds */
};

struct itimerspec {
  struct timespec  it_interval;  /* Timer period */
  struct timespec  it_value;     /* Timer expiration */
};
#endif

// Define dummy CLOCK_MONOTONIC for WIN32
#define CLOCK_MONOTONIC 1
#define BILLION 1000000000L

struct timespec start_time;
struct timespec stop_time;

LARGE_INTEGER getFILETIMEoffset() {
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int clock_gettime(int X, struct timespec *ts) {
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    ts->tv_sec = t.QuadPart / 1000000;
    ts->tv_nsec = t.QuadPart % 1000000;
    return (0);
}

#endif

class Timer {
public:
    Timer():
        step_(-1),
        is_fixed_(false),
        accumulator_(0.0f) {

        set_game_timer();
    }

    void set_fixed(int step) {
        step_ = step;
        is_fixed_ = true;
        last_time_ = get_current_time_in_nanoseconds();
        accumulator_ = 0.0f;
    }

    void set_game_timer() {
        step_ = -1;
        is_fixed_ = false;
        last_time_ = get_current_time_in_nanoseconds();
    }

    void update_frame_time() {
        frame_time_ = get_elapsed_time();
        if(frame_time_ > 0.25) {
            frame_time_ = 0.25;
        }

        if(is_fixed_) {
            accumulator_ += frame_time_;
        }
    }

    bool can_update() {
        if(!is_fixed_) {
            return true;
        }

        double fixed_step = get_fixed_step();
        if(accumulator_ >= fixed_step) {
            accumulator_ -= fixed_step;
            return true;
        }

        return false;
    }

    double get_fixed_step() {
        return 1.0 / double(step_);
    }

    double get_delta_time() {
        if(is_fixed_) {
            return get_fixed_step();
        }

        return frame_time_;
    }

    double get_elapsed_time() {
        uint64_t current_time = get_current_time_in_nanoseconds();
        uint64_t elapsed = current_time - last_time_;

        last_time_ = current_time;
        return nanoseconds_to_seconds(elapsed);
    }

    uint64_t get_current_time_in_nanoseconds() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);

        const uint64_t NANO_SECONDS_IN_SEC = 1000000000;

        return (ts.tv_sec * NANO_SECONDS_IN_SEC) + ts.tv_nsec;
    }

    double get_accumulator() const { return accumulator_; }
private:
    int step_;
    bool is_fixed_;

    uint64_t last_time_;
    double accumulator_;
    double frame_time_;

    double nanoseconds_to_seconds(uint64_t nano) const {
        const double BILLION = 1.0 / 1000000000.0;
        return double(nano) * BILLION;
    }
};

static KTIuint bound_timer_id_ = 0;
static KTIuint current_timer_id_ = 0;

std::map<KTIuint, std::tr1::shared_ptr<Timer> >& timers() {
    static std::map<KTIuint, std::tr1::shared_ptr<Timer> > timers_;
    return timers_;
}

KTIuint get_next_timer_id() {
    return ++current_timer_id_;
}

Timer* get_bound_timer() {
    if(timers().find(bound_timer_id_) == timers().end()) {
        return NULL;
    }

    return timers()[bound_timer_id_].get();
}

void ktiGenTimers(KTIsizei n, KTIuint* names) {
    for(KTIuint i = 0; i < n; ++i) {
        KTIuint new_id = get_next_timer_id();
        timers()[new_id].reset(new Timer());
        names[i] = new_id;
    }
}

void ktiBindTimer(KTIuint name) {
    if(timers().find(name) == timers().end()) {
        bound_timer_id_ = 0;
    }

    bound_timer_id_ = name;
}

void ktiStartFixedStepTimer(KTIint steps_per_second) {
    Timer* timer = get_bound_timer();
    if(!timer) {
        return;
    }

    timer->set_fixed(steps_per_second);
}

void ktiStartGameTimer() {
    Timer* timer = get_bound_timer();
    if(!timer) {
        return;
    }

    timer->set_game_timer();
}

KTIbool ktiTimerCanUpdate() {
    Timer* timer = get_bound_timer();
    if(!timer) {
        return false;
    }

    return timer->can_update();
}

void ktiUpdateFrameTime() {
    Timer* timer = get_bound_timer();
    if(!timer) {
        return;
    }

    timer->update_frame_time();
}

KTIdouble ktiGetAccumulatorValue() {
    Timer* timer = get_bound_timer();
    if(!timer) {
        return 0.0;
    }
    return timer->get_accumulator();
}

KTIdouble ktiGetDeltaTime() {
    Timer* timer = get_bound_timer();
    if(!timer) {
        return 0.0;
    }

    return timer->get_delta_time();
}

void ktiDeleteTimers(KTIsizei n, const KTIuint* names) {
    for(KTIuint i = 0; i < n; ++i) {
        timers().erase(names[i]);
    }
}
