
#pragma once

#include <stdint.h>

#define NUM_POOFERS_PER_NODE (4)

class HaloNode
{
public:
    HaloNode(int addr);

    void queryStatus();
    void sendCommands();
    uint16_t getHsiTemp(int idx);
    void enableAc(bool enabled);
    void enableValve(int idx, bool enabled);

private:
    int addr;
    uint16_t nodeStatus[NUM_POOFERS_PER_NODE];
    bool acEnabled;
    bool valveEnabled[NUM_POOFERS_PER_NODE];
};
