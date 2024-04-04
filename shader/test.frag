#version 330 core

in vec3 fragColorFromVert;
out vec4 fragColor;

void main()
{
    fragColor = vec4((gl_FragCoord.xy), 0.5, 1);
    //fragColor = vec4(fragColorFromVert, 1);
    //fragColor = vec4(1,0,0,1);
}
