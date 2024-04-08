#include "shapeManager.h"

ShapeManager::~ShapeManager()
{
    for(GenericShape* shape : shapeList)
    {
        delete shape;
    }
}

void ShapeManager::AddShape(GenericShape* shape)
{
    shapeList.insert(shape);
}

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

const std::set<GenericShape*>& ShapeManager::GetShapeList() const
{
    return shapeList;
}
