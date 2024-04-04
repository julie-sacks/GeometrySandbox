#include "shapeManager.h"

void ShapeManager::RemoveShape(GenericShape* shape)
{
    if(shapeList.find(shape) == shapeList.end()) return;

    shapeList.erase(shape);

    for(auto parent : shape->getParents())
    {
        parent->removeChild(shape);
    }
    // a little memory inefficient, but it should work
    std::set<GenericShape*> templist = shape->getChildren();
    for(auto child : templist)
    {
        RemoveShape(child);
    }
    delete shape;
}