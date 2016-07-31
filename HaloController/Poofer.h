
#pragma once

#include "HaloNode.h"

#include <SimpleTimer.h>

class Poofer
{
public:
    enum Status {
        COLD,
        HOT_INACTIVE,
        HOT_ACTIVE
    };

    Poofer(HaloNode *node, int idx);

    Status getStatus();
    void trigger(bool previewOnly);
    void run();

private:
    void timerExpired();
    static void timerExpired(void *arg);

    HaloNode *node;
    int idx;
    bool active;
    SimpleTimer timer;
};
