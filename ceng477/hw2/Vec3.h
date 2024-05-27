#ifndef __VEC3_H__
#define __VEC3_H__
#define NO_COLOR -1

#include "Vec4.h"
#include <ostream>
#include "Color.h"

class Vec3
{
public:
    double x, y, z;
    Color color;

    Vec3();
    Vec3(double x, double y, double z);
    Vec3(double x, double y, double z, Color color);
    Vec3(const Vec3 &other);
    Vec3(const Vec4 &vec4);
    double getNthComponent(int n);

    friend std::ostream &operator<<(std::ostream &os, const Vec3 &v);
};

#endif