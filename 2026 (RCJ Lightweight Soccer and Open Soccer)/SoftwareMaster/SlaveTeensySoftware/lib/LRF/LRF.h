#ifndef LRF_SLAVE_H
#define LRF_SLAVE_H

#include <Arduino.h>
#include <Wire.h>
#include <vl53l4cd_class.h>

class LRFArray {
public:
    // s0-s3: mux select pins (LRFMUX0-3), sigPin: mux signal pin (LRFMUXOUT)
    LRFArray(int s0, int s1, int s2, int s3, int sigPin, int count = 16);
    void begin();
    void update();
    uint16_t* get_distances() { return distances; }

private:
    void selectChannel(int ch);

    int _s0, _s1, _s2, _s3, _sigPin;
    int _count;
    uint16_t distances[16];
    bool active[16];
    VL53L4CD* sensors[16];
};

#endif
