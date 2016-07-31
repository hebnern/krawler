// Krawler Halo Controller

#include <Wire.h>

int x;

void setup()
{
  Wire.begin();
}

void loop()
{
  Wire.beginTransmission(8);
  Wire.write("x is ");
  Wire.write(x);
  Wire.endTransmission();

  x++;
  delay(500);
}
