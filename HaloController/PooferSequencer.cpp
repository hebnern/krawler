
#include "PooferSequencer.h"

#define SEQUENCE_INTERVAL (200)

PooferSequencer::PooferSequencer(Poofer *poofers, int numPoofers) :
    poofers(poofers),
    numPoofers(numPoofers)
{
}

void PooferSequencer::onSequenceComplete(SequenceCompleteCallback sequenceCompleteCallback)
{
    this->sequenceCompleteCallback = sequenceCompleteCallback;
}

void PooferSequencer::startSequencePreview(const char* sequence)
{
    startSequence(sequence, true);
}

void PooferSequencer::startSequence(const char* sequence)
{
    startSequence(sequence, false);
}

void PooferSequencer::startSequence(const char* sequence, bool preview)
{
    this->numSteps = strlen(sequence) / numPoofers;
    this->sequence = sequence;
    this->preview = preview;
    this->active = true;
    setStep(0);
}

void PooferSequencer::run()
{
    for (int i = 0; i < numPoofers; ++i) {
        poofers[i].run();
    }
    timer.run();
}

void PooferSequencer::setStep(int step)
{
    curStep = step;
    timer.setTimeout(SEQUENCE_INTERVAL, PooferSequencer::timerExpired, this);
    char const *curStepStr = &sequence[curStep * numPoofers];
    for (int i = 0; i < numPoofers; ++i) {
        if (curStepStr[i] == 'X') {
            poofers[i].trigger(preview);
        }
    }
}

bool PooferSequencer::isActive()
{
    return active;
}

bool PooferSequencer::hasMoreSteps()
{
    return curStep < (numSteps - 1);
}

void PooferSequencer::timerExpired()
{
    if (hasMoreSteps()) {
        setStep(curStep + 1);
    }
    else if (sequenceCompleteCallback) {
        sequenceCompleteCallback();
        active = false;
    }
}

void PooferSequencer::timerExpired(void *arg)
{
    ((PooferSequencer*)arg)->timerExpired();
}
