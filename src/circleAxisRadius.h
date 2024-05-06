#pragma once
#include "genericCircle.h"

class CircleAxisRadius : public GenericCircle
{
private:
    mutable glm::vec3 center;
    mutable glm::vec3 outer;
    mutable float radius;
protected:
    void Recalculate() const override;
public:
    CircleAxisRadius(int center, int outer);
    glm::vec3 GetCenter() const override;
    glm::vec3 GetOuter() const override;
    bool RayIntersects(const CollisionRay& ray, float* t) const override;
};
