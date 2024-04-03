#pragma once

#include <set>

class genericShape
{
private:
    static int idcount;
    int getNextId();
    const int id; // used for exporting to file
    std::set<genericShape*> parents;
    std::set<genericShape*> children;
public:
    genericShape(/* args */);
    ~genericShape();

    // for set ordering
    bool operator<(const genericShape& rhs) const;

    const std::set<genericShape*>& getChildren() const;
    const std::set<genericShape*>& getParents() const;

    // feels weird to have different indices for these, might want to reference a global object list using id instead of pointers directly to the objects
    void addChild(genericShape* child);
    void removeChild(genericShape* child);
};
