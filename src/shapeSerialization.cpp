#include <fstream>
#include <cassert>
#include "json.hpp"
#include "shapeManager.h"
#include "point.h"
#include "segment.h"
#include "midpoint.h"
#include "line.h"

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
        assert(entry.at("id").is_number_integer());
        GenericShape::idcount = entry.at("id"); // this is a way to set the id of the next genned shape

        // read what type to generate
        assert(entry.find("type") != entry.end());
        assert(entry.at("type").is_string());
        std::string typestr = entry.at("type");
        ShapeType type = ShapeType::None;
        // TODO: expand this list as more shapes get implemented
        if(typestr.compare("point") == 0)
            type = ShapeType::Point;
        if(typestr.compare("segment") == 0)
            type = ShapeType::Segment;
        if(typestr.compare("midpoint") == 0)
            type = ShapeType::Midpoint;
        if(typestr.compare("line") == 0)
            type = ShapeType::Line;

        assert(type != ShapeType::None);

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

        case ShapeType::Midpoint:
        {
            shape = new Midpoint(0,0);

            // read position
            assert(entry.find("position") != entry.end());
            assert(entry.at("position").is_number());
            float position = entry.at("position");
            dynamic_cast<Midpoint*>(shape)->t = position;
        }   break;

        case ShapeType::Line:
        {
            shape = new Line(0,0);
        }   break;

        default:
            break;
        }

        // read parents
        assert(entry.find("parents") != entry.end());
        shape->parents = {entry.at("parents").begin(), entry.at("parents").end()};

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

        case ShapeType::Midpoint:
        {
            shapedata["type"] = "midpoint";
            shapedata["position"] = dynamic_cast<Midpoint*>(shape.second)->t;
        }   break;

        case ShapeType::Line:
        {
            shapedata["type"] = "line";
        }   break;

        default:
            break;
        }

        data["shapes"].push_back(shapedata);
    }
    file << data.dump(4);

    return true;
}
