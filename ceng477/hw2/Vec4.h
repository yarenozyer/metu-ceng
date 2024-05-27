#ifndef __VEC4_H__
#define __VEC4_H__
#define NO_COLOR -1

#include <ostream>
#include "Color.h"

class Vec4
{
public:
    double x, y, z, t;
    Color color;

    Vec4();
    Vec4(double x, double y, double z, double t);
    Vec4(double x, double y, double z, double t, Color color);
    Vec4(const Vec4 &other);

    double getNthComponent(int n);

    friend std::ostream &operator<<(std::ostream &os, const Vec4 &v);
};

#endif