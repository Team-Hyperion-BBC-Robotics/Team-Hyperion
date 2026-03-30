#ifndef LRF_H
#define LRF_H

#include <Arduino.h>
#include <Wire.h>
#include <vl53l4cd_class.h>

class LRFArray {
public:
    LRFArray(int xshutPins[8]);
    void begin();
    void update();
    uint16_t distances[8];
    bool active[8]; // Tracks which of the 8 sensors are actually plugged in

private:
    VL53L4CD* sensors[8];
    int _pins[8];
};

#endif