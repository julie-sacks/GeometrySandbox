#pragma once

#include "genericShape.h"
#include <set>


class ShapeManager
{
private:
    std::set<GenericShape*> shapeList;

public:
    void AddShape(GenericShape* shape);
    void RemoveShape(GenericShape* shape);
};
