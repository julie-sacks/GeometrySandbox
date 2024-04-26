#include "point.h"
#include "collisions.h"
#include <glm/gtx/transform.hpp>

void Point::Recalculate() const
{
    isDirty = false;
    modelToWorld = glm::translate(position);
}

Point::Point(glm::vec3 pos) : GenericPoint(ShapeType::Point), position(pos)
{
}

void Point::SetPos(glm::vec3 pos)
{
    SetDirty();
    position = pos;
}

glm::vec3 Point::GetPos() const
{
    return position;
}

bool Point::RayIntersects(const CollisionRay& ray, float* t) const
{
    return Intersects(ray, SphereCollider{position, 1}, t);
}
