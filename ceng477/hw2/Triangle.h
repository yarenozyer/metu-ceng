#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

class Triangle
{
public:
    int vertexIds[3];
    bool isCulled;
    Triangle();
    Triangle(int vid1, int vid2, int vid3);
    Triangle(int vid1, int vid2, int vid3, bool isCulled);
    Triangle(const Triangle &other);
    
    friend std::ostream &operator<<(std::ostream &os, const Triangle &t);
};

#endif