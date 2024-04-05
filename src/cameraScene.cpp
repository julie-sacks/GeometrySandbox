#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "cameraScene.h"
#include <stdio.h>
#include "shader.h"
// #include <glm/vec3.hpp>
#include <vector>
#include <cmath>
#include <cassert>

struct vec3 
{
    float x,y,z;
    vec3(float x, float y, float z):x(x),y(y),z(z) {}
};
struct uivec3
{
    unsigned int x,y,z;
    uivec3(unsigned int x, unsigned int y, unsigned int z):x(x),y(y),z(z) {}
};

// using glm::vec3;
// using glm::ivec3;

// generate a list of vertices in rings from bottom to top and its corresponding indices
static std::pair<std::vector<vec3>, std::vector<uivec3>> GenSphere(int steps = 5)
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
            float x = r*cosf(horizangle);
            float z = r*sinf(horizangle);

            pts.push_back(vec3(x,y,z));
        }
    }

    pts.push_back(vec3(0,1,0));

    // incides
    std::vector<uivec3> idx;
    // bottom disk (0,1,2,  0,2,3,  0,3,4...  0,10,1)
    for(int i = 0; i < steps*2; ++i)
    {
        idx.push_back(uivec3(0,i+1,(i+1)%(steps*2)+1));
    }
    // rings (1,2,11,  2,12,11,  2,3,12,  3,13,12)
    for(int i = 0; i < steps-2; ++i)
    {
        for(int j = 0; j < steps*2; ++j)
        {
            idx.push_back(uivec3( i   *(steps*2) +  j+1,
                                  i   *(steps*2) + (j+1)%(steps*2)+1,
                                 (i+1)*(steps*2) +  j+1));
            idx.push_back(uivec3( i   *(steps*2) + (j+1)%(steps*2)+1,
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
        idx.push_back(uivec3(last,middle+i+1,middle+(i+2)%steps));
    }

    // sanity check
    for(int i = 0; i < idx.size(); ++i)
    {
        assert(idx[i].x < pts.size());
        assert(idx[i].y < pts.size());
        assert(idx[i].z < pts.size());
    }

    return std::pair<std::vector<vec3>, std::vector<uivec3>>(pts, idx);
}

void CameraScene::Load()
{
    shaderProgram = LoadShaders("./shader/test.vert", "./shader/test.frag");

    glUseProgram(shaderProgram);

    auto sphere = GenSphere(16);
    idxcount = sphere.second.size()*3;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.first.size()*sizeof(vec3), sphere.first.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.second.size()*sizeof(uivec3), sphere.second.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}

void CameraScene::Init()
{
    printf("initializing test scene\n");
}

void CameraScene::PreRender(float dt)
{
    //printf("pre-render test scene\n");
    //frameCount++;
}

void CameraScene::Render(float dt)
{
    glClearColor(0.1f, 0.3f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    //printf("rendering  test scene\n");

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, idxcount, GL_UNSIGNED_INT, nullptr);
    glUseProgram(0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
}

void CameraScene::PostRender(float dt)
{
    //printf("post-render test scene\n");
    //if(frameCount >= 3) glfwSetWindowShouldClose(window, GL_TRUE);
}

void CameraScene::Shutdown()
{
    printf("shutting down test scene\n");
}

void CameraScene::Unload()
{
    printf("unloading test scene\n");
}
