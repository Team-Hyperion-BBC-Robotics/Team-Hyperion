#ifndef LIGHTSYS_H
#define LIGHTSYS_H

#include <Arduino.h>
#include <Pins.h>
#include "Vect.h"
#include <Common.h>

class LightSys
{
public:
    LightSys() {};
    void init();
    void update();
    void calibrate();
    float get_line_direction() { return lineDirection; };
    float get_line_magnitude() { return lineMagnitude; };
    bool onLine = false;

private:
    void read_one(int sNum);
    void read_all();
    Vect calculate_line_dir();

    // Helper to calculate the vector for a specific group of sensors
    Vect process_sensor_group(uint8_t *sensArray, float *angleArray, int size, int &activeCount);
    Common com;

    float lineDirection = 0;
    float lineMagnitude = 0;
    int values[3][16] = {0};
    int trueValues[48] = {0};

    int inPin[4] = {LIGHT_PIN_DIGI_0, LIGHT_PIN_DIGI_1, LIGHT_PIN_DIGI_2, LIGHT_PIN_DIGI_3};
    uint8_t muxIndex[48] = {
        41, 0, 40, 0, 39, 16, 38, 14,
        18, 19, 20, 42, 22, 43, 0, 23,
        29, 28, 27, 26, 45, 44, 25, 24,
        46, 47, 32, 33, 30, 31, 0, 1, // this and above of this array does not work
        8, 9, 10, 11, 12, 13, 37, 34,
        2, 3, 4, 5, 6, 7, 36, 35};

    int thresholds[48] = {0};

    uint8_t frontSens[4] = {42, 43, 44, 45};
    uint8_t rightSens[4] = {32, 33, 46, 47};
    uint8_t backSens[4] = {34, 35, 36, 37};
    uint8_t leftSens[4] = {38, 39, 40, 41};

    float frontAngles[4] = {75.0, 82.5, 97.5, 105.0};
    float rightAngles[4] = {352.5, 352.5 - 7.5, 7.5, 15};
    float backAngles[4] = {285.0, 277.5, 262.5, 255.0};
    float leftAngles[4] = {165.0, 172.5, 187.5, 195.0};
};

#endif