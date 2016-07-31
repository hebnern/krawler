// Krawler Halo Controller

#include <Wire.h>
#include <SimpleTimer.h>
#include <CRC8.h>
#include <Button.h>
#include <Adafruit_NeoPixel.h>

#include "HaloNode.h"
#include "Poofer.h"

#define NEO_PX_PIN        (2)
#define START_SEQ_BTN_PIN (3)
#define SPD_POT_PIN       (A0)
#define SEL_POT_PIN       (A1)

#define HALO_NODE_LEFT_ADDR  (8)
#define HALO_NODE_RIGHT_ADDR (9)

#define NUM_HALO_NODES       (2)
#define NUM_POOFERS          (NUM_HALO_NODES * NUM_POOFERS_PER_NODE)

#define NUM_DISPLAY_PIXELS    (24)
#define NUM_PIXELS_PER_POOFER (NUM_DISPLAY_PIXELS / NUM_POOFERS)

#define POOFER_COLD_COLOR         (0) // TODO blue
#define POOFER_HOT_INACTIVE_COLOR (0) // TODO red
#define POOFER_HOT_ACTIVE_COLOR   (0) // TODO yellow

SimpleTimer updateNodesTimer;
SimpleTimer updateDisplayTimer;
Button startSequenceButton(START_SEQ_BTN_PIN, BUTTON_PULLUP_INTERNAL);
Adafruit_NeoPixel display = Adafruit_NeoPixel(NUM_DISPLAY_PIXELS, NEO_PX_PIN, NEO_GRB + NEO_KHZ800);

HaloNode nodes[NUM_HALO_NODES] = {
    HaloNode(HALO_NODE_LEFT_ADDR),
    HaloNode(HALO_NODE_RIGHT_ADDR)
};

Poofer poofers[NUM_POOFERS] = {
    Poofer(&nodes[0], 0),
    Poofer(&nodes[0], 1),
    Poofer(&nodes[0], 2),
    Poofer(&nodes[0], 3),
    Poofer(&nodes[1], 0),
    Poofer(&nodes[1], 1),
    Poofer(&nodes[1], 2),
    Poofer(&nodes[1], 3),
};

void updateNodes(void *arg)
{
    for (int i = 0; i < NUM_HALO_NODES; ++i) {
        nodes[i].queryStatus();
        nodes[i].sendCommands();
    }
}

int pooferStatusToColor(Poofer::Status status)
{
    switch (status) {
        case Poofer::COLD:
            return POOFER_COLD_COLOR;
        case Poofer::HOT_INACTIVE:
            return POOFER_HOT_INACTIVE_COLOR;
        case Poofer::HOT_ACTIVE:
            return POOFER_HOT_ACTIVE_COLOR;
    }
}

void updateDisplay(void *arg)
{
    for (int i = 0; i < NUM_POOFERS; ++i) {
        Poofer& poofer = poofers[i];
        int pixelColor = pooferStatusToColor(poofer.getStatus());

        for (int j = 0; j < NUM_PIXELS_PER_POOFER; ++i) {
            int pixelIdx = (i * NUM_PIXELS_PER_POOFER) + j;
            display.setPixelColor(pixelIdx, pixelColor);
        }
    }
    display.show();
}

void startSequence(Button& button)
{

}

void setup()
{
    Wire.begin();
    Serial.begin(9600);

    display.begin();
    display.show();

    startSequenceButton.clickHandler(startSequence);
    updateNodesTimer.setInterval(100, updateNodes, NULL);
    updateDisplayTimer.setInterval(10, updateDisplay, NULL);
}

void loop()
{
    startSequenceButton.process();
    updateNodesTimer.run();
    updateDisplayTimer.run();
    for (int i = 0; i < NUM_POOFERS; ++i) {
        poofers[i].run();
    }
}
