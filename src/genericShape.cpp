#include "genericShape.h"
#include <cassert>


int GenericShape::idcount = 0;

int GenericShape::getNextId()
{
    return idcount++;
}

GenericShape::GenericShape(ShapeType type, ShapeVisual visual) :
    type(type), visual(visual), id(getNextId())
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

const ShapeSet &GenericShape::getChildren() const
{
    return children;
}

const ShapeSet &GenericShape::getParents() const
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

bool ShapeIdLess::operator()(const GenericShape *lhs, const GenericShape *rhs) const
{
    return lhs->id < rhs->id;
}
