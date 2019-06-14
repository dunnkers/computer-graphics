#version 330
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;

uniform sampler2D textureDiff;
uniform vec3 materialAmbient;
uniform bool hasTextureBool;

out vec4 gBufferOutputs[3];

void main()
{
    vec3 col = vec3(0.0f, 0.0f, 0.0f);
    if (hasTextureBool) col += texture(textureDiff, fragTexCoords).rgb;
    col += materialAmbient;
    gBufferOutputs[0] = vec4(col, 1);
    gBufferOutputs[1] = vec4(fragNormal, 1);
    gBufferOutputs[2] = vec4(fragPosition, 1);
}
