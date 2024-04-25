#include "genericLine.h"
#include "genericPoint.h"
#include "shapeManager.h"

GenericLine::GenericLine(ShapeType type) : GenericShape(type, ShapeVisual::Line)
{
}

glm::vec3 GenericLine::GetP1Pos() const
{
    return dynamic_cast<GenericPoint*>(manager->GetShape(parents[0]))->GetPos();
}
glm::vec3 GenericLine::GetP2Pos() const
{
    return dynamic_cast<GenericPoint*>(manager->GetShape(parents[1]))->GetPos();
}
