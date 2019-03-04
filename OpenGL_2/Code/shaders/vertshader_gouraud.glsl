#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;

// Specify the Uniforms of the vertex shader
 uniform mat4 modelViewTransform;
 uniform mat4 projectionTransform;
 uniform mat3 normalTransform;
 uniform vec3 lightPos;
 uniform vec4 material;

// Specify the output of the vertex stage
out vec3 vertNormal;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);

//    // Transform the vertex into eye space.
//    vec3 modelViewVertex = vec3(u_MVMatrix * a_Position);

//    // Transform the normal's orientation into eye space.
//    vec3 modelViewNormal = vec3(u_MVMatrix * vec4(a_Normal, 0.0));

//    // Will be used for attenuation.
//    float distance = length(u_LightPos - modelViewVertex);

//    // Get a lighting direction vector from the light to the vertex.
//    vec3 lightVector = normalize(u_LightPos - modelViewVertex);

//    // Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
//    // pointing in the same direction then it will get max illumination.
//    float diffuse = max(dot(modelViewNormal, lightVector), 0.1);

//    // Attenuate the light based on distance.
//    diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));

//    // Multiply the color by the illumination level. It will be interpolated across the triangle.
//    v_Color = a_Color * diffuse;

//    // gl_Position is a special variable used to store the final position.
//    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
//    gl_Position = u_MVPMatrix * a_Position;




    vec3 lightDistance = normalize(lightPos - vertCoordinates_in);
    vec3 viewDir = normalize(-vertCoordinates_in);
    vec3 reflectDir = reflect(-lightDistance, vertNormal_in);
    float specAngle = max(dot(reflectDir, viewDir), 0.05);
    float specular = pow(specAngle, material.w);

    vec3 normalTransVert = normalize(normalTransform * vertNormal_in);
    float diffuse = max(dot(normalTransVert, lightDistance), 0.05);

    vec3 unit = vec3(1, 1, 1);
    vertNormal = material.x + unit * material.y * diffuse + material.z * specular;

//    texCoordinates = texCoordinates_in;
}
