#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "testScene.h"
#include "cameraScene.h"

int main(void)
{
    printf("Hello world!\n");

    if(!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Geometry", NULL, NULL);
    if(!window)
    {
        const char* msg;
        int err = glfwGetError(&msg);
        printf("couldn't create window. err %d: %s\n", err, msg);
        return 1;
    }
    
    glViewport(0,0,1280,720);
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        printf("glew issue: %s\n", glewGetErrorString(err));
        return 2;
    }

    printf("gl version: %s\n", glGetString(GL_VERSION));
    Scene* scene = new CameraScene(window);

    scene->Load();
    scene->Init();

    double lastFrameTime = 0;
    while (!glfwWindowShouldClose(window))
    {
        double currentFrameTime = glfwGetTime();
        double dt = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        scene->RunFrame((float)dt);

        //printf("should swap buffers?\n");
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    scene->Shutdown();
    scene->Unload();

    return 0;
}