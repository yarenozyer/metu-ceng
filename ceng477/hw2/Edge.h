
#ifndef __EDGE_H__
#define __EDGE_H__
#include "Vec3.h"
#include "Vec4.h" 
#include <iomanip>
#include <iostream>

class Edge
{
public:
    Vec4 v0;
    Vec4 v1;
    bool is_visible;
    int faceId;

    Edge();
    Edge(Vec4 v0, Vec4 v1); 
    Edge(Vec4 v0, Vec4 v1, bool is_visible, int faceId);    
    Edge(const Edge &other);
};



#endif