#ifndef LRF_SLAVE_H
#define LRF_SLAVE_H

#include <Arduino.h>
#include <Wire.h>
#include <vl53l4cd_class.h>

class LRFArray {
public:
    LRFArray(int xshutPins[8]);
    void begin();
    void update();
    uint16_t* get_distances() { return distances; }

private:
    uint16_t distances[8];
    int _pins[8];
    bool active[8];
    VL53L4CD* sensors[8];
};

#endif