#version 330

uniform vec3 uniform_cameraPosition; // camera position

uniform sampler2D uniform_colorTexture;
uniform sampler2D uniform_normalTexture;
uniform sampler2D uniform_positionTexture;

out vec4 fragColor;

in vec4 fragPosition;

uniform float lightRad;
uniform vec3 lightPos;
uniform vec3 lightCol;

// settings
uniform int uniform_enableLights;

void main() {
    // light sphere screen space retrieval. map to correct range.
    vec2 fragTexCoords = (fragPosition.xy / fragPosition.w) * 0.5 + 0.5;

    // retrieve data from gbuffers
    vec3 color =      texture(uniform_colorTexture, fragTexCoords).xyz;
    vec3 normal = normalize(
                      texture(uniform_normalTexture, fragTexCoords).xyz
                );
    vec3 position =   texture(uniform_positionTexture, fragTexCoords).xyz;

    // compute new light position
    vec3 vector_lightToPosition = position.xyz - lightPos;
    float lightDistance = length(vector_lightToPosition);  // position from the light

    vec3 l = -vector_lightToPosition / lightDistance;
    vec3 diff = lightCol * color.xyz * max(0.0, dot(normal.xyz, l)); // diffuse

    vec3 v = normalize(uniform_cameraPosition - position);
    vec3 h = normalize(l + v);
    vec3 spec = lightCol * 0.4 * pow(max(0.0, dot(h, normal)), 12.0); // specular

    // do some depth test.
    float ztest = step(0.0, lightRad - lightDistance); // set 0 when too far from light centre

    // light attenuation.
    float d = lightDistance / lightRad;
    float attenuation = 1.0 - d;

    // final color
    vec3 outColor = diff + spec;
    outColor *= ztest * attenuation; // plus attenuation

    fragColor = vec4(outColor, 1.0);
//    fragColor = vec4(normal * 0.5 + 0.5, 1.0);

    if (uniform_enableLights == 0) fragColor = vec4(0, 0, 0, 0);
}
