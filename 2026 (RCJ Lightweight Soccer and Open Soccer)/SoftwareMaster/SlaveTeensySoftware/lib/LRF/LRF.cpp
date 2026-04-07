#include "LRF.h"

LRFArray::LRFArray(int s0, int s1, int s2, int s3, int sigPin, int count)
    : _s0(s0), _s1(s1), _s2(s2), _s3(s3), _sigPin(sigPin), _count(count) {
    for (int i = 0; i < _count; i++) {
        // Pass -1 for XSHUT — we control it manually via the mux
        sensors[i] = new VL53L4CD(&Wire, -1);
        active[i] = false;
        distances[i] = 0;
    }
}

void LRFArray::selectChannel(int ch) {
    digitalWrite(_s0, (ch >> 0) & 1);
    digitalWrite(_s1, (ch >> 1) & 1);
    digitalWrite(_s2, (ch >> 2) & 1);
    digitalWrite(_s3, (ch >> 3) & 1);
}

void LRFArray::begin() {
    Wire.begin();

    pinMode(_s0, OUTPUT);
    pinMode(_s1, OUTPUT);
    pinMode(_s2, OUTPUT);
    pinMode(_s3, OUTPUT);
    pinMode(_sigPin, OUTPUT);

    // Pull all XSHUT lines low (reset all sensors)
    for (int i = 0; i < _count; i++) {
        selectChannel(i);
        digitalWrite(_sigPin, LOW);
    }
    delay(10);

    // Bring up each sensor one at a time and assign a unique I2C address
    for (int i = 0; i < _count; i++) {
        selectChannel(i);
        digitalWrite(_sigPin, HIGH);  // XSHUT high → sensor powers up at default address
        delay(10);

        if (sensors[i]->begin() == 0) {
            sensors[i]->VL53L4CD_SetI2CAddress(0x30 + i);
            sensors[i]->InitSensor();
            sensors[i]->VL53L4CD_StartRanging();
            active[i] = true;
        } else {
            digitalWrite(_sigPin, LOW);  // keep failed sensor in reset
        }
    }
}

void LRFArray::update() {
    VL53L4CD_Result_t results;
    uint8_t dataReady = 0;

    for (int i = 0; i < _count; i++) {
        if (active[i]) {
            sensors[i]->VL53L4CD_CheckForDataReady(&dataReady);
            if (dataReady) {
                sensors[i]->VL53L4CD_GetResult(&results);
                distances[i] = results.distance_mm;
                sensors[i]->VL53L4CD_ClearInterrupt();
            }
        } else {
            distances[i] = 8000;  // out of range sentinel
        }
    }
}
