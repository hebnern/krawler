// Krawler Halo Node

#include <Wire.h>
#include "SimpleTimer.h"
#include "CRC8.h"

#define BASE_ADDR (8)

#define ADDR_SEL_PIN (2)
#define AC_EN_PIN    (3)
#define VLV_0_EN_PIN (4)
#define VLV_1_EN_PIN (5)
#define VLV_2_EN_PIN (6)
#define VLV_3_EN_PIN (7)

#define HSI_0_TEMP_PIN (A0)
#define HSI_1_TEMP_PIN (A1)
#define HSI_2_TEMP_PIN (A2)
#define HSI_3_TEMP_PIN (A3)

#define CMD_PKT_SIZE (6)

SimpleTimer timer;
int updateHsiTempsTimerId;
int commsWatchdogTimerId;
uint16_t hsi_temps[4];

void disableOutputs()
{
  digitalWrite(AC_EN_PIN, LOW);
  digitalWrite(VLV_0_EN_PIN, LOW);
  digitalWrite(VLV_1_EN_PIN, LOW);
  digitalWrite(VLV_2_EN_PIN, LOW);
  digitalWrite(VLV_3_EN_PIN, LOW);
}

void commsWatchdogTimerExpired()
{
  disableOutputs();
}

void updateHsiTemps()
{
  hsi_temps[0] = analogRead(HSI_0_TEMP_PIN);
  hsi_temps[1] = analogRead(HSI_1_TEMP_PIN);
  hsi_temps[2] = analogRead(HSI_2_TEMP_PIN);
  hsi_temps[3] = analogRead(HSI_3_TEMP_PIN);
}

void requestEvent()
{
  Wire.write((byte*)&hsi_temps, sizeof(hsi_temps));
}

void receiveEvent(int numBytes)
{
  if (numBytes != CMD_PKT_SIZE) {
    return;
  }

  byte commandPacket[CMD_PKT_SIZE];
  for (int i = 0; i < CMD_PKT_SIZE; ++i) {
    commandPacket[i] = Wire.read();
  }

  byte calculatedCRC = CRC8(commandPacket, CMD_PKT_SIZE-1);
  if (commandPacket[5] == calculatedCRC) {
    digitalWrite(AC_EN_PIN, commandPacket[0]);
    digitalWrite(VLV_0_EN_PIN, commandPacket[1]);
    digitalWrite(VLV_1_EN_PIN, commandPacket[2]);
    digitalWrite(VLV_2_EN_PIN, commandPacket[3]);
    digitalWrite(VLV_3_EN_PIN, commandPacket[4]);

    timer.restartTimer(commsWatchdogTimerId);
  }
}

void setup()
{
  pinMode(ADDR_SEL_PIN, INPUT);
  pinMode(AC_EN_PIN, OUTPUT);
  pinMode(VLV_0_EN_PIN, OUTPUT);
  pinMode(VLV_1_EN_PIN, OUTPUT);
  pinMode(VLV_2_EN_PIN, OUTPUT);
  pinMode(VLV_3_EN_PIN, OUTPUT);

  analogReference(INTERNAL);

  int addr = BASE_ADDR | digitalRead(ADDR_SEL_PIN);
  Wire.begin(addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600);

  disableOutputs();

  // Update HSI temps every 100ms
  updateHsiTempsTimerId = timer.setInterval(100, updateHsiTemps);

  // If we don't hear from the master for 1 second, we disable
  // all outputs to prevent valves from remaining open indefinitely
  commsWatchdogTimerId = timer.setTimeout(1000, commsWatchdogTimerExpired);
}

void loop()
{
  timer.run();
}
