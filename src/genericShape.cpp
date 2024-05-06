#include "genericShape.h"
#include "collisions.h"
#include "shapeManager.h"
#include <cassert>


int GenericShape::idcount = 0;

int GenericShape::getNextId()
{
    return idcount++;
}

GenericShape::GenericShape(ShapeType type, ShapeVisual visual) :
    type(type), visual(visual), id(getNextId()), isDirty(true), isVisible(true)
{
}

void GenericShape::SetDirty() const
{
    isDirty = true;
    for(int childId : children)
    {
        manager->GetShape(childId)->SetDirty();
    }
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

bool GenericShape::IsPointLike() const
{
    switch (type)
    {
    case ShapeType::Point:
        return true;
    case ShapeType::Midpoint:
        return true;
    default:
        return false;
    }
}

bool GenericShape::IsLineLike() const
{
    switch (type)
    {
    case ShapeType::Segment:
        return true;
    case ShapeType::Line:
        return true;
    case ShapeType::Ray:
        return true;
    default:
        return false;
    }
}

const std::vector<int> &GenericShape::getChildren() const
{
    return children;
}

const std::vector<int> &GenericShape::getParents() const
{
    return parents;
}

void GenericShape::addChild(int id)
{
    children.push_back(id);
}

void GenericShape::removeChild(int id)
{
    // search for the element and delete it
    for(auto it = children.begin(); it != children.end(); ++it)
    {
        if(*it != id) continue;
        children.erase(it);
        break;
    }
}

glm::mat4 GenericShape::getModelToWorldMat() const
{
    if(isDirty) Recalculate();
    return modelToWorld;
}

void GenericShape::SetVisibility(bool visible)
{
    isVisible = visible;
}
bool GenericShape::GetVisibility() const
{
    return isVisible;
}

bool ShapeIdLess::operator()(const GenericShape *lhs, const GenericShape *rhs) const
{
    return lhs->id < rhs->id;
}

const char* GetShapeTypeString(ShapeType type)
{
    switch (type)
    {
    case ShapeType::Point:
        return "Point";
    case ShapeType::Segment:
        return "Segment";
    case ShapeType::Midpoint:
        return "Midpoint";
    case ShapeType::Line:
        return "Line";
    case ShapeType::Ray:
        return "Ray";
    case ShapeType::Sphere:
        return "Sphere";
    case ShapeType::CircleAR:
        return "Circle";

    default:
        return "Shape";
    }
}
