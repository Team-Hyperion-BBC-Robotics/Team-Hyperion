#include <Arduino.h>
#include <TSSP_system.h>
#include "LRF.h"

#define OPEN true // Set to true for LRF, false for TSSP

TsspSystem tssp;
int xshutPins[8] = {LRFMUX0, LRFMUX1, LRFMUX2, LRFMUX3, LRFMUXOUT, 8};
LRFArray lrf(xshutPins);

void setup() {
    Serial1.begin(115200);
    if (OPEN) {
        lrf.begin();
    } else {
        tssp.init();
    }
}

void loop() {
    if (OPEN) {
        lrf.update();
        uint16_t* dists = lrf.get_distances();
        Serial1.print("LRF:");
        for(int i = 0; i < 8; i++) {
            Serial1.print(dists[i]);
            if(i < 7) Serial1.print(",");
        }
        Serial1.println();
    } else {
        tssp.update();
        Serial1.printf("DIR:%.2f,STR:%.2f\n", tssp.get_ball_dir(), tssp.get_ball_str());
    }
    delay(10); // Small stability delay
}