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
    // 1. Kill power to all sensors
    for (int i = 0; i < 8; i++) {
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], LOW);
    }
    delay(10);

    for (int i = 0; i < 8; i++) {
        digitalWrite(_pins[i], HIGH); // Wake up ONE sensor
        delay(10);

        // Start communication at the DEFAULT address (0x29)
        if (sensors[i]->begin() == 0) { 
            // 2. Change the address immediately
            uint8_t newAddress = (0x30 + i) << 1; // Note: Some libs want 8-bit addr
            sensors[i]->VL53L4CD_SetI2CAddress(0x30 + i); 
            
            // IMPORTANT: Some libraries require you to re-init the object 
            // or manually set the new address in the class instance here.
            // sensors[i]->address = 0x30 + i; 

            sensors[i]->InitSensor();
            sensors[i]->VL53L4CD_StartRanging();
            
            active[i] = true;
            Serial.printf("Sensor %d FOUND and moved to 0x%X\n", i, 0x30 + i);
        } else {
            Serial.printf("Sensor %d NOT FOUND\n", i);
            digitalWrite(_pins[i], LOW);
        }
    }
}

void LRFArray::update() {
    VL53L4CD_Result_t results;
    uint8_t dataReady = 0;

    for (int i = 0; i < 8; i++) {
        if (active[i]) {
            // CHECK: Is there actually data waiting for us?
            sensors[i]->VL53L4CD_CheckForDataReady(&dataReady);
            
            if (dataReady) {
                sensors[i]->VL53L4CD_GetResult(&results);
                distances[i] = results.distance_mm;
                sensors[i]->VL53L4CD_ClearInterrupt();
            }
            // If dataReady is 0, we just skip this frame for this sensor
        } else {
            distances[i] = 0;
        }
    }
}