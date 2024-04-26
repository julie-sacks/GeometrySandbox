#pragma once

#include <map>
#include <vector>
#include <glm/mat4x4.hpp>
//#include "collisions.h"

struct CollisionRay;

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
    bool isVisible;
protected:
    std::vector<int> parents;
    std::vector<int> children;
    ShapeManager* manager;

    GenericShape(ShapeType type, ShapeVisual visual);
    mutable bool isDirty;
    void SetDirty() const;
    mutable glm::mat4 modelToWorld;
    virtual void Recalculate() const = 0;
public:
    ~GenericShape();

    const int id;
    const ShapeType type;
    const ShapeVisual visual;

    bool IsPointLike() const;
    bool IsLineLike() const;

    const std::vector<int>& getChildren() const;
    const std::vector<int>& getParents() const;

    void addChild(int id);
    void removeChild(int id);

    glm::mat4 getModelToWorldMat() const;

    void SetVisibility(bool visible);
    bool GetVisibility() const;

    virtual bool RayIntersects(const CollisionRay& ray, float* t) const = 0;
};
