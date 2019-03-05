#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoordinates_in;

// Specify the Uniforms of the vertex shader
 uniform mat4 modelViewTransform;
 uniform mat4 projectionTransform;
 uniform mat3 normalTransform;
 uniform vec3 lightPos;
 uniform vec4 material;

// Specify the output of the vertex stage
out vec3 vertNormal;
out vec2 textureCoordinates;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);

    // gouraud calc
    vec3 lightDistance = normalize(lightPos - vertCoordinates_in);
    vec3 normalizedVertex = normalize(-vertCoordinates_in);
    vec3 reflected = reflect(-lightDistance, vertNormal_in);
    float angle = max(dot(reflected, normalizedVertex), 0.05);
    float specular = pow(angle, material.w);

    vec3 normalTransVert = normalize(normalTransform * vertNormal_in);
    float diffuse = max(dot(normalTransVert, lightDistance), 0.05);

    vec3 unit = vec3(1, 1, 1);
    vertNormal = material.x + unit * material.y * diffuse + material.z * specular;

    // texture
    textureCoordinates = textureCoordinates_in;
}
