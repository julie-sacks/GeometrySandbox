#pragma once

#include <set>

class GenericShape
{
private:
    static int idcount;
    int getNextId();
    std::set<GenericShape*> parents;
    std::set<GenericShape*> children;
public:
    const int id;
    GenericShape(/* args */);
    ~GenericShape();

    const std::set<GenericShape*>& getChildren() const;
    const std::set<GenericShape*>& getParents() const;

    // feels weird to have different indices for these, might want to reference a global object list using id instead of pointers directly to the objects
    void addChild(GenericShape* child);
    void removeChild(GenericShape* child);
};
