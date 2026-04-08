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

    // Read all sensors
    for (int i = 0; i < 255; i++)
    {
        for (int j = 0; j < TSSP_NUM; j++)
        {
            tsspValues[j] += 1 - digitalRead(tsspPins[j]);
        }
        delayMicroseconds(3);
    }

    // First pass: filter out broken sensors, sort all valid ones
    for (uint8_t i = 0; i < TSSP_NUM; i++)
    {
        // Skip if sensor is broken (reading 255 or 0 consistently)
        if (tsspValues[i] >= 250 || tsspValues[i] <= 5)
            continue;

        // Insert into sorted array
        for (uint8_t j = 0; j < TSSP_NUM; j++)
        {
            if (tsspValues[i] > tsspSortedValues[j])
            {
                ARRAYSHIFTDOWN(tsspSortedValues, j, TSSP_NUM - 1);
                ARRAYSHIFTDOWN(tsspSortedIndex, j, TSSP_NUM - 1);
                tsspSortedValues[j] = tsspValues[i];
                tsspSortedIndex[j] = i;
                break;
            }
        }
    }

    // Calculate direction - adaptive strategy for close vs far range
    float x = 0.0f;
    float y = 0.0f;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (tsspSortedValues[i] > 0)
        {
            x += tsspSortedValues[i] * xVa[tsspSortedIndex[i]];
            y += tsspSortedValues[i] * yVa[tsspSortedIndex[i]];
        }
    }
    x /= 4;
    y /= 4;

    float addedVals = 0;
    for (uint8_t i = 0; i < TSSP_NUM; i++) {
        addedVals += tsspSortedValues[i];   
    }
    ballStr = addedVals/TSSP_NUM;

    if (x != 0 || y != 0)
    {
        ballDir = fmod(atan2f(y, x) * RAD_TO_DEG, 360.0f);
        if (ballDir < 0) ballDir += 360.0f;
    }
    else
    {
        ballDir = 0;
    }
}