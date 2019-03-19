#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec2 uvCoords;
in vec3 vertNormal;
in vec3 vertCoord;

// Specify the Uniforms of the fragment shaders
// Lighting model constants.
uniform vec4 material;
uniform vec3 lightColour;
uniform vec3 lightPosition;

// wave color
uniform vec3 waveColor;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    // Ambient colour does not depend on any vectors.
    vec3 ambientIntensity    = material.x * waveColor;

    // Calculate light direction vectors in the phong model.
    vec3 lightDirection   = normalize(lightPosition - vertCoord);
    vec3 normal           = normalize(vertNormal);

    // Diffuse colour.
    vec3 diffuseIntensity = waveColor * material.y * lightColour * max(dot(vertNormal, lightDirection), 0);

    // Specular colour.
    vec3 viewDirection     = normalize(-vertCoord); // The camera is always at (0, 0, 0).
    vec3 reflectDirection  = reflect(-lightDirection, vertNormal);
    vec3 specularIntesity = lightColour * material.z * waveColor *  pow(max(dot(reflectDirection, viewDirection), 0), material.w);

    // apply smoothstep etc.
    float smoothstepped = smoothstep(-1.0f, 1.0f, vertCoord.y);
    float mixed = mix(1.0f, 0.5f, smoothstepped);
    vec4 heightColor = vec4(0.0f, 1.0f, mixed, 1.0f);

    // final color intensity.
    vec3 I = ambientIntensity + diffuseIntensity + specularIntesity;
    fColor = heightColor * vec4(I, 1.0);
}
