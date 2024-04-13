#pragma once

#include "genericShape.h"
#include <glm/glm.hpp>

// all shapes will be based on points.

class Point : public GenericShape
{
friend class ShapeManager;
private:
    glm::vec3 position;

public:
    Point(glm::vec3 pos);
    void SetPos(glm::vec3 pos);
    glm::vec3 GetPos() const;
    glm::mat4 getModelToWorldMat() const override;
    bool RayIntersects(const Ray& ray, float* t) const override;
};
