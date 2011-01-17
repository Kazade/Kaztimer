#ifndef KAZTIMER_H_INCLUDED
#define KAZTIMER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int KTIsizei;
typedef unsigned int KTIuint;
typedef int KTIint;
typedef double KTIdouble;
typedef unsigned char KTIbool;

void ktiGenTimers(KTIsizei n, KTIuint* names);
void ktiBindTimer(KTIuint name);
void ktiStartFixedStepTimer(KTIint steps_per_second);
void ktiStartGameTimer();
void ktiUpdateFrameTime();
KTIbool ktiTimerCanUpdate();
KTIdouble ktiGetDeltaTime();
void ktiDeleteTimers(KTIsizei n, const KTIuint* names);

#ifdef __cplusplus
}
#endif

/**
    USAGE:
        KTIuint timers[2];
        ktiGenTimers(2, timers);

        ktiBindTimer(timers[0]);
        ktiStartFixedStepTimer(30);

        while(ktiTimerCanUpdate()) {
            KSfloat dt = ktiGetDeltaTime(); //Will always return a fixed value
            //Do fixed step stuff (e.g. physics)
        }

        ktiStartGameTimer();

        KSfloat deltatime = ktiGetDeltaTime(); //Variable return value
        //ktiTimerCanUpdate() will always return true
        //Update based on elapsed time
*/
#endif // KAZTIMER_H_INCLUDED
