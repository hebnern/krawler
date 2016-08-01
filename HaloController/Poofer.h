
#pragma once

#include "HaloNode.h"

#include <SimpleTimer.h>

class Poofer
{
public:
    enum State {
        COLD,
        HOT_INACTIVE,
        HOT_ACTIVE
    };

    typedef void (*StateChangeCallback)(int pooferIdx, State state);

    Poofer(int pooferIdx, HaloNode *node, int nodeIdx);

    void onStateChange(StateChangeCallback cb);
    void trigger(bool preview);
    void run();

private:
    void setState(State state);
    void timerExpired();
    static void timerExpired(void *arg);

    int pooferIdx;
    HaloNode *node;
    int nodeIdx;
    State curState;
    SimpleTimer timer;
    StateChangeCallback stateChangeCb;
};
