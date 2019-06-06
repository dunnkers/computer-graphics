#version 330

uniform sampler2D uniform_colorTexture;
uniform sampler2D uniform_normalTexture;
uniform sampler2D uniform_positionTexture;


out vec4 outColor;

in vec4 fsPos;

uniform float uLightRadius;
uniform vec3 uLightPosition;
uniform vec3 uLightColor;

uniform vec3 uCameraPos;


void main() {

    // get screen-space position of light sphere
    // (remember to do perspective division.)
    vec2 uv = (fsPos.xy / fsPos.w) * 0.5 + 0.5;

    // now we can sample from the gbuffer for every fragment the light sphere covers.
    vec3 albedo = texture(uniform_colorTexture, uv).xyz;
    vec3 n = normalize(texture(uniform_normalTexture, uv).xyz);
    vec3 pos = texture(uniform_positionTexture, uv).xyz;

    vec3 lightToPosVector = pos.xyz - uLightPosition;
    float lightDist = length(lightToPosVector);  // position from light.
    vec3 l = -lightToPosVector / (lightDist);

    // implement fake z-test. If too far from light center, then 0.
    float ztest = step(0.0, uLightRadius - lightDist);

    // light attenuation.
    float d = lightDist / uLightRadius;
    float attenuation = 1.0 - d;
    vec3 v = normalize(uCameraPos - pos);
    vec3 h = normalize(l + v);

    vec3 color =
        // diffuse
        uLightColor * albedo.xyz * max(0.0, dot(n.xyz, l)) +
        // specular
        uLightColor * 0.4 * pow(max(0.0, dot(h, n)), 12.0); 

    // finally ztest and attenuation.
    color *= ztest * attenuation;

    outColor = vec4(color, 1.0); // done!
}
