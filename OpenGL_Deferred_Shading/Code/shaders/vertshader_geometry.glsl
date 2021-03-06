#version 330
layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoords;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;

uniform mat4 mvpTransform;
uniform mat3 normalTransform;

void main()
{
    fragPosition = (mvpTransform * vec4(vertPosition, 1.0)).xyz;
    fragNormal = normalTransform * vertNormal;
    fragTexCoords = vertTexCoords;
    gl_Position = mvpTransform * vec4(vertPosition, 1.0);
}
