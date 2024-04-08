#pragma once

#include <set>
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

class GenericShape
{
private:
    static int idcount;
    int getNextId();
    std::set<GenericShape*> parents;
    std::set<GenericShape*> children;
public:
    const int id;
    const ShapeType type;
    const ShapeVisual visual;
    GenericShape(ShapeType type, ShapeVisual visual);
    ~GenericShape();

    const std::set<GenericShape*>& getChildren() const;
    const std::set<GenericShape*>& getParents() const;

    // feels weird to have different indices for these, might want to reference a global object list using id instead of pointers directly to the objects
    void addChild(GenericShape* child);
    void removeChild(GenericShape* child);

    // needed uniforms: mat4 modelToWorld
    virtual glm::mat4 getModelToWorldMat() const = 0;
};
