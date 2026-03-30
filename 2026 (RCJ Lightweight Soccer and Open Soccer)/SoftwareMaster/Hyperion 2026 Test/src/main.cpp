#include <Arduino.h>
#include <LRF.h>

int lrfPins[8] = {15, 3, 4, 5, 6, 7, 8, 9};
LRFArray lrf(lrfPins);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // Set to 400kHz (Standard Fast Mode)
  
  delay(1000); // Give the Serial monitor time to open
  lrf.begin();
}

void loop() {
  lrf.update();
  Serial.print("Dist: ");
  Serial.println(lrf.distances[0]);
  delay(50);
}