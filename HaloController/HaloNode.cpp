
#include "HaloNode.h"

#include <Wire.h>
#include <CRC8.h>

#define STATUS_PKT_SIZE (9)
#define COMMAND_PKT_SIZE (6)

HaloNode::HaloNode(int addr) :
    addr(addr)
{
}

void HaloNode::queryStatus()
{
    int numBytes = Wire.requestFrom(addr, STATUS_PKT_SIZE);
    if (numBytes != STATUS_PKT_SIZE) {
        return;
    }

    byte statusPacket[STATUS_PKT_SIZE];
    for (int i = 0; i < STATUS_PKT_SIZE; ++i) {
        statusPacket[i] = Wire.read();
    }

    byte calculatedCRC = CRC8(statusPacket, STATUS_PKT_SIZE-1);
    if (statusPacket[STATUS_PKT_SIZE-1] == calculatedCRC) {
        for (int i = 0; i < NUM_POOFERS_PER_NODE; ++i) {
            nodeStatus[i] = statusPacket[i];
        }
    }
}

void HaloNode::sendCommands()
{
    byte commandPacket[COMMAND_PKT_SIZE];
    commandPacket[0] = acEnableCommand;
    for (int i = 0; i < NUM_POOFERS_PER_NODE; ++i) {
        commandPacket[i + 1] = valveEnableCommands[i];
    }
    commandPacket[COMMAND_PKT_SIZE-1] = CRC8(commandPacket, COMMAND_PKT_SIZE-1);

    Wire.beginTransmission(addr);
    Wire.write(commandPacket, COMMAND_PKT_SIZE);
    Wire.endTransmission();
}

uint16_t HaloNode::getHsiTemp(int idx)
{
    return nodeStatus[idx];
}

void HaloNode::setValveCommand(int idx, bool valveEnabled)
{
    valveEnableCommands[idx] = valveEnabled;
}
