#pragma once
#include <polygon.h>
#include <linesegment.h>
#include <iostream>
#include <QImage>
#include <camera.h>

class Rasterizer
{
private:
    //This is the set of Polygons loaded from a JSON scene file
    std::vector<Polygon> m_polygons;
public:
    Camera camera;
    Rasterizer(const std::vector<Polygon>& polygons);
    QImage RenderScene();
    void ClearScene();
};

std::vector<float> Create2DBoundingBox(Polygon& poly, Triangle& tri);
float area(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3);
glm::vec3 Barycentric(const glm::vec4* verts, glm::vec3& p);
float Z_Interpolation(const glm::vec3& weights);
glm::vec2 UV_Interpolation(const glm::vec2* uv, const glm::vec3& weights, float z);
glm::vec4 NormalInterpolation(const glm::vec4* normal, const glm::vec3& weights, float z);
