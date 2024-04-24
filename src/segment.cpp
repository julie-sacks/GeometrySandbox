#include "segment.h"
#include <glm/gtx/transform.hpp>
#include "shapeManager.h"
#include "genericPoint.h"
#include "collisions.h"

Segment::Segment(int parent1, int parent2) : GenericLine(ShapeType::Segment)
{
    parents.push_back(parent1);
    parents.push_back(parent2);
}

void Segment::Recalculate() const
{
    isDirty = false;
    glm::vec3 p1 = GetP1Pos();
    glm::vec3 p2 = GetP2Pos();

    // scale by 0.5x -> rotate (0,1,0) to p2 -> translate to p1
    glm::mat4 scale = glm::scale(glm::vec3(0.5f, glm::length(GetP2Pos()-GetP1Pos()), 0.5f));

    glm::vec3 dir = glm::normalize(p2-p1);
    glm::vec3 crossp = glm::cross(glm::vec3(0,1,0), glm::normalize(GetP2Pos()-GetP1Pos()));
    glm::mat4 rotate = glm::rotate(acosf(glm::dot(glm::vec3(0,1,0), dir)), crossp);

    glm::mat4 translate = glm::translate(p1);

    modelToWorld = translate*rotate*scale;
}

bool Segment::RayIntersects(const Ray& ray, float* t) const
{
    glm::vec3 p1 = GetP1Pos();
    glm::vec3 p2 = GetP2Pos();
    return Intersects(ray, CylinderCollider{p1, p2, 0.5f}, t);
}

glm::vec3 Segment::GetP1Pos() const
{
    return dynamic_cast<GenericPoint*>(manager->GetShape(parents[0]))->GetPos();
}
glm::vec3 Segment::GetP2Pos() const
{
    return dynamic_cast<GenericPoint*>(manager->GetShape(parents[1]))->GetPos();
}

float Segment::ClampToBounds(float param) const
{
    return glm::max(0.0f, glm::min(1.0f, param));
}
