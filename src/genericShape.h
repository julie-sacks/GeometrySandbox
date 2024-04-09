#pragma once

#include <map>
#include <glm/mat4x4.hpp>

enum class ShapeType
{
    None = -1,
    Point,
    Line,
    Segment,
    Circle,
    Sphere,

    Count
};
enum class ShapeVisual
{
    None = -1,
    Sphere,
    Line,
    Circle,

    Count
};

class GenericShape;

class ShapeIdLess
{
public:
    bool operator()(const GenericShape* lhs, const GenericShape* rhs) const;
};

typedef std::map<int, GenericShape*> ShapeSet;

class GenericShape
{
private:
    static int idcount;
    int getNextId();
    ShapeSet parents;
    ShapeSet children;
public:
    const int id;
    const ShapeType type;
    const ShapeVisual visual;
    GenericShape(ShapeType type, ShapeVisual visual);
    ~GenericShape();

    const ShapeSet& getChildren() const;
    const ShapeSet& getParents() const;

    // feels weird to have different indices for these, might want to reference a global object list using id instead of pointers directly to the objects
    void addChild(GenericShape* child);
    void removeChild(GenericShape* child);

    virtual glm::mat4 getModelToWorldMat() const = 0;
};
