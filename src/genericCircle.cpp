#include "genericCircle.h"

GenericCircle::GenericCircle(ShapeType type) : GenericShape(type, ShapeVisual::Circle)
{
}

float GenericCircle::GetRadius() const
{
    return glm::distance(GetCenter(), GetOuter());
}
