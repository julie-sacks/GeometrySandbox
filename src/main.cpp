#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "testScene.h"

int main(void)
{
    printf("Hello world!\n");

    if(!glfwInit()) return 1;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Geometry", NULL, NULL);
    if(!window)
    {
        printf("couldn't create window\n");
        return 1;
    }

    Scene* scene = new TestScene(window);

    scene->Load();
    scene->Init();

    double lastFrameTime = 0;
    while (!glfwWindowShouldClose(window))
    {
        double currentFrameTime = glfwGetTime();
        double dt = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        scene->RunFrame((float)dt);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    scene->Shutdown();
    scene->Unload();

    return 0;
}