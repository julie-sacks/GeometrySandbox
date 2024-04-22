#version 330 core

in vec3 normal;
out vec4 fragColor;
uniform vec3 lightDir;
uniform vec3 baseColor;

void main()
{
    float brightness = mix(0.3f, 1.0f, pow(max(0,dot(lightDir, normalize(normal))), 2));
    fragColor = vec4(baseColor*brightness, 1);
}
