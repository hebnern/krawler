// Krawler Halo Controller

#include <Wire.h>
#include <SimpleTimer.h>
#include <CRC8.h>
#include <Button.h>
#include <Adafruit_NeoPixel.h>

#define NEO_PX_PIN        (2)
#define START_SEQ_BTN_PIN (3)
#define SPD_POT_PIN       (A0)
#define SEL_POT_PIN       (A1)

#define HALO_NODE_LEFT_ADDR  (8)
#define HALO_NODE_RIGHT_ADDR (9)

#define STATUS_PKT_SIZE (9)

#define NUM_HALO_NODES       (2)
#define NUM_POOFERS_PER_NODE (4)
#define NUM_POOFERS          (NUM_HALO_NODES * NUM_POOFERS_PER_NODE)

#define NUM_DISPLAY_PIXELS    (24)
#define NUM_PIXELS_PER_POOFER (NUM_DISPLAY_PIXELS / NUM_POOFERS)

#define HSI_HOT_THRESHOLD (500)

#define POOFER_COLD_COLOR         (0)
#define POOFER_HOT_INACTIVE_COLOR (0)
#define POOFER_HOT_ACTIVE_COLOR   (0)

SimpleTimer timer;
int updateNodesTimerId;
int updateDisplayTimerId;

Button startSequenceButton(START_SEQ_BTN_PIN, BUTTON_PULLUP_INTERNAL);

Adafruit_NeoPixel display = Adafruit_NeoPixel(NUM_DISPLAY_PIXELS, NEO_PX_PIN, NEO_GRB + NEO_KHZ800);

class HaloNode
{
public:
    HaloNode(int addr) :
        addr(addr)
    {
    }

    void queryStatus()
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

    void sendCommands()
    {

    }

    uint16_t getHsiTemp(int idx)
    {
        return nodeStatus[idx];
    }

private:
    int addr;
    uint16_t nodeStatus[NUM_POOFERS_PER_NODE];
};

class Poofer
{
public:
    enum Status {
        COLD,
        HOT_INACTIVE,
        HOT_ACTIVE
    };

    Poofer(HaloNode *node, int idx) :
        node(node),
        idx(idx)
    {
        triggerTimerId = timer.setTimeout(100, NULL);
    }

    Status getStatus()
    {
        uint16_t hsiTemp = node->getHsiTemp(idx);
        if (hsiTemp > HSI_HOT_THRESHOLD) {
            return HOT_INACTIVE;
        }
        else {
            return COLD;
        }
    }

    void trigger()
    {
        triggered = true;
        timer.restartTimer(triggerTimerId);
    }

private:
    HaloNode *node;
    int idx;
    bool triggered;
    int triggerTimerId;
};

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

void updateNodes()
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

void updateDisplay()
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
    updateNodesTimerId = timer.setInterval(100, updateNodes);
    updateDisplayTimerId = timer.setInterval(10, updateDisplay);
}

void loop()
{
    startSequenceButton.process();
    timer.run();
}
