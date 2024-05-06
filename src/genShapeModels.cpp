#include "genShapeModels.h"
#include <cmath>
#include <glm/glm.hpp>

using glm::vec3;
using glm::uvec3;

// generate a list of vertices in rings from bottom to top and its corresponding indices
std::pair<std::vector<vec3>, std::vector<uvec3>> GenSphere(int steps)
{
    // vertices + normals
    std::vector<vec3> pts;
    pts.push_back(vec3(0,-1,0)); // pos
    pts.push_back(vec3(0,-1,0)); // norm

    // for each ring
    for(int i = 1; i < steps; ++i)
    {
        float vertangle = M_PI * ((float)i / steps);
        float y = -cosf(vertangle);
        float r = sinf(vertangle);
        // for each point in the ring
        for(int j = 0; j < steps*2; ++j)
        {
            float horizangle = M_PI * ((float)j/steps);
            // swap these if tris are inside out
            float x = r*sinf(horizangle);
            float z = r*cosf(horizangle);

            pts.push_back(vec3(x,y,z)); // pos
            pts.push_back(vec3(x,y,z)); // norm
        }
    }

    pts.push_back(vec3(0,1,0)); // pos
    pts.push_back(vec3(0,1,0)); // norm

    // incides
    std::vector<uvec3> idx;
    // bottom disk (0,1,2,  0,2,3,  0,3,4...  0,10,1)
    for(int i = 0; i < steps*2; ++i)
    {
        idx.push_back(uvec3(0,(i+1)%(steps*2)+1,i+1));
    }
    // rings (1,2,11,  2,12,11,  2,3,12,  3,13,12)
    for(int i = 0; i < steps-2; ++i)
    {
        for(int j = 0; j < steps*2; ++j)
        {
            idx.push_back(uvec3( i   *(steps*2) +  j+1,
                                 i   *(steps*2) + (j+1)%(steps*2)+1,
                                (i+1)*(steps*2) +  j+1));
            idx.push_back(uvec3( i   *(steps*2) + (j+1)%(steps*2)+1,
                                (i+1)*(steps*2) + (j+1)%(steps*2)+1,
                                (i+1)*(steps*2) +  j+1));
        }
    }
    // top disk (11,1,2,  11,2,3...  11,10,1)
    for(int i = 0; i < steps*2; ++i)
    {
        //(steps*steps*2)
        int last   = (steps-1)*steps*2 + 1;
        int middle = (steps-2)*steps*2;
        idx.push_back(uvec3(last,middle+i+1,middle+(i+1)%(steps*2)+1));
    }

    // sanity check
    for(int i = 0; i < idx.size(); ++i)
    {
        assert(idx[i].x < pts.size()/2);
        assert(idx[i].y < pts.size()/2);
        assert(idx[i].z < pts.size()/2);
    }

    return std::pair<std::vector<vec3>, std::vector<uvec3>>(pts, idx);
}

// almost identical to sphere, but one ring of tris always at full radius. (-1..1, 0..1, -1..1)
std::pair<std::vector<glm::vec3>, std::vector<glm::uvec3>> GenCylinder(int steps)
{
    // vertices + normals
    std::vector<vec3> pts;
    pts.push_back(vec3(0,0,0));  // pos
    pts.push_back(vec3(0,-1,0)); // norm

    // two rings of verts
    for(int i = 0; i < 2; ++i)
    for(int j = 0; j < steps*2; ++j)
    {
        float horizangle = M_PI * ((float)j/steps);
        // swap these if tris are inside out
        float x = sinf(horizangle);
        float z = cosf(horizangle);

        pts.push_back(vec3(x,i,z)); // pos
        pts.push_back(vec3(x,0,z)); // norm
    }

    pts.push_back(vec3(0,1,0)); // pos
    pts.push_back(vec3(0,1,0)); // norm

    // incides
    std::vector<uvec3> idx;
    // bottom disk (0,1,2,  0,2,3,  0,3,4...  0,10,1)
    for(int i = 0; i < steps*2; ++i)
    {
        idx.push_back(uvec3(0,(i+1)%(steps*2)+1,i+1));
    }
    // rings (1,2,11,  2,12,11,  2,3,12,  3,13,12)
    for(int j = 0; j < steps*2; ++j)
    {
        idx.push_back(uvec3( 0*(steps*2) +  j+1,
                             0*(steps*2) + (j+1)%(steps*2)+1,
                             1*(steps*2) +  j+1));
        idx.push_back(uvec3( 0*(steps*2) + (j+1)%(steps*2)+1,
                             1*(steps*2) + (j+1)%(steps*2)+1,
                             1*(steps*2) +  j+1));
    }
    // top disk (11,1,2,  11,2,3...  11,10,1)
    for(int i = 0; i < steps*2; ++i)
    {
        //(steps*steps*2)
        int last   = 2*steps*2 + 1;
        int middle = 1*steps*2;
        idx.push_back(uvec3(last,middle+i+1,middle+(i+1)%(steps*2)+1));
    }

    // sanity check
    for(int i = 0; i < idx.size(); ++i)
    {
        assert(idx[i].x < pts.size()/2);
        assert(idx[i].y < pts.size()/2);
        assert(idx[i].z < pts.size()/2);
    }

    return std::pair<std::vector<vec3>, std::vector<uvec3>>(pts, idx);
}

std::pair<std::vector<vec3>, std::vector<uvec3>> GenTorus(int steps)
{
    /* start on outer middle (1,0,0)
     * do smaller rings (0.75+0.25*cos(t),0.25*sin(t),0)
     * rotate along torus (x*cos(u),y,z*sin(u))
     *
     * x = cos(u) * (0.75+0.25*cos(t))
     * y = 0.25*sin(t)
     * z = sin(u) + (0.75+0.25*cos(t))
     */
    constexpr float minorRadius = 1.0f;
    constexpr float majorRadius = 1.0f;

    std::vector<vec3> verts;
    for(int i = 0; i < steps*2; ++i)
    {
        float majorangle = i * (2*M_PI)/(steps*2);
        float xmul = cosf(majorangle);
        float zmul = sinf(majorangle);
        for(int j = 0; j < steps; ++j)
        {
            float minorangle = j * (2*M_PI)/(steps);
            float x = xmul * (majorRadius + minorRadius * cosf(minorangle));
            float y = minorRadius * sinf(minorangle);
            float z = zmul * (majorRadius + minorRadius * cosf(minorangle));

            //verts.push_back(vec3(x,y,z)); // pos
            verts.push_back(vec3(xmul, 0, zmul)); // pos
            vec3 norm = vec3(x,y,z) - vec3(xmul*majorRadius, 0, zmul*majorRadius);
            verts.push_back(glm::normalize(norm)); // norm
        }
    }

    std::vector<uvec3> idx;
    for(int i = 0; i < steps*2; ++i)
    {
        for(int j = 0; j < steps; ++j)
        {
            int a = j+i*steps;
            int b = (j+1)%steps+i*steps;
            int c = j+((i+1)%(steps*2))*steps;
            idx.push_back(uvec3(a,b,c));
            a = b;
            b = (j+1)%steps+((i+1)%(steps*2))*steps;
            c = c;
            idx.push_back(uvec3(a,b,c));
        }
    }
    return std::pair<std::vector<vec3>, std::vector<uvec3>>(verts,idx);
}

