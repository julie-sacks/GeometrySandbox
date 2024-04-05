#version 330 core

out vec4 fragColor;
uniform vec3 offset;

void main()
{
    //fragColor = vec4((fragColorFromVert.xy+1)*0.5, 0.5, 1);
    //fragColor = vec4(fragColorFromVert, 1);
    vec3 newoffset = (offset.xzy)/2;
    vec3 color = (gl_FragCoord.xyz)/vec3(1280,720,1) - newoffset;
    fragColor = vec4(color,1);
    fragColor.z = 1-fragColor.z;
}
