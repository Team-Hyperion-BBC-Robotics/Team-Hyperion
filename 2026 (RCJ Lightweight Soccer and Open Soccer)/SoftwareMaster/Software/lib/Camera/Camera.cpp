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
void Camera::update(bool attackBlue, float bearing) {
    if (cameraSerial.available() < CAM_PACKET_SIZE) return;
    int b1 = cameraSerial.read();
    int b2 = cameraSerial.peek();
    if (b1 != CAM_START_PACK_1 || b2 != CAM_START_PACK_2) return;
    cameraSerial.read();
    int goalXY = cameraSerial.read();
    int goalYY = cameraSerial.read();
    int goalSzY = cameraSerial.read();
    int goalXB = cameraSerial.read();
    int goalYB = cameraSerial.read();
    int goalSzB = cameraSerial.read();
    int ballX  = cameraSerial.read();
    int ballY  = cameraSerial.read();

    bool yellowVisible = (goalYY != 255 && goalXY != 255);
    bool blueVisible   = (goalYB != 255 && goalXB != 255);
    
    shift(goalXY, goalYY);
    shift(goalXB, goalYB);
    shift(ballX,  ballY);
    if (attackBlue) {
        attackGoal._angle  = calc_angle(goalYB, goalXB);
        attackGoal._dist   = calc_distance(goalXB, goalYB);
        attackGoal._size   = px_to_mm(goalSzB);
        attackGoal._visible = blueVisible;

        defendGoal._angle  = calc_angle(goalYY, goalXY);
        defendGoal._dist   = calc_distance(goalXY, goalYY);
        defendGoal._size   = px_to_mm(goalSzY);
        defendGoal._visible = yellowVisible;
    }
    else {
        attackGoal._angle  = calc_angle(goalYY, goalXY);
        attackGoal._dist   = calc_distance(goalXY, goalYY);
        attackGoal._size   = px_to_mm(goalSzY);
        attackGoal._visible = yellowVisible;

        defendGoal._angle  = calc_angle(goalYB, goalXB);
        defendGoal._dist   = calc_distance(goalXB, goalYB);
        defendGoal._size   = px_to_mm(goalSzB);
        defendGoal._visible = blueVisible;
    }
    if(OPEN) {
        ballInfo._angle = calc_angle(ballY, ballX);
        ballInfo._dist  = calc_distance(ballX, ballY);

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
        if(attackGoal.visible()) {
            float globalAng = (attackGoal._angle + bearing) * DEG_TO_RAD;
            robotData._x = 0 - (attackGoal._dist * sinf(globalAng));
            robotData._y = CENTER_TO_GOAL - (attackGoal._dist * cosf(globalAng));
            Serial.print(robotData._y);
            Serial.print("\t");
            if(AUSTRALIAN_FIELD) {
                robotData._y += 200.0;
            }
            Serial.println(robotData._y);
            if(robotData._x < 0) {
                robotData._y -= -563.72;
            }
        } else {
            robotData._x = -300.0;
            robotData._y = -300.0;
        }
        // } else if(defendGoal.visible()) {
        //     float globalAng = (defendGoal._angle + bearing) * DEG_TO_RAD;
        //     robotData._x = 0 - (defendGoal._dist * sinf(globalAng));
        //     robotData._y = -CENTER_TO_GOAL - (defendGoal._dist * cosf(globalAng));
        
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
void Camera::localise_to(float targX, float targY, float bearing) {
    if(defendGoal.visible() || attackGoal.visible()) {
        robotData._moveMag = sqrt(pow((robotData._x - targX), 2) + pow((robotData._y - targY), 2));     
        robotData._moveAngle = com.floatMod((atan2f(targX - robotData._x, targY - robotData._y) * RAD_TO_DEG) - bearing + 360.0f, 360.0f);
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
    if (x != 255) {
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
float Camera::px_to_mm(int px) {
    return 0.0000913503*pow(px, 3.51908) + 221.38261;
    // return 6284.13857*pow(px, -0.258574) - 1666.12121;
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
    return px_to_mm(sqrtf(pow(x, 2) + pow(y, 2)));
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