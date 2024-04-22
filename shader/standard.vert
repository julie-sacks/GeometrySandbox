#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;
out vec3 normal;
uniform mat4 modelToWorld;
uniform mat4 normalTransform;
uniform mat4 worldToCamera;
uniform mat4 cameraToNdc;

void main()
{
    gl_Position = cameraToNdc * worldToCamera * modelToWorld * vec4(pos,1);
    normal = vec3(normalTransform * vec4(norm, 0));
}