#include "shapeManager.h"
#include "genShapeModels.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cassert>
#include "point.h"
#include <limits>


void ShapeManager::GenVao(ShapeVisual visual) const
{
    std::pair<std::vector<glm::vec3>, std::vector<glm::uvec3>> data;
    switch (visual)
    {
    case ShapeVisual::Sphere:
        data = GenSphere();
        break;

    case ShapeVisual::Line:
        data = GenCylinder();
        break;

    default:
        // unimplemented behavior, please implement model generation
        assert(false);
        break;
    }

    idxCount[(size_t)visual] = data.second.size()*3;

    GLuint& vao = vaos[(size_t)visual];
    GLuint& vbo = vbos[(size_t)visual];
    GLuint& ebo = ebos[(size_t)visual];
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.first.size()*sizeof(glm::vec3), data.first.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.second.size()*sizeof(glm::uvec3), data.second.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}

unsigned int ShapeManager::GetVao(ShapeVisual visual) const
{
    if(vaos[(size_t)visual] == 0)
        GenVao(visual);
    return vaos[(size_t)visual];
}
unsigned int ShapeManager::GetIdxCount(ShapeVisual visual) const
{
    return idxCount[(size_t)visual];
}

ShapeManager::~ShapeManager()
{
    for(auto& shape : shapeList)
    {
        delete shape.second;
    }
    glDeleteVertexArrays((int)ShapeVisual::Count, vaos);
    glDeleteBuffers((int)ShapeVisual::Count, vbos);
    glDeleteBuffers((int)ShapeVisual::Count, vbos);
}

void ShapeManager::ClearShapes()
{
    for(auto& shape : shapeList)
    {
        delete shape.second;
    }
    shapeList.clear();
    selectedIds.clear();
    GenericShape::idcount = 0;
}

void ShapeManager::AddShape(GenericShape* shape)
{
    assert(shapeList.find(shape->id) == shapeList.end());

    for(int parentid : shape->parents)
    {
        GetShape(parentid)->addChild(shape->id);
    }

    shapeList.insert({shape->id, shape});
    shape->manager = this;
}

void ShapeManager::RemoveShape(int id)
{
    assert(shapeList.find(id) != shapeList.end());
    GenericShape* shape = shapeList.at(id);
    assert(shape);

    shapeList.erase(id);

    for(auto& parentid : shape->getParents())
    {
        GenericShape* parent = GetShape(parentid);
        if(parent == nullptr) continue;
        GetShape(parentid)->removeChild(id);
    }
    // a little memory inefficient, but it should work
    std::vector<int> templist = shape->getChildren();
    for(auto& child : templist)
    {
        if(GetShape(child) == nullptr) continue;
        RemoveShape(child);
    }
    delete shape;
}

GenericShape* ShapeManager::GetShape(int id)
{
    if(shapeList.find(id) == shapeList.end()) return nullptr;
    return shapeList.at(id);
}

const GenericShape* ShapeManager::GetShape(int id) const
{
    assert(shapeList.find(id) != shapeList.end());
    return shapeList.at(id);
}

const ShapeSet& ShapeManager::GetShapeList() const
{
    return shapeList;
}

void ShapeManager::Draw(unsigned int shader) const
{
    GLint ulModelToWorld = glGetUniformLocation(shader, "modelToWorld");
    assert(ulModelToWorld != -1);
    GLint ulNormalTransform = glGetUniformLocation(shader, "normalTransform");
    assert(ulNormalTransform != -1);
    GLint ulBaseColor = glGetUniformLocation(shader, "baseColor");
    assert(ulBaseColor != -1);

    for(auto& shape : shapeList)
    {
        if(!GetShape(shape.first)->GetVisibility()) continue;
        glm::vec3 color(1,1,1);
        if(IsSelected(shape.first))
            color = glm::vec3(1,1,0);
        glUniform3fv(ulBaseColor, 1, &color[0]);

        glm::mat4 modelToWorld = shape.second->getModelToWorldMat();
        glUniformMatrix4fv(ulModelToWorld, 1, GL_FALSE, &modelToWorld[0][0]);
        glm::mat4 normalTransform = glm::transpose(glm::inverse(modelToWorld));
        glUniformMatrix4fv(ulNormalTransform, 1, GL_FALSE, &normalTransform[0][0]);
        glBindVertexArray(GetVao(shape.second->visual));
        glDrawElements(GL_TRIANGLES, GetIdxCount(shape.second->visual), GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

int ShapeManager::SelectRaycast(const CollisionRay& ray, bool multiselect)
{
    if(!multiselect) selectedIds.clear();

    int closestId = -1;
    float closestT = MAXFLOAT;
    // only select the object closest to the camera
    for(const auto& shape : shapeList)
    {
        float t;
        if(!shape.second->RayIntersects(ray, &t)) continue;
        if(t > closestT) continue;
        closestId = shape.first;
        closestT = t;
    }

    if(closestId == -1)
        return closestId;

    return Select(closestId);
}

int ShapeManager::Select(int newid)
{
    // ensure the shape doesn't get added twice
    for(int id : selectedIds)
    {
        if(newid == id) return newid;
    }

    selectedIds.push_back(newid);
    return newid;
}

bool ShapeManager::Deselect(int id)
{
    for(auto it = selectedIds.begin(); it != selectedIds.end(); ++it)
    {
        if(id != *it) continue;
        selectedIds.erase(it);
        return true;
    }
    // id isn't selected currently
    return false;
}

bool ShapeManager::IsSelected(int id) const
{
    for(int selectedId : selectedIds)
    {
        if(id == selectedId) return true;
    }
    return false;
}

const std::vector<int>& ShapeManager::GetSelected() const
{
    return selectedIds;
}

int ShapeManager::GetSelectedCount() const
{
    return selectedIds.size();
}
