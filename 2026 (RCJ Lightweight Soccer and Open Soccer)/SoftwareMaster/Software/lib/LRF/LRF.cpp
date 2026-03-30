#include "LRF.h"

LRFProcessor::LRFProcessor() {
    for(int i=0; i<8; i++) {
        robotCoords[i][0] = 0;
        robotCoords[i][1] = 0;
        distances[i] = 8000;
    }
}

void LRFProcessor::update(Stream &serialPort, float imuHeading) {
    while (serialPort.available()) {
        char c = (char)serialPort.read();
        if (c == '\n') {
            if (buffer.startsWith("LRF:")) {
                sscanf(buffer.c_str(), "LRF:%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu", 
                       &distances[0], &distances[1], &distances[2], &distances[3], 
                       &distances[4], &distances[5], &distances[6], &distances[7]);
                calculate_position(imuHeading);
                calculate_closest_obstacle();
            }
            buffer = ""; // Clear for next line
        } else {
            buffer += c;
        }
    }
}

void LRFProcessor::calculate_position(float imuHeading) {
    float cosH = cos(imuHeading);

    // --- Y-AXIS Logic ---
    float frontY = distances[0] * cosH; 
    float backY = distances[4] * cosH;
    float verifyFrontY = ((distances[1] + distances[7]) * 0.7071) * cosH; 

    if (abs((frontY + backY) - FIELD_Y_MAX) < ROBOT_TOLERANCE) {
        selfY = backY; 
    } else {
        selfY = max(backY, FIELD_Y_MAX - max(frontY, verifyFrontY));
    }

    // --- X-AXIS Logic ---
    float leftX = distances[6] * cosH;
    float rightX = distances[2] * cosH;
    float verifyLeftX = ((distances[5] + distances[7]) * 0.7071) * cosH;

    if (abs((leftX + rightX) - FIELD_X_MAX) < ROBOT_TOLERANCE) {
        selfX = leftX;
    } else {
        selfX = max(leftX, FIELD_X_MAX - max(rightX, verifyLeftX));
    }

    // --- ROBOT DETECTION ---
    for (int i = 0; i < 8; i++) {
        float sensorAngle = imuHeading + (i * 45.0 * DEG_TO_RAD);
        float sCos = cos(sensorAngle);
        float sSin = sin(sensorAngle);

        float distToWall = (sCos > 0) ? (FIELD_Y_MAX - selfY) / sCos : -selfY / sCos;
        float distToSide = (sSin > 0) ? (FIELD_X_MAX - selfX) / sSin : -selfX / sSin;
        float expectedWall = min(abs(distToWall), abs(distToSide));

        if (distances[i] < (expectedWall - ROBOT_TOLERANCE) && distances[i] > 50) {
            robotCoords[i][0] = selfX + (distances[i] * sSin);
            robotCoords[i][1] = selfY + (distances[i] * sCos);
        } else {
            robotCoords[i][0] = 0;
            robotCoords[i][1] = 0;
        }
    }
}

void LRFProcessor::localise_to(int x, int y) {
    localiseMag = sqrt(pow((selfX - x), 2) + pow((selfY - y), 2));
    localiseDir = atan2f(y - selfY, y - selfX) * RAD_TO_DEG;
}

void LRFProcessor::calculate_closest_obstacle() {
    float closestDist = 9999.0;
    closestObsAng = -999.0;
    for (int i = 0; i < 8; i++) {
        if (robotCoords[i][0] != 0 || robotCoords[i][1] != 0) {
            float dx = robotCoords[i][0] - selfX;
            float dy = robotCoords[i][1] - selfY;
            float d = sqrt(dx*dx + dy*dy);
            
            if (d < closestDist) {
                closestDist = d;
                closestObsAng = atan2(dx, dy) * RAD_TO_DEG;
            }
        }
    }
}

void LRFProcessor::calculate_wall_score(float virtualX, float virtualY) {
    if(selfX >= 0) {
        wallMag = sqrt(pow((virtualX - selfX), 2) + pow((virtualY - selfY), 2)); // RIGHT MAG
        wallDir = atan2(virtualY - selfY, virtualX - selfX) * RAD_TO_DEG; // RIGHT DIR
    } else {
        wallMag = sqrt(pow((-virtualX - selfX), 2) + pow((virtualY - selfY), 2)); // LEFT MAG
        wallDir = atan2(virtualY - selfY, -virtualX - selfX) * RAD_TO_DEG; // LEFT DIR
    }
    // WHAT THIS IS FOR: GOAL SCORING OFF THE WALL, PASSING BETWEEN ROBOTS OFF THE 
    // WALL (BASICALLY YOU ARE ABLE TO KICK OFF THE WALL TOO ANY POINT ON THE FIELD)
}