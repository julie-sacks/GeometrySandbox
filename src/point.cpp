#include "point.h"
#include "collisions.h"
#include <glm/gtx/transform.hpp>

Point::Point(glm::vec3 pos) : GenericPoint(ShapeType::Point), position(pos)
{
}

void Point::SetPos(glm::vec3 pos)
{
    position = pos;
}

glm::vec3 Point::GetPos() const
{
    return position;
}

glm::mat4 Point::getModelToWorldMat() const
{
    return glm::translate(position);
}

bool Point::RayIntersects(const Ray& ray, float* t) const
{
    return Intersects(ray, SphereCollider{position, 1}, t);
}
