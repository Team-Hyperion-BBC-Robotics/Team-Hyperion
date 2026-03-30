#ifndef TSSP_SYSTEM_H
#define TSSP_SYSTEM_H

#include <Arduino.h>

class TsspSystem {
public:
    void init();
    void update();

    float get_ball_dir() const { return ballDir; }
    float get_ball_str() const { return ballStr; }
private:
    #define TSSP_NUM 16
    uint8_t tsspPins[TSSP_NUM] = {0};
    float ballDir = 0;
    float ballStr = 0;
    float xVa[TSSP_NUM] = {0.0};
    float yVa[TSSP_NUM] = {0.0};

    #define ARRAYSHIFTDOWN(a, lower, upper){          \
        if (upper == (sizeof(a)/sizeof(a[0])) - 1){   \
            for (int q = upper - 1; q >= lower; q--){ \
                *(a + q + 1) = *(a + q); }            \
        } else{                                       \
            for (int q = upper; q >= lower; q--){     \
                *(a + q + 1) = *(a + q); }}}
};

#endif