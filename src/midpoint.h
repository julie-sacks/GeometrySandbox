#pragma once

#include "genericPoint.h"
#include <glm/glm.hpp>

// all shapes will be based on points.

class Midpoint : public GenericPoint
{
friend class ShapeManager;
private:
    float t;

protected:
    void Recalculate() const override;
public:
    Midpoint(int parent, float t);
    void SetT(float newt);
    float GetT() const;
    glm::vec3 GetPos() const override;
    bool RayIntersects(const CollisionRay& ray, float* t) const override;
};
