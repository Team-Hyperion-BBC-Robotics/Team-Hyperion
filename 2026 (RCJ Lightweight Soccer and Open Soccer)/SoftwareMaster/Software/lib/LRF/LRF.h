#ifndef LRF_MASTER_H
#define LRF_MASTER_H

#include <Arduino.h>
#include <Configuration.h>

class LRFProcessor {
public:
    LRFProcessor();
    
    void update(Stream &serialPort, float imuHeading);
    void localise_to(int x, int y);
    void calculate_wall_score(float virtualX, float virtualY);
    
    float get_self_x() { return selfX; }
    float get_self_y() { return selfY; }
    float get_loc_mag() { return localiseMag; }
    float get_loc_dir() { return localiseDir; }
    float get_wall_dir() { return wallDir; }
    float get_wall_mag() { return wallMag; }
    float get_robot_x(int i) { return robotCoords[i][0]; }
    float get_robot_y(int i) { return robotCoords[i][1]; }
    float get_closest_obstacle() { return closestObsAng; }

private:
    void calculate_position(float imuHeading);
    void calculate_closest_obstacle();

    float localiseMag = 0;
    float localiseDir = 0;
    float wallMag = 0;
    float wallDir = 0;
    float selfX = 0;
    float selfY = 0;
    float closestObsAng = 0;
    float robotCoords[8][2] = {0};
    uint16_t distances[8];
    
    String buffer = ""; 
};

#endif