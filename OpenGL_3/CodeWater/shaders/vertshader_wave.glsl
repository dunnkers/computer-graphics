#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormals_in;
layout (location = 2) in vec2 texCoords_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

// wave uniform props
uniform int waveAmount;
uniform float waveSpeed;
uniform float amplitude[8];
uniform float phase[8];
uniform float frequency[8];

// Specify the output of the vertex stage
out vec2 uvCoords;
out vec3 vertNormal;
out vec3 vertCoord;

float calcDerivForWave(int wave, float uvcoordval)
{
  return amplitude[wave] * cos((2 * M_PI) * (frequency[wave] * uvcoordval + phase[wave] + waveSpeed)) * frequency[wave] * (2 * M_PI);
}

float calcAmplForWave(int wave, float uvcoordval)
{
 return amplitude[wave] * sin((2 * M_PI) * (frequency[wave] * uvcoordval + phase[wave] + waveSpeed));
}

void main()
{
    uvCoords    = texCoords_in;
    vec3 coords = vertCoordinates_in;

    // compute coord for each wave
    float deriv = 0;
    for (int i = 0; i < waveAmount; i ++)
    {
      deriv += calcDerivForWave(i, uvCoords.x);
      coords.z += calcAmplForWave(i, uvCoords.x);
    }

    // set position and normal
    gl_Position = projectionTransform * modelViewTransform * vec4(coords, 1.0);
    vertCoord = vec3(modelViewTransform * vec4(coords, 1.0));
    vertNormal = vec3(-deriv, 0.0, 1.0);
    vertNormal = normalize(vertNormal);
}
