#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
    glm::vec4 forward;
    glm::vec4 right;
    glm::vec4 up;
    float fov;
    glm::vec4 position;
    float near_clip_plane;
    float far_clip_plane;
    float aspect_ratio;

    Camera();

    glm::mat4 ViewMatrix();
    glm::mat4 ProjectionMatrix();

    void MoveForward(float d);
    void MoveRight(float d);
    void MoveUpWard(float d);

    void RotateX(float r);
    void RotateY(float r);
    void RotateZ(float r);

};

#endif // CAMERA_H
