#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "cameraScene.h"
#include <stdio.h>
#include "shader.h"
#include <vector>
#include <cmath>
#include <cassert>
#include "portable-file-dialogs.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "point.h"
#include "segment.h"
#include "midpoint.h"

#define GLM_SWIZZLE
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/detail/type_mat4x4.hpp>

// struct vec3 
// {
//     float x,y,z;
//     vec3(float x, float y, float z):x(x),y(y),z(z) {}
// };
// struct uivec3
// {
//     unsigned int x,y,z;
//     uivec3(unsigned int x, unsigned int y, unsigned int z):x(x),y(y),z(z) {}
// };

using glm::vec3;
using glm::uvec3;

// generate a list of vertices in rings from bottom to top and its corresponding indices
static std::pair<std::vector<vec3>, std::vector<uvec3>> GenSphere(int steps = 5)
{
    // vertices
    std::vector<vec3> pts;
    pts.push_back(vec3(0,-1,0));

    // for each ring
    for(int i = 1; i < steps; ++i)
    {
        float vertangle = M_PI * ((float)i / steps);
        float y = -cosf(vertangle);
        float r = sinf(vertangle);
        // for each point in the ring
        for(int j = 0; j < steps*2; ++j)
        {
            float horizangle = M_PI * ((float)j/steps);
            // swap these if tris are inside out
            float x = r*sinf(horizangle);
            float z = r*cosf(horizangle);

            pts.push_back(vec3(x,y,z));
        }
    }

    pts.push_back(vec3(0,1,0));

    // incides
    std::vector<uvec3> idx;
    // bottom disk (0,1,2,  0,2,3,  0,3,4...  0,10,1)
    for(int i = 0; i < steps*2; ++i)
    {
        idx.push_back(uvec3(0,(i+1)%(steps*2)+1,i+1));
    }
    // rings (1,2,11,  2,12,11,  2,3,12,  3,13,12)
    for(int i = 0; i < steps-2; ++i)
    {
        for(int j = 0; j < steps*2; ++j)
        {
            idx.push_back(uvec3( i   *(steps*2) +  j+1,
                                 i   *(steps*2) + (j+1)%(steps*2)+1,
                                (i+1)*(steps*2) +  j+1));
            idx.push_back(uvec3( i   *(steps*2) + (j+1)%(steps*2)+1,
                                (i+1)*(steps*2) + (j+1)%(steps*2)+1,
                                (i+1)*(steps*2) +  j+1));
        }
    }
    // top disk (11,1,2,  11,2,3...  11,10,1)
    for(int i = 0; i < steps*2; ++i)
    {
        //(steps*steps*2)
        int last   = (steps-1)*steps*2 + 1;
        int middle = (steps-2)*steps*2;
        idx.push_back(uvec3(last,middle+i+1,middle+(i+1)%(steps*2)+1));
    }

    // sanity check
    for(int i = 0; i < idx.size(); ++i)
    {
        assert(idx[i].x < pts.size());
        assert(idx[i].y < pts.size());
        assert(idx[i].z < pts.size());
    }

    return std::pair<std::vector<vec3>, std::vector<uvec3>>(pts, idx);
}

