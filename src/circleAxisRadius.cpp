#include "circleAxisRadius.h"
#include "shapeManager.h"
#include "genericPoint.h"
#include "genericLine.h"
#include "glm/gtx/transform.hpp"

void CircleAxisRadius::Recalculate() const
{
    isDirty = false;
    outer = dynamic_cast<GenericPoint*>(manager->GetShape(parents[1]))->GetPos();
    // project point onto line
    GenericLine* line = dynamic_cast<GenericLine*>(manager->GetShape(parents[0]));
    glm::vec3 delta = outer - line->GetP1Pos();
    glm::vec3 normvec = glm::normalize(line->GetP2Pos() - line->GetP1Pos());
    float dotp = dot(delta, normvec);
    center = line->GetP1Pos() + normvec*dotp;
    radius = glm::distance(outer, center);

    glm::vec3 crossp = glm::cross(glm::vec3(0,1,0), normvec);
    glm::mat4 rotate = glm::rotate(acosf(glm::dot(glm::vec3(0,1,0), normvec)), crossp);
    // fix case where dir = [0,1,0], i.e. no rotation needed
    if(normvec == glm::vec3(0,1,0)) rotate = glm::mat4(1);

    modelToWorld = glm::translate(center) * rotate;
}

CircleAxisRadius::CircleAxisRadius(int center, int outer) : GenericCircle(ShapeType::CircleAR)
{
    parents.push_back(center);
    parents.push_back(outer);
}

glm::vec3 CircleAxisRadius::GetCenter() const
{
    if(isDirty) Recalculate();
    return center;
}

glm::vec3 CircleAxisRadius::GetOuter() const
{
    if(isDirty) Recalculate();
    return outer;
}

bool CircleAxisRadius::RayIntersects(const CollisionRay& ray, float* t) const
{
    // TODO: collision checking
    return false;
}
