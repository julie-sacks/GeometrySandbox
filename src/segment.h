#pragma once

#include "genericLine.h"
#include <glm/glm.hpp>

// all shapes will be based on points.

class Segment : public GenericLine
{
friend class ShapeManager;
private:

public:
    Segment(int parent1, int parent2);
    glm::mat4 getModelToWorldMat() const override;
    bool RayIntersects(const Ray& ray, float* t) const override;
    glm::vec3 GetP1Pos() const override;
    glm::vec3 GetP2Pos() const override;
};
