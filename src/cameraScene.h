#pragma once

#include "scene.h"
#include <bitset>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "shapeManager.h"

struct InputState
{
    std::bitset<GLFW_KEY_LAST+1> currKeys;
    std::bitset<GLFW_KEY_LAST+1> prevKeys;
    std::bitset<GLFW_MOUSE_BUTTON_LAST+1> currMouseBtn;
    std::bitset<GLFW_MOUSE_BUTTON_LAST+1> prevMouseBtn;
    double currCursorX, currCursorY;
    double prevCursorX, prevCursorY;
public:
    // key transitioned to down on this frame
    bool GetTriggered(int glfw_key_code) const;
    // key transitioned to up on this frame
    bool GetReleased(int glfw_key_code) const;
    // key is down on this frame
    bool GetDown(int glfw_key_code) const;
    // key is up on this frame (this is just !GetDown())
    //bool GetUp(int glfw_key_code) const;
    bool GetMouseTriggered(int glfw_mouse_btn) const;
    bool GetMouseReleased(int glfw_mouse_btn) const;
    bool GetMouseDown(int glfw_mouse_btn) const;

    glm::vec2 GetMousePos() const;
    glm::vec2 GetMouseDelta() const;
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
    void ClickAndDragMove(glm::vec2 dragStart, glm::vec2 dragEnd);
    void ClickAndDragPoint(glm::vec2 dragStart, glm::vec2 dragEnd);
    void ClickAndDragMidpoint(glm::vec2 dragStart, glm::vec2 dragEnd);

    void SpawnPoint();
    void SpawnSegment();
    void SpawnMidpoint();
    void SpawnLine();

    // camera variables
    glm::vec3 cameraPos;
    glm::vec3 cameraRot;
    float movementSpeed;
    float fov;
    glm::vec3 GetCameraDir() const;
    Ray ScreenToWorldRay(float x, float y) const;

    ShapeManager manager;
    int lastSelected;

    // file io helpers
    std::string currentFilePath;
    bool OpenFileDialog();
    bool SaveFileToPath(const std::string& path);
    bool SaveFileDialog();
    bool SaveFileAuto();
    void ClearShapes();

    // imgui
    void GuiRender();
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
