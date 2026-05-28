#include "LightSys.h"

void LightSys::init() {
    for (uint8_t i = 0; i < 4; i++) {
        pinMode(inPin[i], OUTPUT);
    }
    pinMode(LIGHT_PIN, INPUT);
    pinMode(LIGHT_PIN2, INPUT);
    pinMode(LIGHT_PIN3, INPUT);
    calibrate();
}

void LightSys::calibrate() {
    read_all(); 
    for (uint8_t i = 0; i < 48; i++) {
        thresholds[i] = trueValues[i] + 400;
    }
}

void LightSys::read_one(int sNum) {
    for (uint8_t i = 0; i < 4; i++) {
        digitalWrite(inPin[i], (sNum >> i) & 0x01);
    }  
    values[0][sNum] = analogRead(LIGHT_PIN);
    values[1][sNum] = analogRead(LIGHT_PIN2);
    values[2][sNum] = analogRead(LIGHT_PIN3);
}

void LightSys::read_all() {
    for(uint8_t i = 0; i < 16; i++) {
        read_one(i);
    }

    for (uint8_t j = 0; j < 3; j++) {
        for (uint8_t i = 0; i < 16; i++) {
            trueValues[muxIndex[(j * 16) + i]] = values[j][i];
        }
    }
}

void LightSys::update() {
    read_all();
    // 2, 3, 4, 5
    if(trueValues[2+32] > 1000) {
        trueValues[2+32] = 0;
    }
    if(trueValues[3+32] > 1000) {
        trueValues[3+32] = 0;
    }
    if(trueValues[4+32] > 1000) {
        trueValues[4+32] = 0;
    }
    if(trueValues[5+32] > 1000) {
        trueValues[5+32] = 0;
    }

    // for(int i = 0; i < 16; i++) {
    //     Serial.print(trueValues[i+32]);
    //     // trueValues[i+32] > thresholds[i+32]
    //     Serial.print("\t");
    // }
    // Serial.println("----------------------------------");
    // Serial.println(analogRead(10+32));
    //index --> actual
    //41 --> 32
    //

    for(int i = 0; i < 16; i++) {
        if(trueValues[i+32] < thresholds[i+32]) {
            onLine = false;
        } else {
            onLine = true;
            break;
        }
    }
    
    Vect lDir = calculate_line_dir(); 
    
    lineDirection = com.floatMod(90.0f - (lDir.angle() * RAD_TO_DEG), 360.0f); 
    lineMagnitude = lDir.mag();
}

Vect LightSys::process_sensor_group(uint8_t* sensArray, float* angleArray, int size, int& activeCount) {
    Vect groupVect(0, 0);
    
    for (int i = 0; i < size; i++) {
        if (trueValues[sensArray[i]] > thresholds[sensArray[i]]) {
            float angle_rad = angleArray[i] * DEG_TO_RAD;
            Vect sensorVector(cos(angle_rad), sin(angle_rad));
            groupVect = groupVect + sensorVector;
            activeCount++;
        }
    }
    return groupVect;
}

Vect LightSys::calculate_line_dir() {
    Vect lineDir(0, 0);
    int activeSensors = 0;
    lineDir = lineDir + process_sensor_group(frontSens, frontAngles, 4, activeSensors);
    lineDir = lineDir + process_sensor_group(rightSens, rightAngles, 4, activeSensors);
    lineDir = lineDir + process_sensor_group(backSens, backAngles, 4, activeSensors);
    lineDir = lineDir + process_sensor_group(leftSens, leftAngles, 4, activeSensors);
    if (activeSensors > 0) {
        lineDir.unit_vect();
    }
    return lineDir;
}