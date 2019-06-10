#version 330

layout(location = 0) in vec3 vertPosition;

out vec4 fragPosition;

uniform mat4 vpTransform;

uniform float lightRad;
uniform vec3 lightPos;

void main()
{
    vec4 position = vpTransform * vec4(vertPosition, 1.0);
    gl_Position = position;
    fragPosition = position;
}
