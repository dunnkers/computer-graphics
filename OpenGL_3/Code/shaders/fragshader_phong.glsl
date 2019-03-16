#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec3 vertCoordinates;
in vec2 textureCoordinates;

// The Uniforms of the fragment shaders
uniform vec3 lightPos;
uniform vec4 material;
uniform sampler2D textureColor;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{
    // gouraud calc
    vec3 lightDistance = normalize(lightPos - vertCoordinates);
    vec3 normalizedVertex = normalize(-vertCoordinates);
    vec3 reflected = reflect(-lightDistance, vertNormal);

    float normalLightDotted = dot(vertNormal, lightDistance);
    float diffused = max(normalLightDotted, 0.05);

    vec3 unit = vec3(1, 1, 1);
    float reflectDotNormalized = dot(reflected, normalizedVertex);
    float angle = max(reflectDotNormalized, 0.05);
    float specular = pow(angle, material.w);
    vec3 fNormal3 = material.x + unit * diffused * material.y + material.z * specular;
    fNormal = vec4(fNormal3, 1.0);

    // texture
    vec4 color = texture(textureColor, textureCoordinates);

    // combine previously calculated light intensity with texture color
    fNormal = fNormal * color;
}
