#pragma once

#include "scene.h"

class TestScene : public Scene
{
private:
    int frameCount = 0;
public:
    TestScene(GLFWwindow* window) : Scene(window) {}
    // steps for running scene
    void Load() override;
    void Init() override;
    void PreRender(float dt) override;
    void Render(float dt) override;
    void PostRender(float dt) override;
    void Shutdown() override;
    void Unload() override;
};
