
#include "Poofer.h"

#define HSI_HOT_THRESHOLD (500)

Poofer::Poofer(HaloNode *node, int idx) :
    node(node),
    idx(idx)
{
}

Poofer::Status Poofer::getStatus()
{
    uint16_t hsiTemp = node->getHsiTemp(idx);
    if (hsiTemp > HSI_HOT_THRESHOLD) {
        if (active) {
            return HOT_ACTIVE;
        }
        else {
            return HOT_INACTIVE;
        }
    }
    else {
        return COLD;
    }
}

void Poofer::trigger(bool previewOnly)
{
    node->setValveCommand(idx, !previewOnly);
    timer.setTimeout(100, Poofer::timerExpired, this);
    active = true;
}

void Poofer::run()
{
    if (active) {
        timer.run();
    }
}

void Poofer::timerExpired()
{
    node->setValveCommand(idx, false);
    active = false;
}

void Poofer::timerExpired(void *arg)
{
    ((Poofer*)arg)->timerExpired();
}
