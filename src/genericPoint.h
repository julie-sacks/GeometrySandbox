#pragma once

#include "genericShape.h"

class GenericPoint : public GenericShape
{
protected:
    GenericPoint(ShapeType type);
public:
    virtual glm::vec3 GetPos() const = 0;
};