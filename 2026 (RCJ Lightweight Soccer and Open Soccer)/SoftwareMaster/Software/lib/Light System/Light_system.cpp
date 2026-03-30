#include <Light_system.h>

/// @brief A function to initalize the sensors
void LightSystem::init()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        pinMode(inPin[i], OUTPUT);
    }
    pinMode(LIGHT_PIN, INPUT);
    pinMode(LIGHT_PIN2, INPUT);

    for (int i = 0; i < NUM_LS; i++)
    {
        sensorCoords[i][0] = cos(i * 360 / NUM_LS);
        sensorCoords[i][1] = sin(i * 360 / NUM_LS);
    }
}

/// @brief A function that reads one light sensor in the inner-ring
/// @param sNum Sensor Index
/// @return Returns the value of the indecated sensor
int LightSystem::inner_read(int sNum)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        digitalWrite(inPin[i], (sNum >> i) & 1);
    }
    return (sNum >= 16) ? analogRead(LIGHT_PIN) : analogRead(LIGHT_PIN2);
}

void LightSystem::update()
{
    prevLineDirection = lineDirection;
    calculate_line_dir();
    calculate_line_state();
    switch (lineState)
    {
    case (0):
        break;
    case (1):
        break;
    case (2):
        lineDirection = lineDirection;
        break;
    case (3):
        lineDirection = prevLineDirection;
        break;
    default:
        break;
    }
}

/// @brief calutaltes the line direction
void LightSystem::calculate_line_dir()
{
    int clusterData[4][2];
    int clusterAmt = 0;
    int minIndex = 0;
    bool inCluster = false;
    lineDirection = -1;

    for (uint8_t i = 0; i < NUM_LS; i++)
    {
        // If a sensor in inner circle is above white thresh, then it should start an inner cluster.
        if (inner_read(i) > LS_THRESH && inCluster != true)
        {
            inCluster = true;
            minIndex = i;
            if (lineState == 0 && clusterAmt)
            {
                initialClusterPointStart = i;
            }
            else
            {
                initialClusterPointEnd = i;
            }
        }
        // Making the cluster end
        if (inCluster == true && !(inner_read(i) > LS_THRESH))
        {
            if (lineState == 0)
            {
                secondClusterPointStart = i;
            }
            else
            {
                secondClusterPointEnd = i;
            }
            inCluster = false;
            clusterData[clusterAmt][0] = minIndex;
            clusterData[clusterAmt][1] = i - 1;
            clusterAmt++;
        }
    }
    // Edge Case (Last Sensor and First Sensor)
    if (inCluster == true && clusterData[0][0] == 0)
    {
        clusterData[0][0] = minIndex;
    }
    else if (inCluster == true)
    {
        clusterData[clusterAmt][0] = minIndex;
        clusterData[clusterAmt][1] = NUM_LS - 1;
        clusterAmt++;
    }
    // Finding the centre of the clusters
    float clusterMid[clusterAmt];
    for (uint8_t i = 0; i < clusterAmt; i++)
    {
        if (clusterData[i][0] < clusterData[i][1])
        {
            clusterMid[i] = (clusterData[i][0] + clusterData[i][1]) / 2;
            clusterMid[i] *= (360.0f / NUM_LS);
        }
        else
        {
            clusterMid[i] = (clusterData[i][0] + clusterData[i][1]) / 2 + 16;
            clusterMid[i] *= (360.0f / NUM_LS);
        }
    }

    // Cluster Cases
    if (clusterAmt == 1)
    {
        // If there is only one cluster, just assign the line direction as middle of cluster.
        lineDirection = clusterMid[0];
    }
    else if (clusterAmt == 2)
    {
        // If there are two clusters, find the in between of those clusters and make that line dir.
        if (abs(clusterMid[0] - clusterMid[1]) > 180)
        {
            lineDirection = ((clusterMid[0] + 360) + clusterMid[1]) / 2;
        }
        else
        {
            lineDirection = (clusterMid[0] + clusterMid[1]) / 2;
        }
    }
    else if (clusterAmt == 3)
    {
        // If there are 3 clusters, pair them, find the middle of them, then find the middle of the new pairs.
        if (abs(clusterMid[0] - clusterMid[2]) > 180)
        {
            lineDirection = ((clusterMid[0] + 360) + clusterMid[2]) / 2;
        }
        else
        {
            lineDirection = (clusterMid[0] + clusterMid[2]) / 2;
        }
    }
}

/*
ls0 - inside the line
ls1 on line on in side
ls2 on line on outside
ls3 out of line
*/
void LightSystem::calculate_line_state()
{
    bool onLine = (lineDirection != -1);
    float lineDirectionif = fabs(lineDirection - prevLineDirection);
    if (lineState != 3 && lineState != 0)
    {
        lineOffset = lineState + ((((sensorCoords[initialClusterPointStart][0] + sensorCoords[secondClusterPointStart][0]) / 2) * ((sensorCoords[initialClusterPointStart][1] + sensorCoords[secondClusterPointStart][1]) / 2)) - (((sensorCoords[initialClusterPointEnd][0] + sensorCoords[secondClusterPointEnd][0]) / 2) * ((sensorCoords[initialClusterPointEnd][1] + sensorCoords[secondClusterPointEnd][1]) / 2)));
    }
    if (lineState == 0)
    {
        if (onLine)
        {
            lineState = 1;
        }
        else
        {
            lineState = 0;
        }
    }
    if (lineState == 1)
    {
        if (!onLine)
        {
            lineState = 0;
        }
        else if (lineDirectionif >= 140)
        {
            lineState = 2;
        }
        else
        {
            lineState = 1;
        }
    }
    if (lineState == 2)
    {
        if (!onLine)
        {
            lineState = 3;
        }
        else if (lineDirectionif >= 140)
        {
            lineState = 1;
        }
        else
        {
            lineState = 2;
        }
    }
    if (lineState == 3)
    {
        if (onLine)
        {
            lineState = 2;
        }
        else
        {
            lineState = 3;
        }
    }
}