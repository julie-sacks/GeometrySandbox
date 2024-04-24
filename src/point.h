#pragma once

#include "genericPoint.h"
#include <glm/glm.hpp>

// all shapes will be based on points.

class Point : public GenericPoint
{
friend class ShapeManager;
private:
    glm::vec3 position;

protected:
    void Recalculate() const override;
public:
    Point(glm::vec3 pos);
    void SetPos(glm::vec3 pos);
    glm::vec3 GetPos() const override;
    bool RayIntersects(const Ray& ray, float* t) const override;
};
