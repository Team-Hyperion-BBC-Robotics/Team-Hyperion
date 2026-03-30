#include <Arduino.h>
#include <Drive_system.h>
#include <Debug.h>
#include <Light_system.h>
#include <Adafruit_BNO055.h>
#include <Camera.h>
#include <Bluetooth.h>
#include <Kicker.h>
#include <Dribbler.h>
#include <Common.h>
#include <LRF.h> 

enum RobotState {
    STATE_IDLE,
    STATE_CALIBRATE,
    STATE_TEST,
    STATE_ATTACK_OPEN,
    STATE_DEFEND_OPEN,
    STATE_LINE_AVOID
};

enum OpenAttackSubState {
    ATTACK_ORBIT,
    ATTACK_PASS,
    ATTACK_HIDE
};

enum OpenDefendSubState {
    DEFEND_VECTOR,
    DEFEND_SURGING,
    DEFEND_WALL_KICK,
    DEFEND_ORBIT
};

VoltageDivider battery(ROBOT_VD_PIN, ROBOT_VOLTAGE_STABALISER);
DriveSystem motors;
LRFProcessor lrf;
Adafruit_BNO055 bno(55, BNO055_ADDRESS_B, &Wire);
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
PID wallAngle(KP_IMU, 0.0, KD_IMU, 100.0);
PID lineAvoid(KP_LAV, 0.0, KD_LAV, 100.0);

RobotState currentBehaviour = STATE_IDLE;
OpenAttackSubState openAttackMode = ATTACK_ORBIT;
OpenDefendSubState openDefendMode = DEFEND_VECTOR;
sensors_event_t bearing;
bool isSurging = false;

void setup() {
    Serial.begin(9600);
    Serial1.begin(115200);
    motors.init();
    ls.init();
    cam.init();
    kicker.init();
    
    pinMode(CALIBRATION_SWITCH, INPUT);
    pinMode(GOAL_TRACK_SWITCH, INPUT);

    while (!bno.begin(OPERATION_MODE_IMUPLUS)) {
        delay(1000);
    }
    bno.setExtCrystalUse(true);
    currentBehaviour = STATE_IDLE;
}

