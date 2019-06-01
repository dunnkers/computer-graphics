#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormals_in;
layout (location = 2) in vec2 texCoords_in;

// Transformation matrices.
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

// Lighting model constants.
//uniform vec4 material;
//uniform vec3 lightPosition;

// Specify the output of the vertex stage
//out float ambient, diffuse, specular;
out vec2 texCoords;
out vec3 position;
out vec3 normal;

//const vec2 madd = vec2(0.5,0.5);
//textureCoord = vertexIn.xy*madd+madd; // scale vertex attribute to [0-1] range

void main()
{
//    // Ambient component.
//    ambient = material.x;

//    // Calculate light direction, vertex position and normal.
//    vec3 vertexPosition        = vec3(modelViewTransform * vec4(vertCoordinates_in, 1));
//    vec3 vertexNormal          = normalize(normalTransform * vertNormals_in);
//    vec3 relativeLightPosition = vec3(modelViewTransform * vec4(lightPosition, 1));
//    vec3 lightDirection        = normalize(relativeLightPosition - vertexPosition);

//    // Diffuse component.
//    float diffuseIntensity = max(dot(vertexNormal, lightDirection), 0);
//    diffuse = material.y * diffuseIntensity;

//    // Specular component.
//    vec3 viewDirection      = normalize(-vertexPosition); // The camera is always at (0, 0, 0).
//    vec3 reflectDirection   = reflect(-lightDirection, vertexNormal);
//    float specularIntensity = max(dot(viewDirection, reflectDirection), 0);
//    specular = material.z * pow(specularIntensity, material.w);

    texCoords = texCoords_in;
    position = vertCoordinates_in; // should be transformed by model...?
    normal = vertNormals_in;
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1);
}

// learnGL code
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

//out vec3 FragPos;
//out vec2 TexCoords;
//out vec3 Normal;

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

//void main()
//{
//    vec4 worldPos = model * vec4(aPos, 1.0);
//    FragPos = worldPos.xyz;
//    TexCoords = aTexCoords;

//    mat3 normalMatrix = transpose(inverse(mat3(model)));
//    Normal = normalMatrix * aNormal;

//    gl_Position = projection * view * worldPos;
//}
