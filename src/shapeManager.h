#pragma once

#include "genericShape.h"
#include <set>


class ShapeManager
{
private:
    std::set<GenericShape*> shapeList;

public:
    ~ShapeManager();
    void AddShape(GenericShape* shape);
    void RemoveShape(GenericShape* shape);
    const std::set<GenericShape*>& GetShapeList() const;
};
