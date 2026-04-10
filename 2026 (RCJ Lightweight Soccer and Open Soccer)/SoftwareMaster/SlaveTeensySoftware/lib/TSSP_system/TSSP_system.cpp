#include <TSSP_system.h>

void TsspSystem::init() {
    for (int i = 0; i < TSSP_NUM; i++) {
        pinMode(tsspPins[i], INPUT);
    }
    for (int i = 0; i < TSSP_NUM; i++) {
        xVa[i] = RAD_TO_DEG*sin(i/TSSP_NUM);
        yVa[i] = RAD_TO_DEG*cos(i/TSSP_NUM);
    }
}

void TsspSystem::update() {
    ballStr = 0;
    uint8_t tsspValues[TSSP_NUM] = {0};
    uint8_t tsspSortedValues[TSSP_NUM] = {0};
    uint8_t tsspSortedIndex[TSSP_NUM] = {0}; 

    for (uint8_t i = 0; i < 100; i++) {
        for (uint8_t j = 0; j < TSSP_NUM; j++) {
            tsspValues[j] += 1 - digitalRead(tsspPins[j]);
        }
        delayMicroseconds(10);
    }

    for(uint8_t i = 0; i < TSSP_NUM; i++) {
        Serial.print(tsspValues[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.println();
    int tt = 0;

    for(uint8_t i = 0; i < TSSP_NUM; i++) {
        for(uint8_t j = 0; j < TSSP_NUM; j++) {
            if(tsspValues[i] > tsspSortedValues[j]) {
                if(j <= i) {
                    ARRAYSHIFTDOWN(tsspSortedValues, j, i);
                    ARRAYSHIFTDOWN(tsspSortedIndex, j, i);
                }
                tsspSortedValues[j] = tsspValues[i];
                tsspSortedIndex[j] = i;
                break;
            }
        }
    }
    tt = ( tsspSortedIndex[0] + tsspSortedIndex[1] ) / 2;
    //Serial.println(tt);
    float x = 0.0;
    float y = 0.0;
    for(uint8_t i = 0; i < 4; i++) {
        x += tsspSortedValues[i]*(cos(tsspSortedIndex[i]*(360.0/TSSP_NUM)*RAD_TO_DEG));
        y += tsspSortedValues[i]*(sin(tsspSortedIndex[i]*(360.0/TSSP_NUM)*RAD_TO_DEG));
    }
    x /= 4;
    y /= 4;
    //ballDir = tsspSortedValues[0];
    //Serial.println(tt*360/TSSP_NUM);
    ballStr = ((3 * tsspSortedValues[0]) + (2 * tsspSortedValues[1]) + tsspSortedValues[2] + tsspSortedValues[3]) / 7.0;
    ballDir = (ballStr != 0) ? 360 -fmod((RAD_TO_DEG * (atan2f(y, x)))-90, 360) : 0;
    ballDir = ((tt*360/TSSP_NUM) % 360);

}