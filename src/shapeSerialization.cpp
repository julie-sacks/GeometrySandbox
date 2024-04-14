#include <fstream>
#include <cassert>
#include "json.hpp"
#include "shapeManager.h"
#include "point.h"
#include "segment.h"

using nlohmann::json;

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
        if(typestr.compare("segment") == 0)
            type = ShapeType::Segment;

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
            dynamic_cast<Point*>(shape)->position = glm::vec3(position[0],position[1],position[2]);
        }   break;

        case ShapeType::Segment:
        {
            shape = new Segment(0,0);
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
            glm::vec3& pos = dynamic_cast<Point*>(shape.second)->position;
            shapedata["position"] = {pos.x, pos.y, pos.z};
        }   break;
        case ShapeType::Segment:
        {
            shapedata["type"] = "segment";
        }   break;
        
        default:
            break;
        }

        data["shapes"].push_back(shapedata);
    }
    file << data.dump(4);

    return true;
}
