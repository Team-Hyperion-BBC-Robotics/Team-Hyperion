#include <Arduino.h>
#include <TSSP_system.h>

void TsspSystem::init() {
    Serial1.begin(115200);
}

void TsspSystem::update() {
    readUartData();
    float relativeDir = (bInfo._dir > 180) ? (bInfo._dir - 360) : bInfo._dir;

    // 2. Calculate the "Strength Factor" (how much influence the distance/signal has)
    float ratio = constrain(bInfo._str / ORBIT_TUNER, 0.0f, 1.0f);
    float strengthFactor = constrain(0.02f * ratio * expf(4.5f * ratio), 0.0f, 1.0f);

    // 3. Calculate the "Angle Shift" (how much to orbit based on the current angle)
    float angleEffect = min(0.4f * expf(0.25f * abs(relativeDir)) - 0.4f, 90.0f);
    float totalOffset = strengthFactor * angleEffect;

    // 4. Determine direction of the orbit and update Movement Info
    float directionAdjustment = (relativeDir < 0) ? -totalOffset : totalOffset;
    mInfo._dir = fmod(bInfo._dir + directionAdjustment, 360.0f);

    // 5. Update Movement Speed (slows down as the orbit offset increases)
    float speedReduction = totalOffset / 90.0f;
    mInfo._spd = BASE_SPEED + (SURGE_SPEED - BASE_SPEED) * (1.0f - speedReduction);
}

void TsspSystem::readUartData() {
    while (Serial1.available()) {
        char c = Serial1.read();
        if (c == '\n') {
            int dirIndex = uartBuffer.indexOf("DIR:");
            int commaIndex = uartBuffer.indexOf(",");
            int strIndex = uartBuffer.indexOf("STR:");

            if (dirIndex != -1 && commaIndex != -1 && strIndex != -1) {
                bInfo._dir = uartBuffer.substring(dirIndex + 4, commaIndex).toFloat();
                bInfo._str = uartBuffer.substring(strIndex + 4).toFloat();
            }
            uartBuffer = "";
        }
        else {
            uartBuffer += c;
        }
    }
}