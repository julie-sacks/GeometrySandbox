#version 330 core

layout(location=0) in vec3 pos;
out vec3 normal;
uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToNdc;

void main()
{
    gl_Position = cameraToNdc * worldToCamera * modelToWorld * vec4(pos,1);
    normal = pos;
}