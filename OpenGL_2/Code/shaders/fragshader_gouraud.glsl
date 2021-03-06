#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec2 textureCoordinates;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example
uniform sampler2D textureColor;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{
    fNormal = vec4(vertNormal, 1.0);

    // texture
    vec4 color = texture(textureColor, textureCoordinates);

    // combine previously calculated light intensity with texture color
    fNormal = fNormal * color;
}
