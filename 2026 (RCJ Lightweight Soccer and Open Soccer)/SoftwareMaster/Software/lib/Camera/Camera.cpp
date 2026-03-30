#include "Camera.h"

/**
 * @brief Initialises the camera communication interface.
 *
 * Opens the high-speed serial link used by the vision module (typically an
 * OpenMV or equivalent) at 115200 baud. This must be called once during system
 * startup before any calls to update.
 *
 * The camera is expected to send structured packets defined by:
 *  - CAM_START_PACK_1
 *  - CAM_START_PACK_2
 *  - CAM_PACKET_SIZE
 *
 * @return void
 */
void Camera::init() {
    cameraSerial.begin(115200);
}

/**
 * Processes a full vision packet and updates goal/ball data.
 *
 * Steps:
 * 1. Validate packet (size + start bytes).
 * 2. Extract raw positions:
 *      - Blue goal (goalXB, goalYB)
 *      - Yellow goal (goalXY, goalYY)
 *      - Ball (ballX, ballY)
 * 3. Shift coordinates into robot space.
 * 4. Apply perspective transforms to get angle + distance.
 * 5. Select attack goal based on `attackBlue`.
 * 6. Mark objects visible when their Y ≠ 0.
 *
 * @param attackBlue  True → attack blue goal; false → attack yellow.
 */
void Camera::update(bool attackBlue) {
    if (cameraSerial.available() < CAM_PACKET_SIZE) return;
    int b1 = cameraSerial.read();
    int b2 = cameraSerial.peek();
    if (b1 != CAM_START_PACK_1 || b2 != CAM_START_PACK_2) return;
    cameraSerial.read();
    int goalXY = cameraSerial.read();
    int goalYY = cameraSerial.read();
    int goalXB = cameraSerial.read();
    int goalYB = cameraSerial.read();
    int ballX  = cameraSerial.read();
    int ballY  = cameraSerial.read();
    shift(goalXY, goalYY);
    shift(goalXB, goalYB);
    shift(ballX,  ballY);
    float cmgoalXY = px_to_cm(goalXY);
    float cmgoalYY = px_to_cm(goalYY);
    float cmgoalXB = px_to_cm(goalXB);
    float cmgoalYB = px_to_cm(goalYB);
    float cmballX = px_to_cm(ballX);
    float cmballY = px_to_cm(ballY);
    if (attackBlue) {
        attackGoal._angle  = calc_angle(cmgoalYB, cmgoalXB);
        attackGoal._dist   = calc_distance(cmgoalXB, cmgoalYB);
        attackGoal._visible = (goalYB != 0);

        defendGoal._angle  = calc_angle(cmgoalYY, cmgoalXY);
        defendGoal._dist   = calc_distance(cmgoalXY, cmgoalYY);
        defendGoal._visible = (goalYY != 0);
    }
    else {
        attackGoal._angle  = calc_angle(cmgoalYY, cmgoalXY);
        attackGoal._dist   = calc_distance(cmgoalXY, cmgoalYY);
        attackGoal._visible = (goalYY != 0);

        defendGoal._angle  = calc_angle(cmgoalYB, cmgoalXB);
        defendGoal._dist   = calc_distance(cmgoalXB, cmgoalYB);
        defendGoal._visible = (goalYB != 0);
    }
    if(OPEN) {
        ballInfo._angle = calc_angle(cmballY, cmballX);
        ballInfo._dist  = calc_distance(cmballX, cmballY);

        float relativeDir = (ballInfo._angle > 180) ? (ballInfo._angle - 360) : ballInfo._angle;
        float ratio = constrain(ballInfo._dist / ORBIT_TUNER, 0.0f, 1.0f);
        float strengthFactor = constrain(0.02f * ratio * expf(4.5f * ratio), 0.0f, 1.0f);
        float angleEffect = min(0.4f * expf(0.25f * abs(relativeDir)) - 0.4f, 90.0f);
        float totalOffset = strengthFactor * angleEffect;
        float directionAdjustment = (relativeDir < 0) ? -totalOffset : totalOffset;
        float speedReduction = totalOffset / 90.0f;
        
        ballInfo._moveAngle = fmod(ballInfo._angle + directionAdjustment, 360.0f);
        ballInfo._moveSpeed = BASE_SPEED + (SURGE_SPEED - BASE_SPEED) * (1.0f - speedReduction);
    } else {
        if(defendGoal.visible()) {
            robotData._x = 0 - (defendGoal.dist() * cos(defendGoal.angle() * DEG_TO_RAD));
            robotData._y = -CENTER_TO_GOAL - (defendGoal.dist() * sin(defendGoal.angle() * DEG_TO_RAD));
        } else if(attackGoal.visible()) {
            robotData._x = 0 - (attackGoal.dist() * cos(attackGoal.angle() * DEG_TO_RAD));
            robotData._y = CENTER_TO_GOAL - (attackGoal.dist() * sin(attackGoal.angle() * DEG_TO_RAD));
        } else {
            robotData._x = 255;
            robotData._y = 0;
        }
    }
}

/*!
 * @brief Calculates the robot's field position and movement vectors toward a target.
 * * Uses the relative distance and angle of the visible goals to estimate the 
 * robot's global (x, y) coordinates. If a goal is visible, it computes the 
 * magnitude and angle required to reach the specified target coordinates.
 * * @note Assumes the defending goal is at y = -85 and the attacking goal is at y = 85.
 * * @param targX The goal X-coordinate to calculate movement towards.
 * @param targY The goal Y-coordinate to calculate movement towards.
 */
void Camera::localise_to(float targX, float targY) {
    if(defendGoal.visible() || attackGoal.visible()) {
        robotData._moveMag = sqrt(pow((robotData._x - targX), 2) + pow((robotData._y - targY), 2));
        robotData._moveAngle = atan2f(targY - robotData._y, targX - robotData._x) * RAD_TO_DEG;
    } else {
        robotData._moveAngle = 0;
        robotData._moveMag = 0;
    }
}

/*!
 * @brief Translates raw camera pixel coordinates to a robot-centric origin.
 * * Adjusts the incoming pixel values by a predefined offset (CAMERA_PIXEL_SHIFT)
 * to align the "zero" point of the vision data with the center of the robot.
 * * @param x Reference to the raw X pixel coordinate; modified in-place.
 * @param y Reference to the raw Y pixel coordinate; modified in-place.
 */
void Camera::shift(int& x, int& y) {
    if (x != 0) {
        x -= CAMERA_PIXEL_SHIFT;
        y -= CAMERA_PIXEL_SHIFT;
    }
}

/*!
 * @brief Converts a pixel-based measurement into physical centimeters.
 * * Applies a linear regression formula to map the vision sensor's 
 * pixel scale to real-world distance.
 * * @param px The pixel value to convert.
 * @return float The resulting distance in centimeters.
 */
float Camera::px_to_cm(int px) {
    return (0.361429*px)-36.18095;
}

/**
 * @brief Computes distance from coordinates.
 *
 * @param x X-axis displacement (robot-centric).
 * @param y Y-axis displacement (robot-centric).
 *
 * @return float Estimated distance to the target.
 */
float Camera::calc_distance(float x, float y) {
    return sqrtf(pow(x, 2) + pow(y, 2));
}

/**
 * @brief Computes the angle to a target or ball using atan2 and camera orientation.
 *
 * @param opp Opposite side (usually the y value).
 * @param adj Adjacent side (usually the x value).
 *
 * @return float Angle in degrees, robot-centric.
 */
float Camera::calc_angle(float opp, float adj) {
    return fmod(90 - (atan2f(opp, adj) * RAD_TO_DEG), 360);
}