#include "point.h"
#include <glm/gtx/transform.hpp>

Point::Point(glm::vec3 pos) : GenericShape(ShapeType::Point, ShapeVisual::Sphere), pos(pos)
{
}

glm::mat4 Point::getModelToWorldMat() const
{
    return glm::translate(pos);
}
