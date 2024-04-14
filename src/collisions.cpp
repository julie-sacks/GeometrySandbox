#include "collisions.h"
#include <glm/glm.hpp>

#define MAX(x,y) ((y) < (x) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

bool Intersects(const Ray& ray, const SphereCollider& sphere, float* rt)
{
    // early out if ray starts outside sphere and is pointed away
    // these values are also used later, so they'll be useful to have
    const glm::vec3 spheretoorigin = ray.origin-sphere.center;
    const float sqradius = sphere.radius*sphere.radius;
    if (dot(spheretoorigin,spheretoorigin) > sqradius &&
        dot(spheretoorigin,ray.direction) >= 0)
        return false;

    /* solve the system of equations
     * line:
     * x=x_0+v_x*t
     * y=same, z=same
     * 
     * sphere:
     * (x-x_1)^2 + (y-y_1)^2 + (z-z_1)^2 = r^2
     * 
     * becomes a big messy quadratic
     */

    // this whole algorithm is kinda wack, i wrote it all out on paper and it might be completely wrong
    // quadratic formula
    float A = dot(ray.direction, ray.direction);
    float B = 2*dot(ray.direction, spheretoorigin);
    float C = dot(spheretoorigin, spheretoorigin) - sqradius;

    float radicand = B*B - 4*A*C;
    // if no (real) solution exists, no intersection occurs.
    if(radicand < 0) return false;

    float root = sqrtf(radicand);
    float t_0 = (-B - root)/(2*A);
    float t_1 = (-B + root)/(2*A);

    // only count intersections on the + side of the ray
    if(t_0 < 0 && t_1 < 0) return false;
    if(rt == nullptr) return true;

    float first = MIN(t_0, t_1);
    float second = MAX(t_0, t_1);
    if(first < 0) *rt = second;
    else          *rt = first;

    return true;
}

bool Intersects(const Ray& ray, const CylinderCollider& cylinder, float *rt)
{
    // find the slice of the (assumed infinite) cylinder perpendicular to the ray origin (contains circle)
    glm::vec3 raydirnorm = normalize(ray.direction);
    glm::vec3 p1top2 = cylinder.p2-cylinder.p1;
    glm::vec3 segdir = normalize(p1top2);
    glm::vec3 perpraydir = ray.direction - segdir*glm::dot(segdir, raydirnorm);

    glm::vec3 raytop1 = cylinder.p1 - ray.origin;
    glm::vec3 raytop1norm = normalize(raytop1);
    glm::vec3 circlecenter = cylinder.p1 + segdir*glm::dot(-raytop1, segdir);

    // important values at this point: circlecenter, perpraydir, ray.origin, cylinder.radius
    // find intersection between the perpendicular ray and the circle (intersection on an infinite cylinder)
    glm::vec3 perpraynorm = normalize(perpraydir);
    glm::vec3 raytocenter = circlecenter - ray.origin;
    glm::vec3 raytocenternorm = normalize(raytocenter);
    float t = dot(raytocenter, perpraynorm);
    glm::vec3 closestpoint = ray.origin + perpraynorm*t;
    float distsquared = dot(closestpoint - circlecenter, closestpoint - circlecenter);
    if(distsquared > cylinder.radius * cylinder.radius) return false;
    //assert(false); // don't go past here yet
    float deltat = sqrt(cylinder.radius*cylinder.radius - distsquared);

    // intersection t values
    float t1 = (t-deltat) / dot(ray.direction, perpraynorm);
    float t2 = (t+deltat) / dot(ray.direction, perpraynorm);

    // t1 should be the closest intersection.
    // if it's negative then replace it with the second intersection
    if(t1 < 0) t1 = t2;

    // then find point of intersection in 3d
    // check if it falls within the cylinder's bounds on the parallel axis
    // might run into problems here with the normalized direction vectors
    glm::vec3 intersection = ray.origin + t1*ray.direction;
    glm::vec3 p1tointersection = intersection - cylinder.p1;
    // 0..1
    float p1toidist = dot(p1tointersection, p1top2)/dot(p1top2, p1top2);
    if(p1toidist < 0 || p1toidist > 1) return false;

    if(rt != nullptr) *rt = t1;

    return true;
}
