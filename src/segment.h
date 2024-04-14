#pragma once

#include "genericShape.h"
#include <glm/glm.hpp>

// all shapes will be based on points.

class Segment : public GenericShape
{
friend class ShapeManager;
private:

public:
    Segment(int parent1, int parent2);
    glm::mat4 getModelToWorldMat() const override;
    bool RayIntersects(const Ray& ray, float* t) const override;
};
