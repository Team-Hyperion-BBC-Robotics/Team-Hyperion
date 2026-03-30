#ifndef LIGHT_SYSTEM_H
#define LIGHT_SYSTEM_H

#include <Arduino.h>
#include <Pins.h>
#include <Configuration.h>
#include <Common.h>
#include <EEPROM.h>

class LightSystem {
public:
    void init();
    float get_line_dir() { return lineDirection; };
    float get_line_state() { return lineOffset; };
    void update();
private:
    // Defines
        #define NUM_LS 32
        #define OUTER_NUM_LS 16
        #define LS_FLIP_THRESH 90
    // Functions
        int inner_read(int sNum);
        void calculate_line_dir();
        void calculate_line_state();
    // Variables
        int inPin[4] = {LIGHT_PIN_DIGI_0, LIGHT_PIN_DIGI_1, LIGHT_PIN_DIGI_2,
                            LIGHT_PIN_DIGI_3};
        float lineDirection = 0.0;
        float prevLineDirection = 0.0;
        int lineState = 0;
        float lineOffset = 0.0;
        float sensorCoords[NUM_LS][2] = {0.0};
        int initialClusterPointStart = 0;
        int secondClusterPointStart = 0;
        int initialClusterPointEnd = 0;
        int secondClusterPointEnd = 0;
    // Refs
        Common com;
};

#endif