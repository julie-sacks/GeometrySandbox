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


bool Intersects(const Ray& ray, const SphereCollider& sphere, float *rt = nullptr);
