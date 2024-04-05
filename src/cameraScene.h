#pragma once

#include "scene.h"

class CameraScene : public Scene
{
private:
    GLuint shaderProgram;
    GLuint vbo, vao, ebo;
    int idxcount;
public:
    CameraScene(GLFWwindow* window) : Scene(window) {}
    // steps for running scene
    void Load() override;
    void Init() override;
    void PreRender(float dt) override;
    void Render(float dt) override;
    void PostRender(float dt) override;
    void Shutdown() override;
    void Unload() override;
};
