#include "genericShape.h"


int genericShape::idcount = 0;

int genericShape::getNextId()
{
    return idcount++;
}

genericShape::genericShape(/* args */) : id(getNextId())
{
}

genericShape::~genericShape()
{
    for (genericShape* parent : parents)
    {
        parent->removeChild(this);
    }
    for (genericShape* child : children)
    {
        delete child;
    }
}

bool genericShape::operator<(const genericShape &rhs) const
{
    return id < rhs.id;
}

void genericShape::addChild(genericShape *child)
{
    children.insert(child);
}

void genericShape::removeChild(genericShape *child)
{
    children.erase(child);
}
