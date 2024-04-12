#include "shapeManager.h"
#include "genShapeModels.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cassert>
#include "point.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <limits>

using nlohmann::json;

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

void ShapeManager::ClearShapes()
{
    for(auto& shape : shapeList)
    {
        delete shape.second;
    }
    shapeList.clear();
    GenericShape::idcount = 0;
}

void ShapeManager::AddShape(GenericShape* shape)
{
    assert(shapeList.find(shape->id) == shapeList.end());
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

int ShapeManager::SelectRaycast(const Ray& ray, bool multiselect)
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
    }

    if(closestId == -1) return closestId;

    // ensure the shape doesn't get added twice
    for(int id : selectedIds)
    {
        if(id == closestId) return closestId;
    }

    selectedIds.push_back(closestId);
    return true;
}

// TODO: proper data validation
bool ShapeManager::LoadFromFile(const char *path)
{
    std::ifstream file(path);
    if(!file.is_open())
    {
        printf("couldn't open file \"%s\" for read\n", path);
        return false;
    }

    json data = json::parse(file);

    assert(data.find("shapes") != data.end());



    ClearShapes();
    int maxid = 0;

    for(auto entry : data.at("shapes"))
    // loop through shapes in file
    {
        // read id from file
        assert(entry.find("id") != entry.end());
        GenericShape::idcount = entry.at("id"); // this is a way to set the id of the next genned shape

        // read what type to generate
        assert(entry.find("type") != entry.end());
        std::string typestr = entry.at("type");
        ShapeType type = ShapeType::None;
        // TODO: expand this list as more shapes get implemented
        if(typestr.compare("point") == 0)
            type = ShapeType::Point;

        GenericShape* shape;
        // read type-specific params in switch statement
        switch (type)
        {
        case ShapeType::Point:
        {
            shape = new Point(glm::vec3(0));

            // read position
            assert(entry.find("position") != entry.end());
            auto position = entry.find("position").value();
            dynamic_cast<Point*>(shape)->pos = glm::vec3(position[0],position[1],position[2]);
        }   break;
        
        default:
            break;
        }
        // read parents
        assert(entry.find("parents") != entry.end());
        shape->parents = {entry.at("parents").begin(), entry.at("parents").end()};
        // read children
        assert(entry.find("children") != entry.end());
        shape->children = {entry.at("children").begin(), entry.at("children").end()};

        AddShape(shape);
        // update maxid
        maxid = (GenericShape::idcount > maxid) ? GenericShape::idcount : maxid;
    }
    // ensure the next object created has a valid id
    GenericShape::idcount = maxid;

    return true;
}

bool ShapeManager::SaveToFile(const char* path)
{
    std::ofstream file(path);
    if(!file.is_open())
    {
        printf("couldn't open file \"%s\" for write\n", path);
        return false;
    }

    json data;
    data["shapes"] = json::array();
    for(const auto& shape : shapeList)
    {
        json shapedata;
        // store id
        shapedata["id"] = shape.second->id;
        // store parents and children
        shapedata["parents"] = shape.second->parents;
        shapedata["children"] = shape.second->children;

        // store type and type-related members
        switch (shape.second->type)
        {
        case ShapeType::Point:
        {
            shapedata["type"] = "point";
            glm::vec3& pos = dynamic_cast<Point*>(shape.second)->pos;
            shapedata["position"] = {pos.x, pos.y, pos.z};
        }   break;
        
        default:
            break;
        }

        data["shapes"].push_back(shapedata);
    }
    file << data.dump(4);

    return true;
}
