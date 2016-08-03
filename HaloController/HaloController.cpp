// Krawler Halo Controller

#include <Wire.h>
#include <SimpleTimer.h>
#include <CRC8.h>
#include <Button.h>
#include <Adafruit_NeoPixel.h>

#include "HaloNode.h"
#include "Poofer.h"
#include "PooferSequencer.h"

#define NEO_PX_PIN        (2)
#define START_SEQ_LED_PIN (3)
#define START_SEQ_BTN_PIN (4)
#define AC_ENABLE_LED_PIN (5)
#define AC_ENABLE_BTN_PIN (6)
#define SPD_POT_PIN       (A0)
#define SEQ_SEL_POT_PIN   (A1)

#define HALO_NODE_LEFT_ADDR  (8)
#define HALO_NODE_RIGHT_ADDR (9)

#define NUM_POOFER_SEQUENCES (2)

#define NUM_HALO_NODES       (2)
#define NUM_POOFERS          (NUM_HALO_NODES * NUM_POOFERS_PER_NODE)

#define NUM_DISPLAY_PIXELS    (24)
#define NUM_PIXELS_PER_POOFER (NUM_DISPLAY_PIXELS / NUM_POOFERS)

#define COLD_COLOR         (0x0000FF) // blue
#define HOT_INACTIVE_COLOR (0xFF0000) // red
#define HOT_ACTIVE_COLOR   (0xFFFF00) // yellow

SimpleTimer readPotsTimer;
SimpleTimer updateNodesTimer;
SimpleTimer restartSequencePreviewTimer;
SimpleTimer updateSequenceButtonLedTimer;
Button startSequenceButton(START_SEQ_BTN_PIN, BUTTON_PULLUP_INTERNAL);
Button acEnableButton(AC_ENABLE_BTN_PIN, BUTTON_PULLUP_INTERNAL);
Adafruit_NeoPixel display = Adafruit_NeoPixel(NUM_DISPLAY_PIXELS, NEO_PX_PIN, NEO_GRB + NEO_KHZ800);
bool displayUpdated;
int curSeqSelect = -1;
bool acEnabled = false;

HaloNode nodes[NUM_HALO_NODES] = {
    HaloNode(HALO_NODE_LEFT_ADDR),
    HaloNode(HALO_NODE_RIGHT_ADDR)
};

Poofer poofers[NUM_POOFERS] = {
    Poofer(0, &nodes[0], 0),
    Poofer(1, &nodes[0], 1),
    Poofer(2, &nodes[0], 2),
    Poofer(3, &nodes[0], 3),
    Poofer(4, &nodes[1], 0),
    Poofer(5, &nodes[1], 1),
    Poofer(6, &nodes[1], 2),
    Poofer(7, &nodes[1], 3),
};

PooferSequencer sequencer(poofers, NUM_POOFERS);

char const *sequences[NUM_POOFER_SEQUENCES] = {
    // Sequence 1
    "X-------"
    "-X------"
    "--X-----"
    "---X----"
    "----X---"
    "-----X--"
    "------X-"
    "-------X",

    // Sequence 2
    "X---X---"
    "-X---X--"
    "--X---X-"
    "---X---X",
};

void readPots(void *arg)
{
    if (sequencer.isActive() && !sequencer.isPreview()) {
        return;
    }

    float seqIntervalScalar = analogRead(SPD_POT_PIN) / 1024.0f;
    sequencer.setIntervalScalar(seqIntervalScalar);

    int newSeqSelect = analogRead(SEQ_SEL_POT_PIN) / (1024 / NUM_POOFER_SEQUENCES);
    if (curSeqSelect != newSeqSelect) {
        sequencer.startSequencePreview(sequences[curSeqSelect]);
    }
}

void updateNodes(void *arg)
{
    for (int i = 0; i < NUM_HALO_NODES; ++i) {
        nodes[i].enableAc(acEnabled);
        nodes[i].queryStatus();
        nodes[i].sendCommands();
    }
}

uint32_t pooferStateToColor(Poofer::State state)
{
    switch (state) {
        case Poofer::HOT_INACTIVE:
            return HOT_INACTIVE_COLOR;
        case Poofer::HOT_ACTIVE:
            return HOT_ACTIVE_COLOR;
        case Poofer::COLD:
        default:
            return COLD_COLOR;
    }
}

void handlePooferStateChange(int pooferIdx, Poofer::State state)
{
    uint32_t pixelColor = pooferStateToColor(state);

    for (int i = 0; i < NUM_PIXELS_PER_POOFER; ++i) {
        int pixelIdx = (pooferIdx * NUM_PIXELS_PER_POOFER) + i;
        display.setPixelColor(pixelIdx, pixelColor);
    }
    displayUpdated = false;
}

void handleAcEnableButtonPressed(Button& button)
{
    acEnabled = !acEnabled;
    digitalWrite(AC_ENABLE_LED_PIN, acEnabled);
}

void handleStartSequenceButtonPressed(Button& button)
{
    sequencer.startSequence(sequences[curSeqSelect]);
}

void restartSequencePreview(void *arg)
{
    sequencer.startSequencePreview(sequences[0]);
}

void handleSequenceComplete()
{
    restartSequencePreviewTimer.setTimeout(750, restartSequencePreview, NULL);
}

void updateSequenceButtonLed(void *arg)
{
    if (sequencer.isActive() && !sequencer.isPreview()) {
        float val = (exp(sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0;
        analogWrite(START_SEQ_LED_PIN, val);
    }
    else {
        analogWrite(START_SEQ_LED_PIN, 255);
    }
}

void setup()
{
    Wire.begin();
    Serial.begin(9600);

    display.begin();
    display.show();

    pinMode(START_SEQ_LED_PIN, OUTPUT);
    pinMode(AC_ENABLE_LED_PIN, OUTPUT);

    startSequenceButton.clickHandler(handleStartSequenceButtonPressed);
    acEnableButton.clickHandler(handleAcEnableButtonPressed);
    updateNodesTimer.setInterval(100, updateNodes, NULL);
    readPotsTimer.setInterval(100, readPots, NULL);
    updateSequenceButtonLedTimer.setInterval(10, updateSequenceButtonLed, NULL);

    for (int i = 0; i < NUM_POOFERS; ++i) {
        poofers[i].onStateChange(handlePooferStateChange);
    }

    sequencer.onSequenceComplete(handleSequenceComplete);
}

void loop()
{
    startSequenceButton.process();
    acEnableButton.process();
    readPotsTimer.run();
    updateNodesTimer.run();

    if (sequencer.isActive()) {
        sequencer.run();
    }
    else {
        restartSequencePreviewTimer.run();
    }

    if (displayUpdated) {
        display.show();
        displayUpdated = false;
    }
}
