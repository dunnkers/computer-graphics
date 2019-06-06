#version 330
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;

uniform sampler2D textureDiff;

out vec4 geoData[3];

void main()
{
    vec4 diff = texture(textureDiff, fragTexCoords).rgba;
    if (diff.a < 0.2) {
        discard;
    }

    // output geometry.
    geoData[0] = vec4(diff.rgb, 1);
    geoData[1] = vec4(fragNormal, 1);
    geoData[2] = vec4(fragPosition, 1);
}
