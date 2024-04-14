#pragma once

#include <glm/vec3.hpp>

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct SphereCollider
{
    glm::vec3 center;
    float radius;
};

// segment with radius
struct CylinderCollider
{
    glm::vec3 p1, p2;
    float radius;
};


bool Intersects(const Ray& ray, const SphereCollider& sphere, float *rt = nullptr);
bool Intersects(const Ray& ray, const CylinderCollider& cylinder, float *rt = nullptr);
