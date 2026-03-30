#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include <math.h>
#include <Configuration.h>
#include <PID.h>


/**
 * @class TargetData
 * @brief Holds processed detection information for a goal target (attack or defend).
 *
 * This class is a lightweight read-only container for structured vision results.
 * It is updated internally by the @ref Camera class and provides:
 *  - Horizontal angle to the target (degrees)
 *  - Estimated distance to the target (in field units)
 *  - Visibility state (true = target seen, false = not detected)
 *
 * All member variables are private and only accessible to @ref Camera as a friend class.
 */
class TargetData {
public:
    float angle()   const { return _angle; }
    float dist()    const { return _dist; }
    bool  visible() const { return _visible; }
private:
    friend class Camera;
    float _angle   = 0;
    float _dist    = 0;
    bool  _visible = false;
};

/**
 * @class BallData
 * @brief Container for processed ball detection information.
 *
 * The ball data consists of:
 *  - Angular offset relative to robot front
 *  - Estimated distance to the ball
 *
 * This class is intentionally simpler than TargetData because the ball
 * does not track a visibility flag — the Camera ensures consistency.
 */
class BallData {
public:
    float angle()       const { return _angle; }
    float dist()        const { return _dist; }
    float move_angle()  const { return _moveAngle; }
    float move_speed()  const { return _moveSpeed; }
private:
    friend class Camera;
    float _angle = 0;
    float _dist  = 0;
    float _moveAngle = 0;
    float _moveSpeed = 0;
};

/**
 * @class RobotData
 * @brief Holds the robot's position on the field and calculated movement vectors.
 */
class RobotData {
public:
    float x()           const { return _x; }
    float y()           const { return _y; }
    float move_mag()    const { return _moveMag; }
    float move_angle()  const { return _moveAngle; }
private:
    friend class Camera;
    float _x = 0;
    float _y = 0;
    float _moveMag = 0;
    float _moveAngle = 0;
};

/**
 * @class Camera
 * @brief Handles all vision processing and transforms raw detection data into structured outputs.
 *
 * The Camera class manages:
 *  - Goal detection (attack & defend targets)
 *  - Ball detection
 *  - Angle and distance computation from pixel coordinates
 *  - Coordinate shifting into the robot's reference frame
 *
 * The Camera is designed to interface with a vision module (OpenMV or similar)
 * using values defined in @ref Configuration.h.
 */
class Camera {
public:
    Camera() {}
    void init();
    void update(bool attackBlue);
    void localise_to(float targX, float targY);
    const TargetData&   attack()        const { return attackGoal; }
    const TargetData&   defend()        const { return defendGoal; }
    const BallData&     ball()          const { return ballInfo;   }
    const RobotData&   robot()          const { return robotData; }
private:
    float calc_distance(float x, float y);
    float calc_angle(float opp, float adj);
    float px_to_cm(int px);
    void shift(int& x, int& y);
    TargetData      attackGoal;
    TargetData      defendGoal;
    BallData        ballInfo;
    RobotData       robotData;
};

#endif