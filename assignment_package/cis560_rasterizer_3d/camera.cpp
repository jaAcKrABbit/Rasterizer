#include "camera.h"

Camera::Camera():forward(glm::vec4(0, 0, -1, 0)), right(glm::vec4(1, 0, 0, 0)),
    up(glm::vec4(0, 1, 0, 0)), fov(45), position(glm::vec4(0, 0, 10, 1)),
    near_clip_plane(0.01), far_clip_plane(100), aspect_ratio(1)
{}


glm::mat4 Camera::ViewMatrix() {
    glm::mat4 orientation(glm::vec4(right[0], up[0], forward[0], 0),
                          glm::vec4(right[1], up[1], forward[1], 0),
                          glm::vec4(right[2], up[2], forward[2], 0),
                          glm::vec4(0, 0, 0, 1));

    glm::mat4 translation(glm::vec4(1, 0, 0, 0),
                          glm::vec4(0, 1, 0, 0),
                          glm::vec4(0, 0, 1, 0),
                          glm::vec4(-position[0], -position[1], -position[2], 1));
    return orientation * translation;
}
glm::mat4 Camera::ProjectionMatrix() {
    float p = far_clip_plane / (far_clip_plane - near_clip_plane);
    float q = -far_clip_plane * near_clip_plane / (far_clip_plane - near_clip_plane);
    // radian?
    float s = 1 / tan(fov/2);
    float a = aspect_ratio;

    return glm::mat4 (glm::vec4(s/a, 0, 0, 0),
                      glm::vec4(0, s, 0, 0),
                      glm::vec4(0, 0, p, 1),
                      glm::vec4(0, 0, q, 0));
}

void Camera::MoveForward(float d) {
    position += forward * d;
}
void Camera::MoveRight(float d) {
    position += right * d;
}
void Camera::MoveUpWard(float d) {
    position += up * d;
}

void Camera::RotateX(float r) {
    glm::mat4 rot = glm::rotate(glm::mat4(), r, glm::vec3(right));
    up = rot * up;
    forward = rot * forward;
}

void Camera::RotateY(float r) {
    glm::mat4 rot = glm::rotate(glm::mat4(), r, glm::vec3(up));
    right = rot * right;
    forward = rot * forward;
}
void Camera::RotateZ(float r) {
    glm::mat4 rot = glm::rotate(glm::mat4(), r, glm::vec3(forward));
    right = rot * right;
    up = rot * up;
}
