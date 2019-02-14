#version 330 core

out vec4 fragColor;

in vec3 interpolatedColor;

void main()
{
    fragColor = vec4(interpolatedColor,1);
}
