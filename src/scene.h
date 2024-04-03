#pragma once

#include <GLFW/glfw3.h>

class Scene
{
private:

protected:
    GLFWwindow* window;
public:
    Scene(GLFWwindow* window_) : window(window_) {}
    // steps for running scene
    virtual void Load();
    virtual void Init();
    virtual void PreRender(float dt);
    virtual void Render(float dt);
    virtual void PostRender(float dt);
    virtual void Shutdown();
    virtual void Unload();

    // wrapping methods. i don't think these need to be virtual
    void RunFrame(float dt); // Pre-Render -> Render -> PostRender
    void Reload(); // Shutdown -> Unload -> Load -> Init
    void Restart(); // Shutdown -> Init
};