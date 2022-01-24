#include "rasterizer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons), camera(Camera())
{}

QImage Rasterizer::RenderScene()
{
    QImage result(512, 512, QImage::Format_RGB32);
    // Fill the image with black pixels.
    // Note that qRgb creates a QColor,
    // and takes in values [0, 255] rather than [0, 1].
    result.fill(qRgb(0.f, 0.f, 0.f));

    // TODO: Complete the various components of code that make up this function
    // It should return the rasterized image of the current scene

    std::array<float, 512* 512> z_Buffer;
    z_Buffer.fill(INT_MAX);

    for(auto& p : m_polygons) {
        // From world space to pixel space
        for(auto& v: p.m_verts) {
            v.m_ppos = camera.ProjectionMatrix() * camera.ViewMatrix() * v.m_pos;
            v.m_ppos =  v.m_ppos / v.m_ppos[3];
            v.m_ppos[0] = (v.m_ppos[0] + 1) / 2 * result.width();
            v.m_ppos[1] = (1 - v.m_ppos[1]) / 2 * result.height();
        }
        for(auto& t : p.m_tris) {
            Vertex vertices[3] = {p.m_verts[t.m_indices[0]],
                                  p.m_verts[t.m_indices[1]],p.m_verts[t.m_indices[2]]};
            //Vertices Positions
            glm::vec4 vp[3] = {vertices[0].m_ppos, vertices[1].m_ppos, vertices[2].m_ppos};
            //uv coords
            glm::vec2 v_uv[3] = {vertices[0].m_uv, vertices[1].m_uv, vertices[2].m_uv};
            //Normal
            glm::vec4 v_normal[3] = {vertices[0].m_normal, vertices[1].m_normal, vertices[2].m_normal};
            //2D representation
            glm::vec2 vp_0 = glm::vec2(vp[0][0],vp[0][1]);
            glm::vec2 vp_1 = glm::vec2(vp[1][0],vp[1][1]);
            glm::vec2 vp_2 = glm::vec2(vp[2][0],vp[2][1]);
            //Put them in an array
            //glm::vec2 v_vec2[3] = {vp_0, vp_1, vp_2};
            LineSegment line_0 = LineSegment(vp_0, vp_1);
            LineSegment line_1 = LineSegment(vp_1, vp_2);
            LineSegment line_2 = LineSegment(vp_2, vp_0);

            LineSegment lines[3] = {line_0, line_1, line_2};
            //Get bounding box values
            std::vector<float> bb = Create2DBoundingBox(p,t);
            // top left corner is origin
            float y_top = ceil(glm::clamp(bb[0], 0.f, 512.f));
            float y_bot = ceil(glm::clamp(bb[1], 0.f, 512.f));
            while(y_top < y_bot) {
                float x_left = 512.f;
                float x_right = 0.f;
                for(auto& line: lines) {
                    float x;
                    if(line.getIntersection(y_top, &x)) {
                        x_left = glm::clamp(std::min(x, x_left),0.f, 512.f);
                        x_right = glm::clamp(std::max(x, x_right),0.f, 512.f);
                    }
                }
                x_left = ceil(x_left);
                x_right = ceil(x_right);
                while( x_left < x_right) {
                    // z value given by json
                    //float z = vp[0][2];
                    glm::vec3 current_pixel = glm::vec3(x_left, y_top, 0);
                    glm::vec3 weights = Barycentric(vp, current_pixel);
                    float z = Z_Interpolation(weights);
                    if(z <  z_Buffer[x_left + 512 * y_top]){                        
                        //Interpolate UV
                        glm::vec2 uv = UV_Interpolation(v_uv, weights, z);
                        glm::vec4 normal = glm::normalize(NormalInterpolation(v_normal, weights, z));
                        glm::vec3 texture = GetImageColor(uv, p.mp_texture);
                        //Lambertian reflection
                        glm::vec4 light = glm::normalize(-camera.forward);
                        float ambient = 0.8f;
                        float reflection = glm::clamp(glm::dot(normal,light), 0.f, 1.f);
                        glm::vec3 p_color =  ambient * texture * reflection;
                        result.setPixel(x_left, y_top, qRgb(p_color[0], p_color[1], p_color[2]));
                        z_Buffer[x_left + 512 * y_top] = z;
                     }
                     x_left ++;
                }
                y_top++;
            }
        }
    }
    return result;
}


//return a vector storing in order: top, bot, left, right
std::vector<float> Create2DBoundingBox(Polygon& poly, Triangle& tri) {
   std::vector<float> res;
   //Triangle verts indices
   unsigned int ti_0 = tri.m_indices[0];
   unsigned int ti_1 = tri.m_indices[1];
   unsigned int ti_2 = tri.m_indices[2];
   //Triangle vertices position
   float t0_x = poly.m_verts[ti_0].m_ppos[0];
   float t0_y = poly.m_verts[ti_0].m_ppos[1];
   float t1_x = poly.m_verts[ti_1].m_ppos[0];
   float t1_y = poly.m_verts[ti_1].m_ppos[1];
   float t2_x = poly.m_verts[ti_2].m_ppos[0];
   float t2_y = poly.m_verts[ti_2].m_ppos[1];
   //Getting bounding box value
   float top = std::min({t0_y, t1_y, t2_y});
   float bot = std::max({t0_y, t1_y, t2_y});
   float left = std::min({t0_x, t1_x, t2_x});
   float right = std::max({t0_x, t1_x, t2_x});

   res.push_back(top);
   res.push_back(bot);
   res.push_back(left);
   res.push_back(right);

   return res;
}

float area(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3) {
    return 0.5 * glm::length(glm::cross(p1-p2, p3-p2));
}

glm::vec3 Barycentric(const glm::vec4* verts, glm::vec3& p) {
    //Convert coordinates again from 2d to 3d with an 0 z vlue
    glm::vec3 p1 = glm::vec3(verts[0][0], verts[0][1], 0);
    glm::vec3 p2 = glm::vec3(verts[1][0], verts[1][1], 0);
    glm::vec3 p3 = glm::vec3(verts[2][0], verts[2][1], 0);
    //3D
    float z1 = verts[0][2];
    float z2 = verts[1][2];
    float z3 = verts[2][2];

    float s = area(p1, p2, p3);
    float s1 = area(p, p2, p3);
    float s2 = area(p, p3, p1);
    float s3 = area(p, p1, p2);

    return glm::vec3(s1 / (s * z1), s2 / (s* z2), s3 / (s* z3));
}

float Z_Interpolation(const glm::vec3 &weights) {
    return 1 /  (weights[0] + weights[1] + weights[2]);
}

glm::vec2 UV_Interpolation(const glm::vec2* uv, const glm::vec3& weights, float z) {
    return z * (uv[0] * weights[0] + uv[1] * weights[1] + uv[2] * weights[2]);
}

glm::vec4 NormalInterpolation(const glm::vec4* normal, const glm::vec3& weights, float z) {
    return  z * (normal[0] * weights[0] + normal[1] * weights[1] + normal[2] * weights[2]);
}

void Rasterizer::ClearScene()
{
    m_polygons.clear();
}
