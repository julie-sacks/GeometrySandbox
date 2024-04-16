#pragma once

#include <map>
#include <vector>
#include <glm/mat4x4.hpp>
#include "collisions.h"

enum class ShapeType
{
    None = -1,
    Point,
    Line,
    Segment,
    Circle,
    Sphere,
    Midpoint,

    Count
};
const char* GetShapeTypeString(ShapeType type);

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
class ShapeManager;

class GenericShape
{
friend class ShapeManager;
private:
    static int idcount;
    int getNextId();
protected:
    std::vector<int> parents;
    std::vector<int> children;
    ShapeManager* manager;
public:
    const int id;
    const ShapeType type;
    const ShapeVisual visual;
    GenericShape(ShapeType type, ShapeVisual visual);
    ~GenericShape();

    const std::vector<int>& getChildren() const;
    const std::vector<int>& getParents() const;

    void addChild(int id);
    void removeChild(int id);

    virtual glm::mat4 getModelToWorldMat() const = 0;

    virtual bool RayIntersects(const Ray& ray, float* t) const = 0;
};
