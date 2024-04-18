#pragma once

#include "genericShape.h"

class GenericLine : public GenericShape
{
protected:
    GenericLine(ShapeType type);
public:
    virtual glm::vec3 GetP1Pos() const = 0;
    virtual glm::vec3 GetP2Pos() const = 0;
};
