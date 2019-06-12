#version 330

in vec2 fragTexCoords;

out vec4 fragColor;

uniform vec3 uniform_cameraPosition; // camera position
uniform mat4 uniform_mvpTransform_inv; // inverse mvp transformation

// gbuffers
uniform sampler2D uniform_colorTexture;
uniform sampler2D uniform_normalTexture;
uniform sampler2D uniform_positionTexture;
uniform sampler2D uniform_depthTexture;

// settings
uniform int uniform_enableSun;
uniform int uniform_enableLights;
uniform int uniform_currentTexture;

// lights
const int max_light_count = 256;
uniform int light_count;
uniform vec3 lightPositions[max_light_count];
uniform vec3 lightColors[max_light_count];

void main()
{
    // retrieve data from gbuffers
    vec3 color =      texture(uniform_colorTexture, fragTexCoords).xyz;
    vec3 normal = normalize(
                texture(uniform_normalTexture, fragTexCoords).xyz
          );
    vec3 position =   texture(uniform_positionTexture, fragTexCoords).xyz;
    vec3 depth =      texture(uniform_depthTexture, fragTexCoords).xyz;

    // color accumulation
    vec3 color_acc = vec3(0.0, 0.0, 0.0);

    // sun light (directional light)
    vec3 sunLight;
    {
        vec3 l = normalize(vec3(-0.6, 0.35, 0.2)); // sunlight direction
        vec3 diff = color.xyz * max(0.0, dot(normal.xyz, l)) * 0.5; // diffuse

        vec3 v = normalize(uniform_cameraPosition - position);
        vec3 h = normalize(l + v);
        float spec = pow(max(0.0, dot(h, normal)), 16.0) * 0.3; // specular

        vec3 ambi = color.xyz * 0.25; // ambient
        sunLight = vec3(diff + spec + ambi);
    }
    if (uniform_enableSun == 1) color_acc += sunLight;

    // light from bulbs (point-lights)
    vec3 bulbsLight = vec3(0.0, 0.0, 0.0);
    vec3 viewDir  = normalize(uniform_cameraPosition - position);
    for (int i = 0; i < light_count; i ++) {
        vec3 vertPosition;
        { // reconstruct world coordinate position from the pixel coordinates.
            vec4 vertPos;
            vertPos.xy = fragTexCoords * 2.0f - 1.0f;
            vertPos.z = depth.x * 2.0f - 1.0f;
            vertPos.w = 1.0f;
            vec4 loc = uniform_mvpTransform_inv * vertPos;
            vertPosition = loc.xyz / loc.w;
        }

        // From LearnGL
//        float radius;
//        { // pre-calculate (!)
//            float constant  = 1.0;
//            float linear    = 0.7;
//            float quadratic = 1.8;
//            float lightMax  = max(max(lightColors[i].r, lightColors[i].g), lightColors[i].b);
//            radius = (-linear + sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax)))
//            / (2 * quadratic);
//        }
//        {
//            float distance = length(lightPositions[i] - vertPosition);
//            if (distance > 20.0f) continue;

//            vec3 lightDir = normalize(lightPositions[i] - vertPosition);
//            vec3 diffuse = max(dot(normal, lightDir), 0.0) * color * lightColors[i];
//            bulbsLight += diffuse;

//            // specular
//            vec3 halfwayDir = normalize(lightDir + viewDir);
//            float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
//            vec3 specular = lightColors[i] * spec;// * Specular;
//            bulbsLight += specular;
//        }
//continue;
        // Phong shading from OpenGL_3-startpoint
        {

//            // inverse pixel position to original position.
//            vec3 vertPosition = (vec4(position, 1.0) * uniform_mvpTransform_inv).xyz;
//            vertPosition.z = depth.x * 2 - 1;

            float dist = length(lightPositions[i] - vertPosition);
            if (dist > 15.0f) continue;
            vec4 material = vec4(0.5, 0.5, 1, 5);

            // Ambient colour does not depend on any vectors.
//            bulbsLight += material.x * color;

            // Calculate light direction vectors in the phong model.
            vec3 lightDirection   = normalize(lightPositions[i] - vertPosition);
            vec3 normal           = normalize(normal);

            // Diffuse colour.
            float diffuseIntensity = max(dot(normal, lightDirection), 0);
            bulbsLight += color * material.y * diffuseIntensity;

            // Specular colour.
            vec3 viewDirection     = normalize(uniform_cameraPosition - vertPosition); // camera?!
            vec3 reflectDirection  = reflect(-lightDirection, normal);
            float specularIntensity = max(dot(reflectDirection, viewDirection), 0);
            bulbsLight += color * lightColors[i] * material.z * pow(specularIntensity, material.w);
        }
    }
    if (uniform_enableLights == 1) color_acc += bulbsLight;

    // output color
    fragColor = vec4(color_acc, 1.0);

    if (uniform_currentTexture == 1) fragColor = vec4(color, 1.0);
    if (uniform_currentTexture == 2) fragColor = vec4(normal * 0.5 + 0.5, 1.0); // map range
    if (uniform_currentTexture == 3) fragColor = vec4(position, 1.0);
    if (uniform_currentTexture == 4) fragColor = vec4(depth, 1.0);
}
