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
        int outter_read(int sNum);
        void calculate_line_dir();
        void calculate_line_dir_outter();
        void calculate_line_state();
    // Variables
        int inPin[4] = {LIGHT_PIN_DIGI_0, LIGHT_PIN_DIGI_1, LIGHT_PIN_DIGI_2,
                            LIGHT_PIN_DIGI_3};
        int muxIndex[48] = {
            41, 0, 40, 0, 39, 16, 38, 14,
            18, 19, 20, 42, 22, 43, 0, 23,
            29, 28, 27, 26, 45, 44, 25, 24,
            46, 47, 32, 33, 30, 31, 0, 1, // this and above of this array does not work
            8, 9, 10, 11, 12, 13, 37, 34,
            2, 3, 4, 5, 6, 7, 36, 35};
        
        float lineDirection = 0.0;
        float prevLineDirection = 0.0;
        int lineState = 0;
        float lineOffset = 0.0;
        float sensorCoords[NUM_LS][2] = {0.0};
        int initialClusterPointStart = 0;
        int secondClusterPointStart = 0;
        int initialClusterPointEnd = 0;
        int secondClusterPointEnd = 0;
    // Refs // https://team-hyperion-bbc-robotics.github.io/
        Common com;
};

#endif