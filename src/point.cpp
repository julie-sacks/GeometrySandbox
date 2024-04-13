#include "point.h"
#include <glm/gtx/transform.hpp>

Point::Point(glm::vec3 pos) : GenericShape(ShapeType::Point, ShapeVisual::Sphere), position(pos)
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
