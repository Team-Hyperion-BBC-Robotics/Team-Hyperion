#include <TSSP_system.h>

void TsspSystem::init()
{
    for (int i = 0; i < TSSP_NUM; i++)
    {
        pinMode(tsspPins[i], INPUT);
    }
    // Precompute unit vectors for each sensor
    // Index 0 is at 90 degrees, wraps around (index 1 is 75°, index 2 is 60°, etc.)
    for(uint8_t i = 0; i < TSSP_NUM; i++) {
        float angle = 90.0f - (i * (360.0f / TSSP_NUM));
        xVa[i] = cosf(angle * DEG_TO_RAD);
        yVa[i] = sinf(angle * DEG_TO_RAD);
    }
}

void TsspSystem::update()
{
    ballStr = 0;
    uint8_t tsspValues[TSSP_NUM] = {0};
    uint8_t tsspSortedValues[TSSP_NUM] = {0};
    uint8_t tsspSortedIndex[TSSP_NUM] = {0};

    for (int i = 0; i < 255; i++)
    {
        for (int j = 0; j < TSSP_NUM; j++)
        {
            tsspValues[j] += 1 - digitalRead(tsspPins[j]);
        }
        delayMicroseconds(3);
    }

    Serial.println();
    for (int j = 0; j < TSSP_NUM; j++)
    {
        Serial.print(tsspValues[j]);
        Serial.print("\t");
    }

    uint8_t validCount = 0;
    for (uint8_t i = 0; i < TSSP_NUM && validCount < 4; i++)
    {
        if (tsspValues[i] >= 250 || tsspValues[i] <= 5)
            continue;

        for (uint8_t j = 0; j < TSSP_NUM; j++)
        {
            if (tsspValues[i] > tsspSortedValues[j])
            {
                if (j <= i)
                {
                    ARRAYSHIFTDOWN(tsspSortedValues, j, i);
                    ARRAYSHIFTDOWN(tsspSortedIndex, j, i);
                }
                tsspSortedValues[j] = tsspValues[i];
                tsspSortedIndex[j] = i;
                break;
            }
        }
        validCount++;
    }

    float x = 0.0f;
    float y = 0.0f;
    uint8_t sensorsUsed = 0;

    for (uint8_t i = 0; i < 4; i++)
    {
        if (tsspSortedValues[i] > 0)
        {
            x += tsspSortedValues[i] * xVa[tsspSortedIndex[i]];
            y += tsspSortedValues[i] * yVa[tsspSortedIndex[i]];
            sensorsUsed++;
        }
    }

    if (sensorsUsed > 0)
    {
        x /= sensorsUsed;
        y /= sensorsUsed;
    }

    ballStr = 0;
    if (tsspSortedValues[0] > 0) ballStr += 3 * tsspSortedValues[0];
    if (tsspSortedValues[1] > 0) ballStr += 2 * tsspSortedValues[1];
    if (tsspSortedValues[2] > 0) ballStr += tsspSortedValues[2];
    if (tsspSortedValues[3] > 0) ballStr += tsspSortedValues[3];
    ballStr /= 7.0f;

    if (ballStr > 0) {
        ballDir = fmod(atan2f(y, x) * RAD_TO_DEG, 360.0f);
        if (ballDir < 0) ballDir += 360.0f;
    } else {
        ballDir = 0;
    }

    Serial.println(ballDir);
}