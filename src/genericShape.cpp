#include "genericShape.h"
#include <cassert>


int GenericShape::idcount = 0;

int GenericShape::getNextId()
{
    return idcount++;
}

GenericShape::GenericShape(ShapeType type) : type(type), id(getNextId())
{
}

GenericShape::~GenericShape()
{
    // for (GenericShape* parent : parents)
    // {
    //     parent->removeChild(this);
    // }
    // for (GenericShape* child : children)
    // {
    //     delete child;
    // }
}

const std::set<GenericShape *> &GenericShape::getChildren() const
{
    return children;
}

const std::set<GenericShape *> &GenericShape::getParents() const
{
    return parents;
}

void GenericShape::addChild(GenericShape *child)
{
    bool result = children.insert(child).second;
    assert(!result);
}

void GenericShape::removeChild(GenericShape *child)
{
    // search for the element and delete it
    children.erase(child);
}
