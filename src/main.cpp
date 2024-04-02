#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}