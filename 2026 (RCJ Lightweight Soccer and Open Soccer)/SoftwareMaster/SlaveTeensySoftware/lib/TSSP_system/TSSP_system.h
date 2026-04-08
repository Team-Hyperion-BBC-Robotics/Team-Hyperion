#ifndef TSSP_SYSTEM_H
#define TSSP_SYSTEM_H

#include <Arduino.h>
#include <Pins.h>

class TsspSystem {
public:
    void init();
    void update();

    float get_ball_dir() const { return ballDir; }
    float get_ball_str() const { return ballStr; }
private:
    #define TSSP_NUM 24
    uint8_t tsspPins[TSSP_NUM] = {T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24};
    float ballDir = 0;
    float ballStr = 0;
    float xVa[TSSP_NUM] = {0.0};
    float yVa[TSSP_NUM] = {0.0};

    #define SENSOR_BROKEN_HIGH 255
    #define SENSOR_BROKEN_LOW 0 

    #define ARRAYSHIFTDOWN(a, lower, upper){          \
        if (upper == (sizeof(a)/sizeof(a[0])) - 1){   \
            for (int q = upper - 1; q >= lower; q--){ \
                *(a + q + 1) = *(a + q); }            \
        } else{                                       \
            for (int q = upper; q >= lower; q--){     \
                *(a + q + 1) = *(a + q); }}}
};

#endif