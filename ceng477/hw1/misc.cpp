#include "misc.h"
#include <cmath>

parser::Vec3f misc::triangleNorm(misc::Face face){
    parser::Vec3f v1 = face.p2 - face.p1;
    parser::Vec3f v2 = face.p3 - face.p2;

    parser::Vec3f vector = normalizeVector(parser::crossProduct(v1, v2));
    return vector;
}

parser::Vec3f misc::normalizeVector(parser::Vec3f vector)
{
    float length = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

    if(length)
    {
        vector.x /= length;
        vector.y /= length;
        vector.z /= length;
        
    }
    return vector;
}