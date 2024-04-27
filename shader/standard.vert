#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;
out vec3 normal;

uniform int shapeVisual;
uniform mat4 modelToWorld;
uniform mat4 normalTransform;
uniform mat4 worldToCamera;
uniform mat4 cameraToNdc;
uniform float torusMinorRadius;
uniform float torusMajorRadius;

void main()
{
    vec3 nPos = pos;
    if(shapeVisual == 2) // circle
    {
        // scale torus's radii
        vec3 minoradjustment = norm*torusMinorRadius;
        nPos *= torusMajorRadius;
        nPos += minoradjustment;
    }
    gl_Position = cameraToNdc * worldToCamera * modelToWorld * vec4(nPos,1);
    normal = vec3(normalTransform * vec4(norm, 0));
}