#ifndef __HW1__MISC__
#define __HW1__MISC__

#include <string>
#include <vector>
#include <iostream>
#include "parser.h"


namespace misc{
    struct Face{
        parser::Vec3f p1;
        parser::Vec3f p2;
        parser::Vec3f p3;
    };
    
    enum ObjectType{
        MESH,
        TRIANGLE,
        SPHERE,
        NONE,
    };

    struct HitObjectData{
        ObjectType objectType;
        int objectIndex;
        int faceIndex;
        int materialId;
    };

    struct IntersectingRayData{
        bool intersect;
        float t;
    };

    parser::Vec3f triangleNorm(Face);

    parser::Vec3f normalizeVector(parser::Vec3f vector);

    struct ThreadArgs{
        parser::Camera camera;
        parser::Scene* scene;
        int i_start, i_end, j_start, j_end;
        unsigned char* image;
    };
}

#endif
