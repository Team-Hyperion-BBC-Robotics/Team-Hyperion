#include "LRF.h"

LRFArray::LRFArray(int xshutPins[8]) {
    for (int i = 0; i < 8; i++) {
        _pins[i] = xshutPins[i];
        sensors[i] = new VL53L4CD(&Wire, _pins[i]);
        active[i] = false;
        distances[i] = 0;
    }
}

void LRFArray::begin() {
    Wire.begin();
    for (int i = 0; i < 8; i++) {
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], LOW);
    }
    delay(10);

    for (int i = 0; i < 8; i++) {
        digitalWrite(_pins[i], HIGH);
        delay(10);
        if (sensors[i]->begin() == 0) { 
            sensors[i]->VL53L4CD_SetI2CAddress(0x30 + i);  
            sensors[i]->InitSensor();
            sensors[i]->VL53L4CD_StartRanging();
            active[i] = true;
        } else {
            digitalWrite(_pins[i], LOW);
        }
    }
}

void LRFArray::update() {
    VL53L4CD_Result_t results;
    uint8_t dataReady = 0;

    for (int i = 0; i < 8; i++) {
        if (active[i]) {
            sensors[i]->VL53L4CD_CheckForDataReady(&dataReady);
            if (dataReady) {
                sensors[i]->VL53L4CD_GetResult(&results);
                distances[i] = results.distance_mm;
                sensors[i]->VL53L4CD_ClearInterrupt();
            }
        } else {
            distances[i] = 8000; // Out of range value
        }
    }
}