#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define SECOND_ROBOT 0

#if SECOND_ROBOT
    #define TOF1  0
    #define TOF2  0
    #define TOF3  0
    #define TOF4  0
    #define TOF5  0
    #define TOF6  0
    #define TOF7  0
    #define TOF8  0
    #define TOF9  0
    #define TOF10 0
    #define TOF11 0
    #define TOF12 0
    #define TOF13 0
    #define TOF14 0
    #define TOF15 0
    #define TOF16 0
    #define TOF17 0
    #define TOF18 0
    #define TOF19 0
    #define TOF20 0
    #define TOF21 0
    #define TOF22 0
    #define TOF23 0
    #define TOF24 0
#else
    #define TOF1  0
    #define TOF2  0
    #define TOF3  15
    #define TOF4  15
    #define TOF5  15
    #define TOF6  15
    #define TOF7  0
    #define TOF8  0
    #define TOF9  0
    #define TOF10 0
    #define TOF11 0
    #define TOF12 0
    #define TOF13 0
    #define TOF14 0
    #define TOF15 0
    #define TOF16 0
    #define TOF17 0
    #define TOF18 0
    #define TOF19 0
    #define TOF20 0
    #define TOF21 0
    #define TOF22 0
    #define TOF23 0
    #define TOF24 0
#endif

#define SENSOR_BROKEN_HIGH 255
#define SENSOR_BROKEN_LOW 0 

#endif