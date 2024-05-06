#pragma once

#include "genericShape.h"

class GenericCircle : public GenericShape
{
protected:
    GenericCircle(ShapeType type);
public:
    virtual glm::vec3 GetCenter() const = 0;
    virtual glm::vec3 GetOuter() const = 0;
    virtual float GetRadius() const;
};
