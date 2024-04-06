#version 330 core

in vec3 normal;
out vec4 fragColor;
uniform vec3 lightDir;

void main()
{
    float brightness = mix(0.3f, 1.0f, pow(max(0,dot(lightDir, normal)), 2));
    fragColor = vec4(brightness, brightness, brightness, 1);
}
