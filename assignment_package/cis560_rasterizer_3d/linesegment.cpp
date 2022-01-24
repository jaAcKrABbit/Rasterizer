#include "linesegment.h"

LineSegment::LineSegment(glm::vec2& ep1, glm::vec2& ep2):
    slope((ep2[1]-ep1[1])/(ep2[0]-ep1[0])), endpoint_1(ep1),endpoint_2(ep2)
{}

bool LineSegment::getIntersection(int y, float* x) {
    float a = std::max(endpoint_1[1], endpoint_2[1]);
    float b = std::min(endpoint_1[1], endpoint_2[1]);

    if(y <= a && y >= b) {
        //When the line is vertical or horizontal to the pixel row
        if(slope == 0 || endpoint_1[0] == endpoint_2[0]) {
            *x = endpoint_1[0];
        }else {
            *x = (y + slope * endpoint_1[0] - endpoint_1[1]) / slope;
        }
        return true;
    }
    return false;
}
