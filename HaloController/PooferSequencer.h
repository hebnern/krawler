
#pragma once

#include "Poofer.h"
#include <SimpleTimer.h>

class PooferSequencer
{
public:
    typedef void (*SequenceCompleteCallback)();

    PooferSequencer(Poofer *poofers, int numPoofers);

    void onSequenceComplete(SequenceCompleteCallback sequenceCompleteCallback);
    void setIntervalScalar(float seqIntervalScalar);
    void startSequencePreview(const char* sequence);
    void startSequence(const char* sequence);
    bool isActive();
    bool isPreview();
    void run();

private:
    void startSequence(const char* sequence, bool preview);
    void setStep(int step);
    bool hasMoreSteps();
    void timerExpired();
    static void timerExpired(void *arg);

    SequenceCompleteCallback sequenceCompleteCallback;
    Poofer *poofers;
    int numPoofers;
    bool preview;
    int curStep;
    int numSteps;
    SimpleTimer timer;
    const char* sequence;
    bool active;
    int interval;
};
