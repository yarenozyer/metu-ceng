#include <iomanip>
#include <iostream>
#include "Triangle.h"

Triangle::Triangle()
{
    this->vertexIds[0] = -1;
    this->vertexIds[1] = -1;
    this->vertexIds[2] = -1;
    this->isCulled = false;
}

Triangle::Triangle(int vid1, int vid2, int vid3)
{
    this->vertexIds[0] = vid1;
    this->vertexIds[1] = vid2;
    this->vertexIds[2] = vid3;
    this->isCulled = false;
}

Triangle::Triangle(int vid1, int vid2, int vid3, bool isCulled)
{
    this->vertexIds[0] = vid1;
    this->vertexIds[1] = vid2;
    this->vertexIds[2] = vid3;
    this->isCulled = isCulled;
}

Triangle::Triangle(const Triangle &other)
{
    this->vertexIds[0] = other.vertexIds[0];
    this->vertexIds[1] = other.vertexIds[1];
    this->vertexIds[2] = other.vertexIds[2];
    this->isCulled = other.isCulled;
}

std::ostream &operator<<(std::ostream &os, const Triangle &t)
{
    os << std::fixed << std::setprecision(0) << "Triangle with vertices (" << t.vertexIds[0] << ", " << t.vertexIds[1] << ", " << t.vertexIds[2] << ")";
    return os;
}