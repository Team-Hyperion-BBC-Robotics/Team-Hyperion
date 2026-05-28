#ifndef VECT_H
#define VECT_H

class Vect {
public:
    float x, y;

    Vect(float _x = 0, float _y = 0) : x(_x), y(_y) {};

    Vect operator+(const Vect& v) { return Vect(x + v.x, y + v.y); }
    Vect operator-(const Vect& v) { return Vect(x - v.x, y - v.y); }
    Vect operator*(double scalar) { return Vect(x * scalar, y * scalar); }

    float mag() { return sqrt(x*x + y*y); }
    float angle() { return atan2(y, x); }
    
    void unit_vect() {
        float m = mag();
        if (m > 0) { x /= m; y /= m; }
    }
};

#endif
