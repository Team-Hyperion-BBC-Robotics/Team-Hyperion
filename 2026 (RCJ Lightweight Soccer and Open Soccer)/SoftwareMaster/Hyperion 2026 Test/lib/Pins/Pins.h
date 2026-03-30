#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

#define FRPWM 25
#define FRINA 26
#define FRINB 27
#define FLPWM 11
#define FLINA 12
#define FLINB 24
#define BRPWM 28
#define BRINA 29
#define BRINB 30
#define BLPWM 8
#define BLINA 9
#define BLINB 10
#define DRINA 0
#define DRINB 0
#define DRPWM 0

#define LIGHT_PIN_DIGI_0 0
#define LIGHT_PIN_DIGI_1 0
#define LIGHT_PIN_DIGI_2 0
#define LIGHT_PIN_DIGI_3 0
#define LIGHT_PIN_DIGI_4 0
#define LIGHT_PIN_DIGI_5 0

#define KICKER_PIN 0

#define KICKER_VD_PIN 0 
#define ROBOT_VD_PIN 0

//! @def TSSP1 @brief Tssp Pin
#define TSSP1 16
//! @def TSSP2 @brief Tssp Pin
#define TSSP2 13
//! @def TSSP3 @brief Tssp Pin
#define TSSP3 41
//! @def TSSP4 @brief Tssp Pin
#define TSSP4 40
//! @def TSSP5 @brief Tssp Pin
#define TSSP5 39
//! @def TSSP6 @brief Tssp Pin
#define TSSP6 37
//! @def TSSP7 @brief Tssp Pin
#define TSSP7 36
//! @def TSSP8 @brief Tssp Pin
#define TSSP8 33
//! @def TSSP9 @brief Tssp Pin
#define TSSP9 31
//! @def TSSP10 @brief Tssp Pin
#define TSSP10 7
//! @def TSSP11 @brief Tssp Pin
#define TSSP11 6
//! @def TSSP12 @brief Tssp Pin
#define TSSP12 5
//! @def TSSP13 @brief Tssp Pin
#define TSSP13 4
//! @def TSSP14 @brief Tssp Pin
#define TSSP14 3
//! @def TSSP15 @brief Tssp Pin
#define TSSP15 2
//! @def TSSP16 @brief Tssp Pin
#define TSSP16 17

/*
    SWITCHES:
    - Power (Hardware Switch) - Powers motors and comm module.
    - Logic (Hardware Switch) - Powers all logic based systems.
    - Calibration (Software Switch) - Allows for calibration of IMU and LS.
    - Motor (Software Switch) - No motors are run unless this switch is turned on.
    - Dribbler (Software Switch) - Dribbler is not to run unless this switch is turned on.
*/
#define CALIBRATION_SWITCH 0
#define MOTOR_SWITCH 0

#endif