#version 330

layout(location = 0) in vec3 vertPosition;

out vec4 fragPosition;

uniform mat4 vpTransform;

uniform float uLightRadius;
uniform vec3 uLightPosition;

void main()
{
  vec4 position = vpTransform * vec4((vertPosition * uLightRadius) + uLightPosition, 1.0);

  gl_Position = position;
  fragPosition = position;
}
