#version 330 core

layout(location=0) in vec3 pos;
out vec3 fragColorFromVert;

vec3 points[3] = vec3[](
    vec3(-0.5, -0.5, 0),
    vec3(0.5, -0.5, 0),
    vec3(0, -0.5, 0));

void main()
{
    gl_Position = vec4(pos, 1);
    fragColorFromVert = pos;
    //gl_Position.w = 1;
}
