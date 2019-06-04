#version 330 core

// Define constants
#define M_PI 3.141593

// The input from the vertex shader.
//in vec3 vertNormal;
//in vec3 vertPosition;
//in vec3 relativeLightPosition;
in vec2 texCoords;

// Lighting model constants.
uniform vec4 material;
uniform vec3 lightColour;

// Texture sampler
//uniform sampler2D textureSampler;

uniform sampler2D fPosition;
uniform sampler2D fNormal;
uniform sampler2D fColour;

uniform int control_var; //control variable

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
uniform vec3 lightPosition;
uniform mat4 modelViewTransform;

layout (location = 0) out vec4 vertColour;

vec3 color()
{
    return texture(fPosition, texCoords).rgb;
}

vec3 normals()
{
    return texture(fNormal, texCoords).rgb;
}

vec3 depth()
{
    float t2 = pow(texture(fColour, texCoords).x, 256);
    return vec3(t2,t2,t2);
}

void main()
{
    if(control_var==1) vertColour = vec4(color(), 1);
    else if(control_var==2) vertColour = vec4(normals(), 1);
    else if(control_var==3) vertColour = vec4(depth(),1);
    return;


  // retrieve data from gbuffer
  vec3 vertPosition = texture(fPosition, texCoords).rgb;
  vec3 vertNormal = texture(fNormal, texCoords).rgb;
  vec3 Diffuse = texture(fColour, texCoords).rgb;
  float Specular = texture(fColour, texCoords).a;

  // Ambient colour does not depend on any vectors.
//  vec3 texColour = texture(textureSampler, texCoords).xyz;
  vec3 texColour = texture(fColour, texCoords).xyz;
  vec3 colour    = material.x * texColour;

  // Calculate light direction vectors in the phong model.
  vec3 relativeLightPosition = vec3(modelViewTransform * vec4(lightPosition, 1)); // @FIXME dont calc here?
  vec3 lightDirection   = normalize(relativeLightPosition - vertPosition);
  vec3 normal           = normalize(vertNormal);

  // Diffuse colour.
  float diffuseIntesity = max(dot(normal, lightDirection), 0);
  colour += texColour * material.y * diffuseIntesity ;

  // Specular colour.
  vec3 viewDirection     = normalize(-vertPosition); // The camera is always at (0, 0, 0).
  vec3 reflectDirection  = reflect(-lightDirection, normal);
  float specularIntesity = max(dot(reflectDirection, viewDirection), 0);
  colour += texColour * lightColour * material.z * pow(specularIntesity, material.w);

  vertColour = vec4(colour, 1);
  vertColour = vec4(vertPosition, 1.0);
}

// learnGL code
//#version 330 core
//out vec4 FragColor;

//in vec2 TexCoords;

//uniform sampler2D gPosition;
//uniform sampler2D gNormal;
//uniform sampler2D gAlbedoSpec;

//struct Light {
//    vec3 Position;
//    vec3 Color;

//    float Linear;
//    float Quadratic;
//};
//const int NR_LIGHTS = 32;
//uniform Light lights[NR_LIGHTS];
//uniform vec3 viewPos;

//void main()
//{
//    // retrieve data from gbuffer
//    vec3 FragPos = texture(gPosition, TexCoords).rgb;
//    vec3 Normal = texture(gNormal, TexCoords).rgb;
//    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
//    float Specular = texture(gAlbedoSpec, TexCoords).a;

//    // then calculate lighting as usual
//    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
//    vec3 viewDir  = normalize(viewPos - FragPos);
//    for(int i = 0; i < NR_LIGHTS; ++i)
//    {
//        // diffuse
//        vec3 lightDir = normalize(lights[i].Position - FragPos);
//        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
//        // specular
//        vec3 halfwayDir = normalize(lightDir + viewDir);
//        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
//        vec3 specular = lights[i].Color * spec * Specular;
//        // attenuation
//        float distance = length(lights[i].Position - FragPos);
//        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
//        diffuse *= attenuation;
//        specular *= attenuation;
//        lighting += diffuse + specular;
//    }
//    FragColor = vec4(lighting, 1.0);
//}
