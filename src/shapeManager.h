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

    std::vector<int> selectedIds;
public:
    ~ShapeManager();
    void ClearShapes();
    void AddShape(GenericShape* shape);
    void RemoveShape(int id);
    GenericShape* GetShape(int id);
    const GenericShape* GetShape(int id) const;
    const ShapeSet& GetShapeList() const;
    // before calling, configure all uniforms other than:
    // `mat4 modelToWorld`, `mat4 normalTransform and `baseColor`
    void Draw(unsigned int shader) const;

    // returns selected shape id (even if it was already selected), or -1
    int SelectRaycast(const CollisionRay& ray, bool multiselect = false);
    int Select(int newid);
    bool Deselect(int id);
    bool IsSelected(int id) const;
    const std::vector<int>& GetSelected() const;
    int GetSelectedCount() const;

    bool LoadFromFile(const char* path);
    bool SaveToFile(const char* path);
};
