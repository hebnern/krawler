
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
    void setValveCommand(int idx, bool valveEnabled);

private:
    int addr;
    uint16_t nodeStatus[NUM_POOFERS_PER_NODE];
    bool acEnableCommand;
    bool valveEnableCommands[NUM_POOFERS_PER_NODE];
};
