#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
//in float ambient, diffuse, specular;
in vec2 texCoords;
in vec3 normal;

// Specify the Uniforms of the fragment shaders
uniform sampler2D textureSampler;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec3 fColour;
out vec3 fNormal;

void main()
{
  fColour = texture(textureSampler, texCoords).xyz;
  fNormal = normal;
}
