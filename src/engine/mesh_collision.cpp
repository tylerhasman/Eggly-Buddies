
#include "mesh_collision.h"
#include "glm/vec2.hpp"
#include "glm/common.hpp"
#include "glm/geometric.hpp"

bool MeshCollision::CircleTriangleCollision(const Circle& circle, const Triangle& tri, glm::vec2& normal) {
    // Vectors that point from circle center to each vertex of the triangle
    glm::vec2 v1 = tri.p1 - circle.center;
    glm::vec2 v2 = tri.p2 - circle.center;
    glm::vec2 v3 = tri.p3 - circle.center;

    // Edges of the triangle
    glm::vec2 e1 = tri.p2 - tri.p1;
    glm::vec2 e2 = tri.p3 - tri.p2;
    glm::vec2 e3 = tri.p1 - tri.p3;

    // Closest point on each edge from circle center
    glm::vec2 closest1 = tri.p1 + glm::clamp(glm::dot(v1, e1) / glm::dot(e1, e1), 0.0f, 1.0f) * e1;
    glm::vec2 closest2 = tri.p2 + glm::clamp(glm::dot(v2, e2) / glm::dot(e2, e2), 0.0f, 1.0f) * e2;
    glm::vec2 closest3 = tri.p3 + glm::clamp(glm::dot(v3, e3) / glm::dot(e3, e3), 0.0f, 1.0f) * e3;

    // Check each closest point for collision with circle
    glm::vec2 d1 = closest1 - circle.center;
    glm::vec2 d2 = closest2 - circle.center;
    glm::vec2 d3 = closest3 - circle.center;
    
    if (glm::length(d1) < circle.radius) {
        normal = glm::normalize(d1);
        return true;
    }
    if (glm::length(d2) < circle.radius) {
        normal = glm::normalize(d2);
        return true;
    }
    if (glm::length(d3) < circle.radius) {
        normal = glm::normalize(d3);
        return true;
    }

    return false;
}