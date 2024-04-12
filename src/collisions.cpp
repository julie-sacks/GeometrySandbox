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
