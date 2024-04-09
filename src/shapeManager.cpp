#include "shapeManager.h"
#include "genShapeModels.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cassert>

void ShapeManager::GenVao(ShapeVisual visual) const
{
    std::pair<std::vector<glm::vec3>, std::vector<glm::uvec3>> data;
    switch (visual)
    {
    case ShapeVisual::Sphere:
        data = GenSphere();
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

void ShapeManager::AddShape(GenericShape* shape)
{
    shapeList.insert({shape->id, shape});
}

void ShapeManager::RemoveShape(int id)
{
    assert(shapeList.find(id) == shapeList.end());
    GenericShape* shape = shapeList.at(id);
    assert(shape);

    shapeList.erase(id);

    for(auto& parent : shape->getParents())
    {
        GetShape(parent)->removeChild(id);
    }
    // a little memory inefficient, but it should work
    std::vector<int> templist = shape->getChildren();
    for(auto& child : templist)
    {
        RemoveShape(child);
    }
    delete shape;
}

GenericShape* ShapeManager::GetShape(int id)
{
    assert(shapeList.find(id) == shapeList.end());
    return shapeList.at(id);
}

const GenericShape* ShapeManager::GetShape(int id) const
{
    assert(shapeList.find(id) == shapeList.end());
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

    for(auto& shape : shapeList)
    {
        glm::mat4 modelToWorld = shape.second->getModelToWorldMat();
        glUniformMatrix4fv(ulModelToWorld, 1, GL_FALSE, &modelToWorld[0][0]);
        glBindVertexArray(GetVao(shape.second->visual));
        glDrawElements(GL_TRIANGLES, GetIdxCount(shape.second->visual), GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}
