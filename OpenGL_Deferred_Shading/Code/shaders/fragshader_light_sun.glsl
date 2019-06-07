#version 330

in vec2 fragTexCoords;

out vec4 fragColor;

uniform vec3 uniform_cameraPosition; // camera position

uniform sampler2D uniform_colorTexture;
uniform sampler2D uniform_normalTexture;
uniform sampler2D uniform_positionTexture;

// settings
uniform int uniform_enableSun;

void main()
{
  // retrieve data from gbuffers
  vec3 color =      texture(uniform_colorTexture, fragTexCoords).xyz;
  vec3 normal = normalize(
                    texture(uniform_normalTexture, fragTexCoords).xyz
              );
  vec3 position =   texture(uniform_positionTexture, fragTexCoords).xyz;

  vec3 l = normalize(vec3(-0.6, 0.35, 0.2));
  vec3 diff = color.xyz * max(0.0, dot(normal.xyz, l)) * 0.75; // diffuse

  vec3 v = normalize(uniform_cameraPosition - position);
  vec3 h = normalize(l + v);
  float spec = pow(max(0.0, dot(h, normal)), 16.0) * 0.3; // specular

  vec3 ambi = color.xyz * 0.25; // ambient

  fragColor = vec4(vec3(diff + spec + ambi), 1.0);
//  fragColor = vec4(normal * 0.5 + 0.5, 1.0);

  if (uniform_enableSun == 0) fragColor = vec4(0, 0, 0, 0);
}
