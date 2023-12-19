
#pragma once

#include "glm/vec2.hpp"

struct Circle {
    glm::vec2 center;
    float radius;
};

struct Triangle {
    glm::vec2 p1, p2, p3;
};

class MeshCollision {
public:
    static bool CircleTriangleCollision(const Circle& circle, const Triangle& tri, glm::vec2& normal);
};