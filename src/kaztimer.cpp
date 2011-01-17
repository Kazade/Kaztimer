#include <ctime>
#include <boost/shared_ptr.hpp>
#include <map>
#include "kaztimer.h"

class Timer {
public:
    Timer():
        step_(-1),
        accumulator_(0.0f) {

        set_game_timer();
    }

    void set_fixed(int step) {
        step_ = step;
        is_fixed_ = true;
        last_time_ = get_current_time_in_ms();
        accumulator_ = 0.0f;
    }

    void set_game_timer() {
        step_ = -1;
        is_fixed_ = false;
        last_time_ = get_current_time_in_ms();
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
        long current_time = get_current_time_in_ms();
        double elapsed = double(current_time - last_time_) * 0.001;
        last_time_ = current_time;
        return elapsed;
    }

    long get_current_time_in_ms() {
#ifdef WIN32
        return timeGetTime();
#else
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return (long) (ts.tv_sec * 1000.0) + ts.tv_nsec / 1000000.0;
#endif
    }

private:
    int step_;
    bool is_fixed_;

    long last_time_;
    double accumulator_;
    double frame_time_;
};

static std::map<KTIuint, boost::shared_ptr<Timer> > timers_;

static KTIuint bound_timer_id_ = 0;
static KTIuint current_timer_id_ = 0;

KTIuint get_next_timer_id() {
    return ++current_timer_id_;
}

Timer* get_bound_timer() {
    if(timers_.find(bound_timer_id_) == timers_.end()) {
        return NULL;
    }

    return timers_[bound_timer_id_].get();
}

void ktiGenTimers(KTIsizei n, KTIuint* names) {
    for(KTIuint i = 0; i < n; ++i) {
        KTIuint new_id = get_next_timer_id();
        timers_[new_id].reset(new Timer());
        names[i] = new_id;
    }
}

void ktiBindTimer(KTIuint name) {
    if(timers_.find(name) == timers_.end()) {
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

KTIdouble ktiGetDeltaTime() {
    Timer* timer = get_bound_timer();
    if(!timer) {
        return 0.0;
    }

    return timer->get_delta_time();
}

void ktiDeleteTimers(KTIsizei n, const KTIuint* names) {
    for(KTIuint i = 0; i < n; ++i) {
        timers_.erase(names[i]);
    }
}
