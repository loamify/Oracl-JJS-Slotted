#pragma once
#include <cmath>

class f_vector2d
{
public:
    f_vector2d() : x(0.f), y(0.f) {}
    f_vector2d(double _x, double _y) : x(_x), y(_y) {}
    ~f_vector2d() {}
    double x, y;
};

class f_vector
{
public:
    f_vector() : x(0.f), y(0.f), z(0.f) {}
    f_vector(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
    ~f_vector() {}
    double x, y, z;
    inline double dot(f_vector v) { return x * v.x + y * v.y + z * v.z; }
    inline double distance(f_vector v) { return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0))); }
    f_vector operator-(f_vector v) { return f_vector(x - v.x, y - v.y, z - v.z); }
}; 