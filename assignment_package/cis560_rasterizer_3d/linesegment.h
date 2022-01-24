#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include "glm/glm.hpp"
#include <iostream>
class LineSegment
{
public:
    float slope;
    glm::vec2 endpoint_1;
    glm::vec2 endpoint_2;

    LineSegment(glm::vec2& ep1,  glm::vec2& ep2);
    bool getIntersection(int y, float* x);

};

#endif // LINESEGMENT_H
