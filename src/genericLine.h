#pragma once

#include "genericShape.h"

class GenericLine : public GenericShape
{
protected:
    GenericLine(ShapeType type);
    glm::mat4 GetRotationMat() const;
public:
    virtual glm::vec3 GetP1Pos() const;
    virtual glm::vec3 GetP2Pos() const;
    // if necessary, clamp the parameter to be on the shape's bounds
    virtual float ClampToBounds(float param) const = 0;
};
