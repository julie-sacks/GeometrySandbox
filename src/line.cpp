#include "line.h"
#include "collisions.h"
#include <glm/gtx/transform.hpp>

Line::Line(int parent1, int parent2) : GenericLine(ShapeType::Line)
{
    parents.push_back(parent1);
    parents.push_back(parent2);
}

void Line::Recalculate() const
{
    isDirty = false;
    glm::vec3 p1 = GetP1Pos();
    glm::vec3 p2 = GetP2Pos();
    // extend the line to the clipping plane/viewport bounds
    glm::vec3 linedir = glm::normalize(p2-p1);
    p1 -= linedir*200.0f;
    p2 += linedir*200.0f;

    // scale by 0.5x -> rotate (0,1,0) to p2 -> translate to p1
    glm::mat4 scale = glm::scale(glm::vec3(0.5f, glm::length(p2-p1), 0.5f));

    glm::vec3 dir = glm::normalize(p2-p1);
    glm::vec3 crossp = glm::cross(glm::vec3(0,1,0), glm::normalize(GetP2Pos()-GetP1Pos()));
    glm::mat4 rotate = glm::rotate(acosf(glm::dot(glm::vec3(0,1,0), dir)), crossp);

    glm::mat4 translate = glm::translate(p1);

    modelToWorld = translate*rotate*scale;
}

bool Line::RayIntersects(const Ray &ray, float *t) const
{
    return Intersects(ray, LineCylinderCollider{GetP1Pos(), GetP2Pos(), 0.5f}, t);
}

float Line::ClampToBounds(float param) const
{
    return param;
}
