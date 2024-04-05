#pragma once

#include "scene.h"
#include <bitset>
#include <glm/vec3.hpp>

struct InputState
{
    std::bitset<GLFW_KEY_LAST+1> currState;
    std::bitset<GLFW_KEY_LAST+1> prevState;
public:
    // key transitioned to down on this frame
    bool GetTriggered(int glfw_key_code) const;
    // key transitioned to up on this frame
    bool GetReleased(int glfw_key_code) const;
    // key is down on this frame
    bool GetDown(int glfw_key_code) const;
    // key is up on this frame (this is just !GetDown())
    //bool GetUp(int glfw_key_code) const;
};

class CameraScene : public Scene
{
private:
    GLuint shaderProgram;
    GLuint vbo, vao, ebo;
    int idxcount;
    InputState inputs;
    void PollInputs();
    void HandleInputs(float dt);

    // camera variables
    glm::vec3 cameraPos;
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
