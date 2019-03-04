#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

/* */
vec3 map(vec3 value, int inMin, int inMax, int outMin, int outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

//vec3 map(vec3 value, vec3 inMin, vec3 inMax, vec3 outMin, vec3 outMax) {
//  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
//}

void main()
{
//    vec3 vertNormalMapped = map(vertNormal,
//                     vec3(-1, -1, -1),
//                     vec3(1, 1, 1),
//                     vec3(0, 0, 0),
//                     vec3(1, 1, 1));
    vec3 vertNormalMapped = map(vertNormal, -1, 1, 0, 1);
    fNormal = vec4(vertNormalMapped, 1.0);
}
