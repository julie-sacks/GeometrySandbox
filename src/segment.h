#pragma once

#include "genericLine.h"
#include <glm/glm.hpp>

// all shapes will be based on points.

class Segment : public GenericLine
{
friend class ShapeManager;
private:
    mutable glm::vec3 p1, p2; // these should be copies of the parent points
protected:
    void Recalculate() const override;
public:
    Segment(int parent1, int parent2);
    bool RayIntersects(const Ray& ray, float* t) const override;
    glm::vec3 GetP1Pos() const override;
    glm::vec3 GetP2Pos() const override;
    float ClampToBounds(float param) const override;
};
