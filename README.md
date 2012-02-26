# Kaztimer

## What is it?

Kaztimer is a small library that provides cross-platform timers for game 
development. It provides both fixed and variable step timers. The former
is great for physics simulations, the later for general framerate independence.

## How do I use it?

First, add the source files to your project and include kaztimer.h. Then creating
and using timers is pretty straightforward:


```
    KTIuint timers[2];
    ktiGenTimers(2, timers);

    ktiBindTimer(timers[0]);
    ktiStartFixedStepTimer(30); //30 updates per second

    while(ktiTimerCanUpdate()) {
        float dt = ktiGetDeltaTime(); //Will always return a fixed value
        //Do fixed step stuff (e.g. physics)
    }

    ktiBindTimer(timers[1]);
    ktiStartGameTimer();

    float deltatime = ktiGetDeltaTime(); //Variable return value
    //ktiTimerCanUpdate() will always return true
    //Update based on elapsed time
```

