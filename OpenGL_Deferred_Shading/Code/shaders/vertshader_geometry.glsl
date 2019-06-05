#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;

out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;

uniform mat4 uVp;
void main()
{
    fsPos = vsPos;
    fsNormal = vsNormal;
    fsUv = vsUv;
    gl_Position = uVp * vec4(vsPos, 1.0);
}