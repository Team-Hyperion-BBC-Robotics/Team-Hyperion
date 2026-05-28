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

#define TSSP_THRESHOLD 20

void TsspSystem::update() {
    uint8_t rawValues[TSSP_NUM] = {0};
    uint8_t read = 0;
    
    // 1. Data Collection (Same as your original)
    for (uint8_t i = 0; i < 100; i++) {
        for (uint8_t j = 0; j < TSSP_NUM; j++) {
            rawValues[j] += 1 - digitalRead(tsspPins[j]);
        }
        delayMicroseconds(10);
    }

    // for (uint8_t i=0; i < TSSP_NUM; i++) {
    //     Serial.print(rawValues[i]);
    //     Serial.print("\t");
    // }
    // Serial.println("");

    for (uint8_t j = 0; j < TSSP_NUM; j++) {
        if (rawValues[j] > 0) {
            read += 1;
        }
    }

    for (uint8_t i = 0; i < TSSP_NUM; i++) {
        if(rawValues[i] != 0 || rawValues[i] != 255) {
            rawValues[i] += tsspOffset[i];
        }
    }

    float clusterX = 0, clusterY = 0;
    int activeSensors = 0;

    // 2. Clustering Logic
    // We treat the sensors as a circular array
    for (int i = 0; i < TSSP_NUM; i++) {
        if (rawValues[i] > TSSP_THRESHOLD) {
            float angle = (i * 360.0 / TSSP_NUM) * DEG_TO_RAD;
            
            // Add component vectors based on intensity
            clusterX += rawValues[i] * cos(angle);
            clusterY += rawValues[i] * sin(angle);
            
            activeSensors++;
        }
    }

    // 3. Final Calculation
    if (activeSensors > 0) {
        // Average direction
        // ballDir = floatMod(atan2(clusterY, clusterX) * RAD_TO_DEG, 360);
        ballDir = floatMod(180-((atan2f(clusterY, clusterX))*RAD_TO_DEG)-90, 360);
        
        // Normalize magnitude (e.g., average intensity per active sensor)
    } else {
        ballDir = 0;
        ballStr = 0;
    }

    // ballStr = read;
    float rawStr = (float)read;
    smoothedBallStr = (rawStr * alpha) + (smoothedBallStr * (1.0 - alpha));
    ballStr = (read!=0)?(uint8_t)smoothedBallStr:0;
    Serial.print(ballDir);
    Serial.print("\t");
    Serial.println(ballStr);
}

float TsspSystem::floatMod(float x, float m){
    float r = fmod(x, m);
    return r<0 ? r+m : r;
}