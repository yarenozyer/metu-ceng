#include <iomanip>
#include <iostream>
#include "Vec3.h"

Vec3::Vec3()
{
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
    this->color = Color(0, 0, 0);
}

Vec3::Vec3(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->color = Color(0, 0, 0);
}

Vec3::Vec3(double x, double y, double z, Color color)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->color = color;
}

Vec3::Vec3(const Vec3 &other)
{
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->color = other.color;
}

Vec3::Vec3(const Vec4 &vec4)
{
    this->x = vec4.x;
    this->y = vec4.y;
    this->z = vec4.z;
    this->color = vec4.color;
}

double Vec3::getNthComponent(int n)
{
    switch (n)
    {
    case 0:
        return this->x;

    case 1:
        return this->y;

    case 2:
    default:
        return this->z;
    }
}

std::ostream &operator<<(std::ostream &os, const Vec3 &v)
{
    os << std::fixed << std::setprecision(6) << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    return os;
}