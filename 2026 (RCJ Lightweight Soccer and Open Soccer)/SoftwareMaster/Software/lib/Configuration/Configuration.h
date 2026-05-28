#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>

// [MASTER VALUES]
#define ROBOT 1
#define GOAL_TRACKING_ENABLED 1
#define AUSTRALIAN_FIELD 1
#define TEST_MODE 0
#define DRIBBLER_ENABLED 0
#define OPEN 0
#define ATTACK 1

// [TUNING VALUES]
#if ROBOT
    #define ORBIT_TUNER  8.0 
    #define BASE_SPEED 60
    #define SURGE_SPEED 80
    #define LS_THRESH 0
    #define KP_IMU 1.0
    #define KD_IMU 0.03
    #define KP_GOALT 1.0
    #define KD_GOALT 0.06
    #define KP_VERT 0.0
    #define KD_VERT 0.0
    #define KP_HOZT 0.0
    #define KD_HOZT 0.0
    #define KP_LOC 0.15
    #define KD_LOC 0.0
    #define KP_LAV 0.0
    #define KD_LAV 0.0
    #define SP_DEFEND_VERT 0
    #define BALL_CLOSE_STR 0
    #define DEF_KEEP_SURGE_UNTIL 0
#else
    #define ORBIT_TUNER 1
    #define BASE_SPEED 0
    #define SURGE_SPEED 0
    #define LS_THRESH 0
    #define KP_IMU 0.0
    #define KD_IMU 0.0
    #define KP_GOALT 0.0
    #define KD_GOALT 0.0
    #define KP_VERT 0.0
    #define KD_VERT 0.0
    #define KP_HOZT 0.0
    #define KD_HOZT 0.0
    #define KP_LOC 0.0
    #define KD_LOC 0.0
    #define KP_LAV 0.0
    #define KD_LAV 0.0
    #define SP_DEFEND_VERT 0
    #define BALL_CLOSE_STR 0
    #define DEF_KEEP_SURGE_UNTIL 0
#endif

// [BLUETOOTH SPECIFICATIONS]
#define BT_SERIAL Serial1
#define BT_BAUD 9600
#define BT_PACKET_SIZE 6
#define BT_START_BYTE 255
#define BT_NO_DATA 255
#define BT_PERFORMANCE_PERCENT 10

// [CAMERA SPECIFICATIONS]
#define cameraSerial Serial5
#define CAM_PACKET_SIZE 10
#define CAM_START_PACK_1 200
#define CAM_START_PACK_2 122
#define CAMERA_PIXEL_SHIFT 120

// [KICKER SPECIFICATIONS]
#define KICKER_VOLTAGE_STABALISER 1
#define KICKER_REQUIRED_VOLT 0
#define KICKER_VOLTAGE_OFFSET 1.0
#define ROBOT_VOLTAGE_STABALISER 73.41388
#define ROBOT_VOLTAGE_OFFSET 0.215224
#define ROBOT_REQUIRED_VOLT 11.2

// [FIELD SPECIFICATIONS]
#define CENTER_TO_GOAL 850

#define FIELD_Y_MAX 2430.0
#define FIELD_X_MAX 1820.0
#define ROBOT_TOLERANCE 100.0

#endif