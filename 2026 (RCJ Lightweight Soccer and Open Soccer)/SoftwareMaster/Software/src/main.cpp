#include <Arduino.h>
#include <Drive_system.h>
#include <Debug.h>
#include <TSSP_system.h>
#include <Light_system.h>
#include <Adafruit_BNO055.h>
#include <Camera.h>
#include <Bluetooth.h>
#include <Kicker.h>
#include <Dribbler.h>
#include <Common.h>

enum RobotState {
    STATE_IDLE,
    STATE_CALIBRATE,
    STATE_TEST,
    STATE_ATTACK,
    STATE_DEFEND,
    STATE_LINE_AVOID
};

enum DefendSubState {
    LWDEFEND_VECTOR,
    LWDEFEND_SURGE,
    LWDEFEND_LOCALISE
};

VoltageDivider battery(ROBOT_VD_PIN, ROBOT_VOLTAGE_STABALISER);
DriveSystem motors;
Adafruit_BNO055 bno(55, BNO055_ADDRESS_B, &Wire);
TsspSystem tssp;
LightSystem ls;
Camera cam;
Bluetooth bt;
Kicker kicker;
Dribbler dribbler;
Common com;

Timer batteryTimer(5000000);
Timer wallKickTimer(1500000);

PID correction(KP_IMU, 0.0, KD_IMU, 100.0);
PID goalTrack(KP_GOALT, 0.0, KD_GOALT, 100.0);
PID hozt(KP_HOZT, 0.0, KD_HOZT);
PID vert(KP_VERT, 0.0, KD_VERT);
PID localise(KP_LOC, KD_LOC, 0.0, 200.0);
PID lineAvoid(KP_LAV, 0.0, KD_LAV, 100.0);

RobotState currentBehaviour = STATE_IDLE;
DefendSubState defendMode = LWDEFEND_VECTOR;
sensors_event_t bearing;

void setup() {
    Serial.begin(9600);
    motors.init();
    tssp.init();
    ls.init();
    cam.init();
    kicker.init();
    
    pinMode(CALIBRATION_SWITCH, INPUT);
    pinMode(GOAL_TRACK_SWITCH, INPUT);

    while (!bno.begin(OPERATION_MODE_IMUPLUS)) {
        Serial.println("No BNO055 detected.");
        delay(1000);
    }
    bno.setExtCrystalUse(true);
    currentBehaviour = STATE_IDLE;
}

void loop() {
    bno.getEvent(&bearing);
    tssp.update();
    cam.update(digitalRead(GOAL_TRACK_SWITCH));
    bt.update(tssp.ball().dir(), tssp.ball().str(), cam.attack().angle(), cam.defend().dist(), 0.0f, false, cam.robot().x(), cam.robot().y());
    ls.update();

    if (DRIBBLER_ENABLED && (tssp.ball().str() > BALL_CLOSE_STR)) {
        dribbler.run(100);
    }

    float _dir = 0;
    float _spd = 0;
    float _cor = -correction.update(com.normaliseAngle180(bearing.orientation.x), 0.0);
    
    if (TEST_MODE) {
        currentBehaviour = STATE_TEST;
    } else if (digitalRead(CALIBRATION_SWITCH)) {
        currentBehaviour = STATE_CALIBRATE;
    } else if (ls.get_line_dir() != -1) {
        currentBehaviour = STATE_LINE_AVOID;
    } else if (bt.get_role()) {
        currentBehaviour = STATE_ATTACK;
    } else {
        currentBehaviour = STATE_DEFEND;
    }

    switch (currentBehaviour) {
        case STATE_TEST:
            _spd = 0;
            break;

        case STATE_CALIBRATE:
            bno.setMode(OPERATION_MODE_CONFIG);
            delay(20);
            bno.setMode(OPERATION_MODE_IMUPLUS);
            break;

        case STATE_LINE_AVOID:
            _spd = -lineAvoid.update(ls.get_line_state(), 0.0);
            _dir = fmod(ls.get_line_dir() + 180.0, 360);
            break;

        case STATE_ATTACK:
            if(tssp.ball().str() != 0) {
                _dir = tssp.move().dir();
                _spd = tssp.move().spd();
                if (GOAL_TRACKING_ENABLED) {
                    _cor = -goalTrack.update(com.normaliseAngle180(cam.attack().angle()), 0.0);
                    if ((abs(com.normaliseAngle180(cam.attack().angle())) < 8.0) && tssp.ball().str() > BALL_CLOSE_STR) {
                        kicker.fire();
                    }
                }
            } else {
                cam.localise_to(0, 0);
                _dir = cam.robot().move_angle();
                _spd = -localise.update(cam.robot().move_mag(), 0.0);
            }
            break;

        case STATE_DEFEND:
            if(tssp.ball().str() >= BALL_CLOSE_STR) {
                defendMode = LWDEFEND_SURGE;
            } else if(tssp.ball().str() != 0) {
                defendMode = LWDEFEND_VECTOR;
            } else {
                defendMode = LWDEFEND_LOCALISE;
            }

            switch (defendMode) {
                case(LWDEFEND_VECTOR): {
                    float vertVect;
                    if (ls.get_line_state() == 0) {
                        vertVect = (cam.defend().visible() && cam.defend().dist() <= 40) ? 100 : -100;
                    } else {
                        vertVect = vert.update(ls.get_line_state(), 1.6);
                    }
                    float hoztVectAng = (tssp.ball().str() != 0) ? com.normaliseAngle180(tssp.ball().dir()) : -com.normaliseAngle180(bearing.orientation.x);
                    float hoztVect = -hozt.update(hoztVectAng, 0.0f);
                    _dir = fmod(atan2f(hoztVect, vertVect) * RAD_TO_DEG, 360);
                    _spd = sqrtf(powf(vertVect, 2) + powf(hoztVect, 2));
                    _cor = goalTrack.update(com.normaliseAngle180(fmod(cam.defend().angle() + 180.0f, 360.0f)), 0.0f);
                    break;
                }
                case(LWDEFEND_SURGE):
                    _spd = 100;
                    _dir = tssp.ball().dir();
                    if (GOAL_TRACKING_ENABLED) {
                        _cor = -goalTrack.update(com.normaliseAngle180(cam.attack().angle()), 0.0);
                        if ((abs(com.normaliseAngle180(cam.attack().angle())) < 8.0) && tssp.ball().str() > BALL_CLOSE_STR) {
                            kicker.fire();
                        }
                    }
                    break;
                case(LWDEFEND_LOCALISE):
                    cam.localise_to(40*((cam.robot().x() < 0) ? -1 : 1), 65*((cam.robot().y() < 0) ? -1 : 1));
                    _dir = cam.robot().move_angle();
                    _spd = cam.robot().move_mag();
                    break;
            }
            break;
    }

    if (battery.get_lvl() > ROBOT_REQUIRED_VOLT) {
        batteryTimer.update(); 
        motors.run(_spd, _dir, _cor);
    } else if (batteryTimer.time_has_passed_no_update()) {
        motors.run(0, 0, 20); 
    } else {
        motors.run(_spd, _dir, _cor);
    }
}