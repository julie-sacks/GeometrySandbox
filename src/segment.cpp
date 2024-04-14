#include "segment.h"
#include <glm/gtx/transform.hpp>
#include "shapeManager.h"
#include "point.h"

Segment::Segment(int parent1, int parent2) : GenericShape(ShapeType::Segment, ShapeVisual::Line)
{
    parents.push_back(parent1);
    parents.push_back(parent2);
}

glm::mat4 Segment::getModelToWorldMat() const
{
    glm::vec3 p1 = dynamic_cast<Point*>(manager->GetShape(parents[0]))->GetPos();
    glm::vec3 p2 = dynamic_cast<Point*>(manager->GetShape(parents[1]))->GetPos();

    // scale by 0.5x -> rotate (0,1,0) to p2 -> translate to p1
    glm::mat4 scale = glm::scale(glm::vec3(0.5f, glm::length(p2-p1), 0.5f));

    glm::vec3 dir = glm::normalize(p2-p1);
    glm::vec3 crossp = glm::cross(glm::vec3(0,1,0), glm::normalize(p2-p1));
    glm::mat4 rotate = glm::rotate(acosf(glm::dot(glm::vec3(0,1,0), dir)), crossp);

    glm::mat4 translate = glm::translate(p1);

    return translate*rotate*scale;
}

bool Segment::RayIntersects(const Ray& ray, float* t) const
{
    glm::vec3 p1 = dynamic_cast<Point*>(manager->GetShape(parents[0]))->GetPos();
    glm::vec3 p2 = dynamic_cast<Point*>(manager->GetShape(parents[1]))->GetPos();
    return Intersects(ray, CylinderCollider{p1, p2, 0.5f}, t);
}