void CameraScene::PollInputs()
{
    inputs.prevKeys = inputs.currKeys;
    inputs.prevMouseBtn = inputs.currMouseBtn;
    inputs.prevCursorX = inputs.currCursorX;
    inputs.prevCursorY = inputs.currCursorY;

    inputs.currKeys = false;
    if(!ImGui::GetIO().WantCaptureKeyboard)
    {
        // go through each key code (including invalid ones for simplicity)
        for(int i = 0; i <= GLFW_KEY_LAST; ++i)
        {
            bool state = glfwGetKey(window, i);
            if(state == GLFW_INVALID_ENUM) continue;
            inputs.currKeys[i] = (state == GLFW_PRESS);
        }
    }

    glfwGetCursorPos(window, &inputs.currCursorX, &inputs.currCursorY);

    inputs.currMouseBtn = false;
    if(!ImGui::GetIO().WantCaptureMouse)
    {
        for(int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
        {
            bool state = glfwGetMouseButton(window, i);
            if(state == GLFW_INVALID_ENUM) continue;
            inputs.currMouseBtn[i] = (state == GLFW_PRESS);
        }
    }
}

void CameraScene::HandleInputs(float dt)
{
    // commands
    if(inputs.GetDown(GLFW_KEY_LEFT_CONTROL) || inputs.GetDown(GLFW_KEY_RIGHT_CONTROL))
    {
        if(inputs.GetTriggered(GLFW_KEY_R))
            shaderProgram = LoadShaders("./shader/standard.vert", "./shader/standard.frag");

        if(inputs.GetTriggered(GLFW_KEY_N))
        {
            ClearShapes();
        }

        if(inputs.GetTriggered(GLFW_KEY_O))
        {
            OpenFileDialog();
        }

        if(inputs.GetTriggered(GLFW_KEY_S))
        {
            SaveFileAuto();
        }
        return;
    }

    // handle non-command inputs
    glm::mat4 cameraTrans = glm::rotate(-cameraRot.x, vec3(0,1,0)) *
                            glm::rotate(-cameraRot.y, vec3(1,0,0));

    if(inputs.GetDown(GLFW_KEY_W))
        cameraPos += movementSpeed*dt * vec3(cameraTrans*glm::vec4(0,0,-1,0));
    if(inputs.GetDown(GLFW_KEY_A))
        cameraPos += movementSpeed*dt * vec3(cameraTrans*glm::vec4(-1,0,0,0));
    if(inputs.GetDown(GLFW_KEY_S))
        cameraPos += movementSpeed*dt * vec3(cameraTrans*glm::vec4(0,0,1,0));
    if(inputs.GetDown(GLFW_KEY_D))
        cameraPos += movementSpeed*dt * vec3(cameraTrans*glm::vec4(1,0,0,0));
    if(inputs.GetDown(GLFW_KEY_E))
        cameraPos += movementSpeed*dt * vec3(cameraTrans*glm::vec4(0,1,0,0));
    if(inputs.GetDown(GLFW_KEY_Q))
        cameraPos += movementSpeed*dt * vec3(cameraTrans*glm::vec4(0,-1,0,0));

    if(inputs.GetDown(GLFW_KEY_UP))
        cameraRot.y += 1.0f*dt;
    if(inputs.GetDown(GLFW_KEY_DOWN))
        cameraRot.y -= 1.0f*dt;
    if(inputs.GetDown(GLFW_KEY_LEFT))
        cameraRot.x -= 1.0f*dt;
    if(inputs.GetDown(GLFW_KEY_RIGHT))
        cameraRot.x += 1.0f*dt;

    if(inputs.GetTriggered(GLFW_KEY_EQUAL))
        movementSpeed += 0.25f;
    if(inputs.GetTriggered(GLFW_KEY_MINUS))
        movementSpeed -= 0.25f;

    static glm::vec2 dragStart; // keeps track of the original mouse position
    static bool preDrag; // only start dragging after a bit of mouse movement
    if(inputs.GetMouseTriggered(GLFW_MOUSE_BUTTON_LEFT))
    {
        bool multiselect = inputs.GetDown(GLFW_KEY_LEFT_SHIFT) || inputs.GetDown(GLFW_KEY_RIGHT_SHIFT);
        lastSelected = manager.SelectRaycast(ScreenToWorldRay(inputs.currCursorX, inputs.currCursorY), multiselect);
        dragStart = glm::vec2(inputs.currCursorX, inputs.currCursorY);
        preDrag = true;
    }
    if(inputs.GetMouseDown(GLFW_MOUSE_BUTTON_LEFT))
    {
        glm::vec2 dragEnd(inputs.currCursorX, inputs.currCursorY);
        // only start moving if the cursor has moved a certain amount
        glm::vec2 dragDelta = dragEnd - dragStart;
        if(glm::dot(dragDelta, dragDelta) > 10) preDrag = false;
        // don't run if the cursor hasn't moved this frame
        if(inputs.GetMouseDelta() != glm::zero<glm::vec2>() && !preDrag)
        {
            ClickAndDragMove(dragStart, dragEnd);
        }
    }

    if(inputs.GetMouseDown(GLFW_MOUSE_BUTTON_RIGHT))
    {
        cameraRot += vec3(inputs.GetMouseDelta(), 0)*0.01f;
    }
    if(inputs.GetMouseTriggered(GLFW_MOUSE_BUTTON_MIDDLE) || inputs.GetTriggered(GLFW_KEY_ENTER))
    {
        SpawnPoint();
    }

    if(inputs.GetTriggered(GLFW_KEY_1))
        SpawnPoint();
    if(inputs.GetTriggered(GLFW_KEY_2))
        SpawnSegment();
    if(inputs.GetTriggered(GLFW_KEY_3))
        SpawnMidpoint();


    if(inputs.GetTriggered(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void CameraScene::ClickAndDragMove(glm::vec2 dragStart, glm::vec2 dragEnd)
{
    // only drag exactly one object if it was selected with the most recent click
    if(lastSelected == -1 || manager.GetSelectedCount() != 1) return;

    // only drag points
    GenericShape* shape = manager.GetShape(lastSelected);
    if(manager.GetShape(lastSelected)->type != ShapeType::Point) return;

    Point* point = dynamic_cast<Point*>(shape);
    glm::vec3 cameraToShape = point->GetPos() - cameraPos;

    Ray newDir = ScreenToWorldRay(dragEnd.x, dragEnd.y);
    float distanceFromCam = glm::dot(GetCameraDir(), cameraToShape) / glm::dot(GetCameraDir(), newDir.direction);

    point->SetPos(cameraPos + newDir.direction*distanceFromCam);
}

void CameraScene::SpawnPoint()
{
    manager.AddShape(new Point(cameraPos + 5.0f*GetCameraDir()));
}
void CameraScene::SpawnSegment()
{
    const std::vector<int>& selectedList = manager.GetSelected();
    if(selectedList.size() != 2) return;
    for(int id : selectedList)
    {
        if(manager.GetShape(id)->type != ShapeType::Point) return;
    }

    manager.AddShape(new Segment(selectedList[0], selectedList[1]));
}

void CameraScene::SpawnMidpoint()
{
    const std::vector<int>& selectedList = manager.GetSelected();
    if(selectedList.size() != 1) return;
    if(manager.GetShape(selectedList[0])->type != ShapeType::Segment) return;
    manager.AddShape(new Midpoint(selectedList[0], 0.5f));
}

glm::vec3 CameraScene::GetCameraDir() const
{
    return  glm::rotate(-cameraRot.x, vec3(0,1,0)) *
            glm::rotate(-cameraRot.y, vec3(1,0,0)) *
            glm::vec4(0,0,-1,0);
}

// TODO: for some reason the expected value is nearly exactly a factor of 1.1x away.
Ray CameraScene::ScreenToWorldRay(float x, float y) const
{
    // take the center position, then use a screen plane at a known distance from the camera
    // and add the offset on the screen plane to the cameraDir (scaled to unit distance)
    float ducttapeadjustment = 1.1f;
    x = (x/1280.0f) - 0.5f;
    y = (y/ 720.0f) - 0.5f;
    float vpheight = cosf(fov/2) * 2;
    float vpwidth = vpheight * (16.0f/9.0f);
    glm::vec4 offset(x * vpwidth, -y * vpheight, 0, 0);

    glm::vec3 cameraDir = glm::rotate(-cameraRot.x, vec3(0,1,0)) *
                          glm::rotate(-cameraRot.y, vec3(1,0,0)) *
                          (glm::vec4(0,0,-1,0) + offset * ducttapeadjustment);

    return Ray{cameraPos, glm::normalize(cameraDir)};
}

bool CameraScene::OpenFileDialog()
{
    auto selection = pfd::open_file("Open file","",
        std::vector<std::string>{"JSON Files", "*.json *.jsonc", "All Files", "*"}).result();
    if(selection.empty()) return false;
    
    manager.LoadFromFile(selection[0].c_str());
    currentFilePath = selection[0];
    return true;
}

bool CameraScene::SaveFileToPath(const std::string& path)
{
    currentFilePath = path;
    return manager.SaveToFile(path.c_str());
}

bool CameraScene::SaveFileDialog()
{
    auto selection = pfd::save_file("Save file to...","",
        std::vector<std::string>{"JSON Files", "*.json *.jsonc", "All Files", "*"}).result();
    if(selection.empty()) return false;

    return SaveFileToPath(selection);
}

bool CameraScene::SaveFileAuto()
{
    bool result = false;
    if(!currentFilePath.empty())
        result = SaveFileToPath(currentFilePath);
    if(result) return true;
    
    // if either no file is loaded or it failed to save to the existing path
    return SaveFileDialog();
}

void CameraScene::ClearShapes()
{
    manager.ClearShapes();
    lastSelected = -1;
}

void CameraScene::GuiRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool isDemoVisible = false;
    if(inputs.GetTriggered(GLFW_KEY_F1)) isDemoVisible = !isDemoVisible;
    if(isDemoVisible) ImGui::ShowDemoWindow();

    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) {ClearShapes();}
            if (ImGui::MenuItem("Open", "Ctrl+O")) {OpenFileDialog();}
            if (ImGui::MenuItem("Save", "Ctrl+S")) {SaveFileAuto();}
            if (ImGui::MenuItem("Save As...")) {SaveFileDialog();}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if(ImGui::Begin("Tools", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus))
    {
        if(ImGui::Button("Point   ")) SpawnPoint();
        if(ImGui::Button("Segment ")) SpawnSegment();
        if(ImGui::Button("Midpoint")) SpawnMidpoint();
    } ImGui::End();

    char title[64] = "No selection###";
    GenericShape* shape = nullptr;
    if(lastSelected != -1)
    {
        shape = manager.GetShape(lastSelected);
        sprintf(title, "%s %d###", GetShapeTypeString(shape->type), lastSelected);
    }

    ImGui::PushID("SelectionBox");
    if(ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus)
        && shape)
    {
        switch (shape->type)
        {
        case ShapeType::Point:
        {
            Point* point = dynamic_cast<Point*>(shape);
            glm::vec3 pos = point->GetPos();
            ImGui::InputFloat3("Pos: ", &pos[0]);
            point->SetPos(pos);
        }   break;

        case ShapeType::Segment:
            break;

        case ShapeType::Midpoint:
        {
            Midpoint* midpoint = dynamic_cast<Midpoint*>(shape);
            float t = midpoint->GetT();
            ImGui::InputFloat("Pos: ", &t);
            midpoint->SetT(t);
        }   break;

        default:
            break;
        }
    } ImGui::End();
    ImGui::PopID();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void CameraScene::Load()
{
    shaderProgram = LoadShaders("./shader/standard.vert", "./shader/standard.frag");

    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}

void CameraScene::Init()
{
    printf("initializing camera scene\n");
    cameraPos = vec3(0,0,5);
    cameraRot = vec3(0,0,0);
    movementSpeed = 2;
    fov = 80.0f*(M_PI/180.0f);
    lastSelected = -1;
}

void CameraScene::PreRender(float dt)
{
    PollInputs();
    HandleInputs(dt);
}

void CameraScene::Render(float dt)
{
    glm::mat4 modelToWorld(1);
    glm::mat4 worldToCam = glm::rotate(cameraRot.y, vec3(1,0,0)) *
                           glm::rotate(cameraRot.x, vec3(0,1,0)) *
                           glm::translate(-cameraPos);
    glm::mat4 persp = glm::perspective((float)(80*(M_PI/180)), 1280.0f/720, 0.01f, 100.0f);

    glClearColor(0.1f, 0.3f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //printf("rendering camera scene\n");
    glUseProgram(shaderProgram);

    GLint ulModelToWorld = glGetUniformLocation(shaderProgram, "modelToWorld");
    GLint ulWorldToCamera = glGetUniformLocation(shaderProgram, "worldToCamera");
    GLint ulCameraToNdc = glGetUniformLocation(shaderProgram, "cameraToNdc");
    GLint ulLightDir = glGetUniformLocation(shaderProgram, "lightDir");

    glUniformMatrix4fv(ulModelToWorld, 1, false, &modelToWorld[0][0]);
    glUniformMatrix4fv(ulWorldToCamera, 1, false, &worldToCam[0][0]);
    glUniformMatrix4fv(ulCameraToNdc, 1, false, &persp[0][0]);
    vec3 lightDir = glm::normalize(vec3(1,2,3));
    glUniform3f(ulLightDir, lightDir.x, lightDir.y, lightDir.z);

    manager.Draw(shaderProgram);
    glBindVertexArray(0);
    glUseProgram(0);

    GuiRender();
}

void CameraScene::PostRender(float dt)
{
    //printf("post-render camera scene\n");
    //if(frameCount >= 3) glfwSetWindowShouldClose(window, GL_TRUE);
}

void CameraScene::Shutdown()
{
    printf("shutting down camera scene\n");
}

void CameraScene::Unload()
{
    printf("unloading camera scene\n");
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

bool InputState::GetTriggered(int glfw_key_code) const
{
    return currKeys[glfw_key_code] && !prevKeys[glfw_key_code];
}

bool InputState::GetReleased(int glfw_key_code) const
{
    return !currKeys[glfw_key_code] && prevKeys[glfw_key_code];
}

bool InputState::GetDown(int glfw_key_code) const
{
    return currKeys[glfw_key_code];
}

bool InputState::GetMouseTriggered(int glfw_mouse_btn) const
{
    return currMouseBtn[glfw_mouse_btn] && !prevMouseBtn[glfw_mouse_btn];
}

bool InputState::GetMouseReleased(int glfw_mouse_btn) const
{
    return !currMouseBtn[glfw_mouse_btn] && prevMouseBtn[glfw_mouse_btn];
}

bool InputState::GetMouseDown(int glfw_mouse_btn) const
{
    return currMouseBtn[glfw_mouse_btn];
}

glm::vec2 InputState::GetMousePos() const
{
    return glm::vec2(currCursorX, currCursorY);
}

glm::vec2 InputState::GetMouseDelta() const
{
    return glm::vec2(currCursorX-prevCursorX, currCursorY-prevCursorY);
}
