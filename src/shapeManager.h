#pragma once

#include "genericShape.h"
#include <set>


class ShapeManager
{
private:
    ShapeSet shapeList;
    mutable unsigned int vaos[(size_t)ShapeVisual::Count] {0};
    mutable unsigned int vbos[(size_t)ShapeVisual::Count] {0};
    mutable unsigned int ebos[(size_t)ShapeVisual::Count] {0};
    mutable unsigned int idxCount[(size_t)ShapeVisual::Count] {0};
    // lazy generation
    void GenVao(ShapeVisual visual) const;
    unsigned int GetVao(ShapeVisual visual) const;
    unsigned int GetIdxCount(ShapeVisual visual) const;
public:
    ~ShapeManager();
    void ClearShapes();
    void AddShape(GenericShape* shape);
    void RemoveShape(int id);
    GenericShape* GetShape(int id);
    const GenericShape* GetShape(int id) const;
    const ShapeSet& GetShapeList() const;
    // before calling, configure all uniforms other than `mat4 ModelToWorld`
    void Draw(unsigned int shader) const;

    void LoadFromFile(const char* path);
};
