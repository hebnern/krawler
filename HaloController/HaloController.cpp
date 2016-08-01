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

#define COLD_COLOR         (0) // TODO blue
#define HOT_INACTIVE_COLOR (0) // TODO red
#define HOT_ACTIVE_COLOR   (0) // TODO yellow

#define SEQUENCE_INTERVAL (200)

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

class PooferSequence
{
public:
    PooferSequence(
        char const *step0=NULL,
        char const *step1=NULL,
        char const *step2=NULL,
        char const *step3=NULL,
        char const *step4=NULL,
        char const *step5=NULL,
        char const *step6=NULL,
        char const *step7=NULL)
    {
        steps[0] = step0;
        steps[1] = step1;
        steps[2] = step2;
        steps[3] = step3;
        steps[4] = step4;
        steps[5] = step5;
        steps[6] = step6;
        steps[7] = step7;
    }

    void start()
    {
        previewOnly = false;
        setStep(0);
    }

    void startPreview()
    {
        previewOnly = true;
        setStep(0);
    }

    void run()
    {
        timer.run();
    }

private:
    void setStep(int step)
    {
        curStep = step;
        timer.setTimeout(SEQUENCE_INTERVAL, PooferSequence::timerExpired, this);
        for (int i = 0; i < NUM_POOFERS; ++i) {
            if (steps[curStep][i] == 'X') {
                poofers[i].trigger(previewOnly);
            }
        }
    }

    void timerExpired()
    {
        setStep(curStep + 1);
    }

    static void timerExpired(void *arg)
    {
        ((PooferSequence*)arg)->timerExpired();
    }

    bool previewOnly;
    int curStep;
    char const *steps[NUM_POOFERS];
    SimpleTimer timer;
};

PooferSequence sequences[] = {
    PooferSequence(
        "X-------",
        "-X------",
        "--X-----",
        "---X----",
        "----X---",
        "-----X--",
        "------X-",
        "-------X"),
    PooferSequence(
        "X-------",
        "-X------",
        "--X-----",
        "---X----",
        "----X---",
        "-----X--",
        "------X-",
        "-------X"),
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
            return COLD_COLOR;
        case Poofer::HOT_INACTIVE:
            return HOT_INACTIVE_COLOR;
        case Poofer::HOT_ACTIVE:
            return HOT_ACTIVE_COLOR;
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
    sequences[0].start();
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

    sequences[0].startPreview();
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
