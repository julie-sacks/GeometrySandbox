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
#include "collisions.h"

#include "point.h"
#include "segment.h"
#include "midpoint.h"
#include "line.h"
#include "ray.h"

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

static glm::vec2 torusradii{1,0.5f};
static glm::mat4 testmat{1};
static bool drawdebugshape = false;

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

// fake torus of 0 minor radius, 1 major radius
static std::pair<std::vector<vec3>, std::vector<uvec3>> GenTorus(int steps = 16)
{
    /* start on outer middle (1,0,0)
     * do smaller rings (0.75+0.25*cos(t),0.25*sin(t),0)
     * rotate along torus (x*cos(u),y,z*sin(u))
     *
     * x = cos(u) * (0.75+0.25*cos(t))
     * y = 0.25*sin(t)
     * z = sin(u) + (0.75+0.25*cos(t))
     */
    constexpr float minorRadius = 1.0f;
    constexpr float majorRadius = 1.0f;

    std::vector<vec3> verts;
    for(int i = 0; i < steps*2; ++i)
    {
        float majorangle = i * (2*M_PI)/(steps*2);
        float xmul = cosf(majorangle);
        float zmul = sinf(majorangle);
        for(int j = 0; j < steps; ++j)
        {
            float minorangle = j * (2*M_PI)/(steps);
            float x = xmul * (majorRadius + minorRadius * cosf(minorangle));
            float y = minorRadius * sinf(minorangle);
            float z = zmul * (majorRadius + minorRadius * cosf(minorangle));

            //verts.push_back(vec3(x,y,z)); // pos
            verts.push_back(vec3(xmul, 0, zmul)); // pos
            vec3 norm = vec3(x,y,z) - vec3(xmul*majorRadius, 0, zmul*majorRadius);
            verts.push_back(glm::normalize(norm)); // norm
        }
    }

    std::vector<uvec3> idx;
    for(int i = 0; i < steps*2; ++i)
    {
        for(int j = 0; j < steps; ++j)
        {
            int a = j+i*steps;
            int b = (j+1)%steps+i*steps;
            int c = j+((i+1)%(steps*2))*steps;
            idx.push_back(uvec3(a,b,c));
            a = b;
            b = (j+1)%steps+((i+1)%(steps*2))*steps;
            c = c;
            idx.push_back(uvec3(a,b,c));
        }
    }
    return std::pair<std::vector<vec3>, std::vector<uvec3>>(verts,idx);
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

    if(inputs.GetTriggered(GLFW_KEY_1))
        SpawnPoint();
    if(inputs.GetTriggered(GLFW_KEY_2))
        SpawnSegment();
    if(inputs.GetTriggered(GLFW_KEY_3))
        SpawnMidpoint();
    if(inputs.GetTriggered(GLFW_KEY_4))
        SpawnLine();

    if(inputs.GetTriggered(GLFW_KEY_DELETE))
    {
        for(int id : manager.GetSelected())
        {
            manager.RemoveShape(id);
        }
        lastSelected = -1;
    }

    if(inputs.GetTriggered(GLFW_KEY_H))
    {
        for(int id : manager.GetSelected())
        {
            GenericShape* shape = manager.GetShape(id);
            shape->SetVisibility(!shape->GetVisibility());
        }
    }

    if(inputs.GetTriggered(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void CameraScene::ClickAndDragMove(glm::vec2 dragStart, glm::vec2 dragEnd)
{
    // only drag exactly one object if it was selected with the most recent click
    if(lastSelected == -1 || manager.GetSelectedCount() != 1) return;

    // only drag points and midpoints
    GenericShape* shape = manager.GetShape(lastSelected);
    if(manager.GetShape(lastSelected)->type == ShapeType::Point)
        ClickAndDragPoint(dragStart, dragEnd);
    if(manager.GetShape(lastSelected)->type == ShapeType::Midpoint)
        ClickAndDragMidpoint(dragStart, dragEnd);
}

void CameraScene::ClickAndDragPoint(glm::vec2 dragStart, glm::vec2 dragEnd)
{
    Point* point = dynamic_cast<Point*>(manager.GetShape(lastSelected));
    glm::vec3 cameraToShape = point->GetPos() - cameraPos;

    CollisionRay newDir = ScreenToWorldRay(dragEnd.x, dragEnd.y);
    float distanceFromCam = glm::dot(GetCameraDir(), cameraToShape) / glm::dot(GetCameraDir(), newDir.direction);

    point->SetPos(cameraPos + newDir.direction*distanceFromCam);
}

void CameraScene::ClickAndDragMidpoint(glm::vec2 dragStart, glm::vec2 dragEnd)
{
    Midpoint* midpoint = dynamic_cast<Midpoint*>(manager.GetShape(lastSelected));
    CollisionRay clickray = ScreenToWorldRay(dragEnd.x, dragEnd.y);
    GenericLine* line = dynamic_cast<GenericLine*>(manager.GetShape(midpoint->getParents()[0]));

    glm::vec3 segdir = glm::normalize(line->GetP2Pos() - line->GetP1Pos());
    // t from p1 to camera position
    float lowert = glm::dot(segdir, cameraPos - line->GetP1Pos());
    glm::vec3 camnearestpos = line->GetP1Pos() + segdir*lowert;
    glm::vec3 camtosegdir = glm::normalize(camnearestpos - cameraPos);
    float disttoseg = glm::distance(cameraPos, camnearestpos);
    float camtosegt = disttoseg / dot(camtosegdir, clickray.direction);
    // t from camera position to cursor position
    float uppert = glm::dot(segdir, clickray.direction * camtosegt);

    midpoint->SetT((lowert+uppert)/glm::distance(line->GetP1Pos(), line->GetP2Pos()));

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
        if(!manager.GetShape(id)->IsPointLike()) return;
    }

    manager.AddShape(new Segment(selectedList[0], selectedList[1]));
}
void CameraScene::SpawnMidpoint()
{
    const std::vector<int>& selectedList = manager.GetSelected();
    if(selectedList.size() != 1) return;
    if(!manager.GetShape(selectedList[0])->IsLineLike()) return;
    manager.AddShape(new Midpoint(selectedList[0], 0.5f));
}
void CameraScene::SpawnLine()
{
    const std::vector<int>& selectedList = manager.GetSelected();
    if(selectedList.size() != 2) return;
    for(int id : selectedList)
    {
        if(!manager.GetShape(id)->IsPointLike()) return;
    }

    manager.AddShape(new Line(selectedList[0], selectedList[1]));
}
void CameraScene::SpawnRay()
{
    const std::vector<int>& selectedList = manager.GetSelected();
    if(selectedList.size() != 2) return;
    for(int id : selectedList)
    {
        if(!manager.GetShape(id)->IsPointLike()) return;
    }

    manager.AddShape(new Ray(selectedList[0], selectedList[1]));
}

glm::vec3 CameraScene::GetCameraDir() const
{
    return  glm::rotate(-cameraRot.x, vec3(0,1,0)) *
            glm::rotate(-cameraRot.y, vec3(1,0,0)) *
            glm::vec4(0,0,-1,0);
}

// TODO: for some reason the expected value is nearly exactly a factor of 1.1x away.
CollisionRay CameraScene::ScreenToWorldRay(float x, float y) const
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

    return CollisionRay{cameraPos, glm::normalize(cameraDir)};
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
        if(ImGui::Button("Line    ")) SpawnLine();
        if(ImGui::Button("Ray     ")) SpawnRay();
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

    if(ImGui::Begin("Shapes", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus))
    {
        for (auto& shape : manager.GetShapeList())
        {
            char shapeLabel[64] = {0};
            sprintf(shapeLabel, "%d: %s", shape.first, GetShapeTypeString(shape.second->type));
            if(ImGui::Selectable(shapeLabel, manager.IsSelected(shape.first)))
            {
                if(!manager.IsSelected(shape.first))
                {
                    lastSelected = manager.Select(shape.first);
                }
                else
                {
                    manager.Deselect(shape.first);
                    if(shape.first == lastSelected)
                    {
                        const auto& selected = manager.GetSelected();
                        if(selected.size() > 0) lastSelected = selected[0];
                        else                    lastSelected = -1;
                    }
                }
            }
        }
        
    } ImGui::End();

    if(ImGui::Begin("transform test", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus))
    {
        ImGui::Checkbox("Draw debug shape", &drawdebugshape);
        ImGui::InputFloat2("major/minor", &torusradii[0]);

        static glm::vec4 row0 = testmat[0];
        static glm::vec4 row1 = testmat[1];
        static glm::vec4 row2 = testmat[2];
        static glm::vec4 row3 = testmat[3];
        ImGui::InputFloat4("1", &row0[0]);
        ImGui::InputFloat4("2", &row1[0]);
        ImGui::InputFloat4("3", &row2[0]);
        ImGui::InputFloat4("4", &row3[0]);
        testmat = glm::mat4(row0, row1, row2, row3);
    } ImGui::End();

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


    auto data = GenTorus(16);
    idxcount = data.second.size()*3;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.first.size()*sizeof(glm::vec3), data.first.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.second.size()*sizeof(glm::uvec3), data.second.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void CameraScene::Init()
{
    printf("initializing camera scene\n");
    cameraPos = vec3(0,0,5);
    cameraRot = vec3(0,0,0);
    movementSpeed = 5;
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
    glm::mat4 worldToCam = glm::rotate(cameraRot.y, vec3(1,0,0)) *
                           glm::rotate(cameraRot.x, vec3(0,1,0)) *
                           glm::translate(-cameraPos);
    glm::mat4 persp = glm::perspective((float)(80*(M_PI/180)), ((float)windowWidth)/windowHeight, 0.01f, 100.0f);

    glClearColor(0.1f, 0.3f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //printf("rendering camera scene\n");
    glUseProgram(shaderProgram);

    GLint ulWorldToCamera = glGetUniformLocation(shaderProgram, "worldToCamera");
    GLint ulCameraToNdc = glGetUniformLocation(shaderProgram, "cameraToNdc");
    GLint ulLightDir = glGetUniformLocation(shaderProgram, "lightDir");

    glUniformMatrix4fv(ulWorldToCamera, 1, false, &worldToCam[0][0]);
    glUniformMatrix4fv(ulCameraToNdc, 1, false, &persp[0][0]);
    vec3 lightDir = glm::normalize(vec3(1,2,3));
    glUniform3f(ulLightDir, lightDir.x, lightDir.y, lightDir.z);


    if(drawdebugshape)
    {
        glm::mat4 modelToWorld = glm::transpose(testmat);
        glm::mat4 normalTransform = glm::transpose(glm::inverse(modelToWorld));

        GLint ulModelToWorld = glGetUniformLocation(shaderProgram, "modelToWorld");
        GLint ulNormalTransform = glGetUniformLocation(shaderProgram, "normalTransform");
        GLint ulBaseColor = glGetUniformLocation(shaderProgram, "baseColor");
        GLint ulShapeVisual = glGetUniformLocation(shaderProgram, "shapeVisual");
        GLint ulTorusMinorRadius = glGetUniformLocation(shaderProgram, "torusMinorRadius");
        GLint ulTorusMajorRadius = glGetUniformLocation(shaderProgram, "torusMajorRadius");
        
        glUniformMatrix4fv(ulModelToWorld, 1, false, &modelToWorld[0][0]);
        glUniformMatrix4fv(ulNormalTransform, 1, GL_FALSE, &normalTransform[0][0]);
        glUniform3f(ulBaseColor, 1.f, 0.8f, 0.9f);
        glUniform1i(ulShapeVisual, (int)ShapeVisual::Circle);
        glUniform1f(ulTorusMajorRadius, torusradii[0]);
        glUniform1f(ulTorusMinorRadius, torusradii[1]);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, idxcount, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
        glUniform1i(ulShapeVisual, 0);

    }


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
