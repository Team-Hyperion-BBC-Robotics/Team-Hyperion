#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

#define FRINA 0
#define FRINB 0
#define FRPWM 0
#define BRINA 0
#define BRINB 0
#define BRPWM 0
#define BLINA 0
#define BLINB 0
#define BLPWM 0
#define FLINA 0
#define FLINB 0
#define FLPWM 0
#define DRINA 0
#define DRINB 0
#define DRPWM 0

#define LIGHT_PIN_DIGI_0 23
#define LIGHT_PIN_DIGI_1 22
#define LIGHT_PIN_DIGI_2 15
#define LIGHT_PIN_DIGI_3 14
#define LIGHT_PIN 21
#define LIGHT_PIN2 20

#define KICKER_PIN 0

#define KICKER_VD_PIN 0 
#define ROBOT_VD_PIN 0

/*
    SWITCHES:
    - Power (Hardware Switch) - Powers all logic + comm module.
    - Motor (Hardware Switch) - Powers to motor.
    - Kicker (Hardware Switch) - Stops power going to the Kicker Cap.
    - Calibration (Software Switch) - Allows for calibration of IMU and LS.
    - Goal Tracking (Software Switch) - Tells the code which goal to attack.
*/
#define CALIBRATION_SWITCH 0
#define GOAL_TRACK_SWITCH 0

#endif