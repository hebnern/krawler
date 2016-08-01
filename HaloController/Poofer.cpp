
#include "Poofer.h"

#define HSI_HOT_THRESHOLD (500)

Poofer::Poofer(int pooferIdx, HaloNode *node, int nodeIdx) :
    pooferIdx(pooferIdx),
    node(node),
    nodeIdx(nodeIdx)
{
}

void Poofer::onStateChange(Poofer::StateChangeCallback stateChangeCb)
{
    this->stateChangeCb = stateChangeCb;
}

void Poofer::setState(Poofer::State state)
{
    if (curState != state) {
        curState = state;
        if (stateChangeCb) {
            stateChangeCb(pooferIdx, state);
        }
    }
}

void Poofer::trigger(bool preview)
{
    if (curState == HOT_INACTIVE) {
        node->setValveCommand(nodeIdx, !preview);
        timer.setTimeout(100, Poofer::timerExpired, this);
        setState(HOT_ACTIVE);
    }
}

void Poofer::run()
{
    uint16_t hsiTemp = node->getHsiTemp(nodeIdx);
    if (hsiTemp < HSI_HOT_THRESHOLD) {
        node->setValveCommand(nodeIdx, false);
        setState(COLD);
    }
    else if (curState == HOT_ACTIVE) {
        timer.run();
    }
}

void Poofer::timerExpired()
{
    node->setValveCommand(nodeIdx, false);
    setState(HOT_INACTIVE);
}

void Poofer::timerExpired(void *arg)
{
    ((Poofer*)arg)->timerExpired();
}
