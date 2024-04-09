#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "testScene.h"
#include "cameraScene.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}