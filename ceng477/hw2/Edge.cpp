#include "Edge.h"

Edge::Edge()
{
    this->is_visible = false;
    this->v0 = Vec4(-1, -1, -1, -1);
    this->v1 = Vec4(-1, -1, -1, -1);
    this->faceId = -1;
}

Edge::Edge(Vec4 v0, Vec4 v1)
{
    this->v0 = v0;
    this->v1 = v1;
    this->is_visible = true;
    this->faceId = -1;
}

Edge::Edge(Vec4 v0, Vec4 v1, bool is_visible, int faceId)
{
    this->v0 = v0;
    this->v1 = v1;
    this->is_visible = is_visible;
    this->faceId = faceId;
}

Edge::Edge(const Edge &other)
{
    this->v0 = other.v0;
    this->v1 = other.v1;
    this->is_visible = other.is_visible;
    this->faceId = other.faceId;
}