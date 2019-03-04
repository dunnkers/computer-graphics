#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec3 vertCoordinates;

// The Uniforms of the fragment shaders
uniform vec3 lightPos;
uniform vec4 material;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

/* Simple range mapping function */
vec3 map(vec3 value, int inMin, int inMax, int outMin, int outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

void main()
{    
    // gouraud calc
    vec3 lightDistance = normalize(lightPos - vertCoordinates);
    vec3 normalizedVertex = normalize(-vertCoordinates);
    vec3 reflected = reflect(-lightDistance, vertNormal);
    float angle = max(dot(reflected, normalizedVertex), 0.05);
    float specular = pow(angle, material.w);

    float diffuse = max(dot(vertNormal, lightDistance), 0.05);

    vec3 unit = vec3(1, 1, 1);
    vec3 fNormal3 = material.x + unit * material.y * diffuse + material.z * specular;
    fNormal = vec4(fNormal3, 1.0);

}
