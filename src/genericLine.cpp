#include "genericLine.h"
#include "genericPoint.h"
#include "shapeManager.h"
#include "glm/gtx/transform.hpp"

GenericLine::GenericLine(ShapeType type) : GenericShape(type, ShapeVisual::Line)
{
}

glm::mat4 GenericLine::GetRotationMat() const
{
    glm::vec3 dir = glm::normalize(GetP2Pos()-GetP1Pos());
    glm::vec3 crossp = glm::cross(glm::vec3(0,1,0), glm::normalize(GetP2Pos()-GetP1Pos()));
    glm::mat4 rotate = glm::rotate(acosf(glm::dot(glm::vec3(0,1,0), dir)), crossp);
    // fix case where dir = [0,1,0], i.e. no rotation needed
    if(dir == glm::vec3(0,1,0)) rotate = glm::mat4(1);
    return rotate;
}

glm::vec3 GenericLine::GetP1Pos() const
{
    return dynamic_cast<GenericPoint*>(manager->GetShape(parents[0]))->GetPos();
}
glm::vec3 GenericLine::GetP2Pos() const
{
    return dynamic_cast<GenericPoint*>(manager->GetShape(parents[1]))->GetPos();
}