void loop() {
    bno.getEvent(&bearing);
    lrf.update(Serial1, bearing.orientation.x);
    cam.update(digitalRead(GOAL_TRACK_SWITCH));
    bt.update(cam.ball().angle(), cam.ball().dist(), cam.attack().angle(), cam.defend().dist(), 0.0f, false, lrf.get_self_x(), lrf.get_self_y());
    ls.update();

    if (DRIBBLER_ENABLED && (cam.ball().dist() > BALL_CLOSE_STR)) {
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
        currentBehaviour = STATE_ATTACK_OPEN;
    } else {
        currentBehaviour = STATE_DEFEND_OPEN;
    }

    switch (currentBehaviour) {
        case STATE_LINE_AVOID:
            _spd = -lineAvoid.update(ls.get_line_state(), 0.0);
            _dir = fmod(ls.get_line_dir() + 180.0, 360);
            break;

        case STATE_ATTACK_OPEN:
            if(lrf.get_self_y() < 0 && cam.ball().dist() > BALL_CLOSE_STR) openAttackMode = ATTACK_HIDE;
            if(openAttackMode == ATTACK_HIDE && lrf.get_self_y() >= 900) {
                if(abs(com.normaliseAngle180(cam.attack().angle())) < 10.0) {
                    kicker.fire();
                    openAttackMode = ATTACK_ORBIT;
                }
            }

            if (cam.ball().dist() != 0 && GOAL_TRACKING_ENABLED) {
                if(openAttackMode == ATTACK_HIDE) {
                    _dir = fmod(360.0f - bearing.orientation.x, 360.0f);
                    _spd = 90;
                    float targetAngle = (lrf.get_self_y() >= 900) ? cam.attack().angle() : com.normaliseAngle180(cam.attack().angle() + 180.0f);
                    _cor = -goalTrack.update(com.normaliseAngle180(targetAngle), 0.0);
                } else if (openAttackMode == ATTACK_PASS) {
                    lrf.calculate_wall_score(bt.get_other_x(), bt.get_other_y());
                    if (abs(com.normaliseAngle180(bearing.orientation.x - lrf.get_wall_dir())) < 5.0 && cam.ball().dist() > BALL_CLOSE_STR) {
                        kicker.fire();
                        openAttackMode = ATTACK_ORBIT; 
                    }
                    _cor = wallAngle.update(com.normaliseAngle180(lrf.get_wall_dir() - bearing.orientation.x), 0.0);
                    _spd = BASE_SPEED;
                } else {
                    _dir = cam.ball().move_angle();
                    _spd = cam.ball().move_speed();
                    _cor = -goalTrack.update(com.normaliseAngle180(cam.attack().angle()), 0.0);
                    if (cam.ball().dist() > BALL_CLOSE_STR && abs(com.normaliseAngle180(lrf.get_closest_obstacle())) < 20) {
                        openAttackMode = ATTACK_PASS;
                        wallKickTimer.update();
                    }
                }
                if (openAttackMode != ATTACK_PASS && (abs(com.normaliseAngle180(cam.attack().angle())) < 8.0) && cam.ball().dist() > BALL_CLOSE_STR) {
                    kicker.fire();
                }
            } else {
                lrf.localise_to(0, 0);
                _dir = lrf.get_loc_dir();
                _spd = lrf.get_loc_mag();
            }
            break;

        case STATE_DEFEND_OPEN:
            if(wallKickTimer.time_has_passed_no_update()) {
                if (cam.ball().angle() < 310 && cam.ball().angle() > 130) openDefendMode = DEFEND_ORBIT;
                else if (cam.ball().dist() >= BALL_CLOSE_STR || isSurging) {
                    if(abs(com.normaliseAngle180(lrf.get_closest_obstacle())) < 20.0) {
                        openDefendMode = DEFEND_WALL_KICK;
                        wallKickTimer.update();
                    } else {
                        openDefendMode = DEFEND_SURGING;
                        isSurging = true;
                    }
                } else openDefendMode = DEFEND_VECTOR;
            }

            switch (openDefendMode) {
                case DEFEND_ORBIT:
                    _dir = cam.ball().move_angle(); _spd = cam.ball().move_speed();
                    break;
                case DEFEND_SURGING:
                    if (cam.ball().dist() < BALL_CLOSE_STR) { isSurging = false; openDefendMode = DEFEND_VECTOR; }
                    else { _dir = cam.ball().angle(); _spd = SURGE_SPEED; }
                    break;
                case DEFEND_WALL_KICK:
                    lrf.calculate_wall_score(0, 1095);
                    if (abs(com.normaliseAngle180(bearing.orientation.x - lrf.get_wall_dir())) < 5.0 && cam.ball().dist() > BALL_CLOSE_STR) {
                        kicker.fire(); openDefendMode = DEFEND_VECTOR; 
                    }
                    _cor = wallAngle.update(com.normaliseAngle180(lrf.get_wall_dir() - bearing.orientation.x), 0.0);
                    _spd = BASE_SPEED;
                    break;
                case DEFEND_VECTOR:
                    if (cam.defend().visible()) {
                        float vertVect = vert.update(cam.defend().dist() - SP_DEFEND_VERT, 0);
                        float hoztVectAng = (cam.ball().dist() != 0) ? com.normaliseAngle180(cam.ball().angle()) : -com.normaliseAngle180(bearing.orientation.x);
                        float hoztVect = -hozt.update(hoztVectAng, 0.0f);
                        _dir = fmod(atan2f(hoztVect, vertVect) * RAD_TO_DEG, 360);
                        _spd = sqrtf(powf(vertVect, 2) + powf(hoztVect, 2));
                        _cor = goalTrack.update(com.normaliseAngle180(fmod(cam.defend().angle() + 180.0f, 360.0f)), 0.0f);
                    } else {
                        lrf.localise_to(0, 0); _dir = lrf.get_loc_dir(); _spd = lrf.get_loc_mag();
                    }
                    break;
            }
            if (GOAL_TRACKING_ENABLED && abs(com.normaliseAngle180(cam.attack().angle())) < 8.0 && cam.ball().dist() > BALL_CLOSE_STR) kicker.fire();
            break;
    }

    if (battery.get_lvl() > ROBOT_REQUIRED_VOLT) {
        batteryTimer.update(); motors.run(_spd, _dir, _cor);
    } else if (batteryTimer.time_has_passed_no_update()) {
        motors.run(0, 0, 20); 
    } else {
        motors.run(_spd, _dir, _cor);
    }
}