#version 330

layout(location = 0) in vec3 vsPos;

out vec4 fsPos;

uniform mat4 uVp;
uniform float uLightRadius;
uniform vec3 uLightPosition;

void main()
{
  vec4 pos = uVp * vec4((vsPos * uLightRadius) + uLightPosition, 1.0);

  gl_Position = pos;
  fsPos = pos;
}