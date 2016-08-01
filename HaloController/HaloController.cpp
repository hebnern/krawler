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
#define START_SEQ_BTN_PIN (3)
#define SPD_POT_PIN       (A0)
#define SEL_POT_PIN       (A1)

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

SimpleTimer updateNodesTimer;
SimpleTimer restartSequencePreviewTimer;
Button startSequenceButton(START_SEQ_BTN_PIN, BUTTON_PULLUP_INTERNAL);
Adafruit_NeoPixel display = Adafruit_NeoPixel(NUM_DISPLAY_PIXELS, NEO_PX_PIN, NEO_GRB + NEO_KHZ800);
bool displayUpdated;

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

void updateNodes(void *arg)
{
    for (int i = 0; i < NUM_HALO_NODES; ++i) {
        nodes[i].queryStatus();
        nodes[i].sendCommands();
    }
}

int pooferStateToColor(Poofer::State state)
{
    switch (state) {
        case Poofer::COLD:
            return COLD_COLOR;
        case Poofer::HOT_INACTIVE:
            return HOT_INACTIVE_COLOR;
        case Poofer::HOT_ACTIVE:
            return HOT_ACTIVE_COLOR;
    }
}

void handlePooferStateChange(int pooferIdx, Poofer::State state)
{
    int pixelColor = pooferStateToColor(state);

    for (int i = 0; i < NUM_PIXELS_PER_POOFER; ++i) {
        int pixelIdx = (pooferIdx * NUM_PIXELS_PER_POOFER) + i;
        display.setPixelColor(pixelIdx, pixelColor);
    }
    displayUpdated = false;
}

void handleStartSequenceButtonPressed(Button& button)
{
    sequencer.startSequence(sequences[0]);
}

void restartSequencePreview(void *arg)
{
    sequencer.startSequencePreview(sequences[0]);
}

void handleSequenceComplete()
{
    restartSequencePreviewTimer.setTimeout(750, restartSequencePreview, NULL);
}

void setup()
{
    Wire.begin();
    Serial.begin(9600);

    display.begin();
    display.show();

    startSequenceButton.clickHandler(handleStartSequenceButtonPressed);
    updateNodesTimer.setInterval(100, updateNodes, NULL);

    for (int i = 0; i < NUM_POOFERS; ++i) {
        poofers[i].onStateChange(handlePooferStateChange);
    }

    sequencer.onSequenceComplete(handleSequenceComplete);
    sequencer.startSequencePreview(sequences[0]);
}

void loop()
{
    startSequenceButton.process();
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
