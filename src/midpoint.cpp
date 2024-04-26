#include "midpoint.h"
#include "shapeManager.h"
#include "segment.h"
#include "collisions.h"
#include <glm/gtx/transform.hpp>

Midpoint::Midpoint(int parent, float t) : GenericPoint(ShapeType::Midpoint), t(t)
{
    parents.push_back(parent);
}

void Midpoint::SetT(float newt)
{
    SetDirty();
    t = dynamic_cast<GenericLine*>(manager->GetShape(parents[0]))->ClampToBounds(newt);
}

float Midpoint::GetT() const
{
    return t;
}

glm::vec3 Midpoint::GetPos() const
{
    glm::vec3 p1, p2;
    // future-proofing for different types of midpoint parents
    GenericShape* parent = manager->GetShape(parents[0]);
    if(parent->IsLineLike())
    {
        p1 = dynamic_cast<GenericLine*>(parent)->GetP1Pos();
        p2 = dynamic_cast<GenericLine*>(parent)->GetP2Pos();
    }
    else assert(false);
    return glm::mix(p1, p2, t);
}

void Midpoint::Recalculate() const
{
    isDirty = false;
    modelToWorld = glm::translate(GetPos());
}

bool Midpoint::RayIntersects(const CollisionRay& ray, float* t) const
{
    return Intersects(ray, SphereCollider{GetPos(), 1}, t);
}
