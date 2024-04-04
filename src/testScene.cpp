#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "testScene.h"
#include <stdio.h>
#include "shader.h"

void TestScene::Load()
{
    printf("loading test scene\n");
    shaderProgram = LoadShaders("./shader/test.vert", "./shader/test.frag");

    float tridata[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glUseProgram(shaderProgram);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), tridata, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}

void TestScene::Init()
{
    printf("initializing test scene\n");
}

void TestScene::PreRender(float dt)
{
    //printf("pre-render test scene\n");
    frameCount++;
}

void TestScene::Render(float dt)
{
    glClearColor(0.1f, 0.3f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    //printf("rendering  test scene\n");

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
}

void TestScene::PostRender(float dt)
{
    //printf("post-render test scene\n");
    //if(frameCount >= 3) glfwSetWindowShouldClose(window, GL_TRUE);
}

void TestScene::Shutdown()
{
    printf("shutting down test scene\n");
}

void TestScene::Unload()
{
    printf("unloading test scene\n");
}
