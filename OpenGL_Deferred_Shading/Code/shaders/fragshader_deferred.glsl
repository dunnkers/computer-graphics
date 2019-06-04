#version 330 core

layout (location = 0) out vec3 fColour;
layout (location = 1) out vec3 fNormal;
//layout (location = 2) out vec4 fColour;

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec2 texCoords;
in vec3 position;
in vec3 normal;

// Specify the Uniforms of the fragment shaders
uniform sampler2D textureSampler;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
//out vec4 fColour;
//out vec3 fNormal;

//layout (location = 0) in vec3 fNormal;
//layout (location = 1) in vec3 fColour;

void main()
{
//  fPosition = position;
  fNormal = normalize(normal * 0.5 + 0.5); // map to 0-1 from -1 to 1
  fColour.rgb = texture(textureSampler, texCoords).xyz;
//  fColour.a = 1.0; // @FIXME use specular intensity instead.
}

// learnGL code
//#version 330 core
//layout (location = 0) out vec3 gPosition;
//layout (location = 1) out vec3 gNormal;
//layout (location = 2) out vec4 gAlbedoSpec;

//in vec2 TexCoords;
//in vec3 FragPos;
//in vec3 Normal;

//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;

//void main()
//{
//    // store the fragment position vector in the first gbuffer texture
//    gPosition = FragPos;
//    // also store the per-fragment normals into the gbuffer
//    gNormal = normalize(Normal);
//    // and the diffuse per-fragment color
//    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
//    // store specular intensity in gAlbedoSpec's alpha component
//    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
//}
