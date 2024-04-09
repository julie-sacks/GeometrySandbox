#include "genShapeModels.h"
#include <cmath>

using glm::vec3;
using glm::uvec3;

// generate a list of vertices in rings from bottom to top and its corresponding indices
std::pair<std::vector<vec3>, std::vector<uvec3>> GenSphere(int steps)
{
    // vertices
    std::vector<vec3> pts;
    pts.push_back(vec3(0,-1,0));

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

            pts.push_back(vec3(x,y,z));
        }
    }

    pts.push_back(vec3(0,1,0));

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
        assert(idx[i].x < pts.size());
        assert(idx[i].y < pts.size());
        assert(idx[i].z < pts.size());
    }

    return std::pair<std::vector<vec3>, std::vector<uvec3>>(pts, idx);
}