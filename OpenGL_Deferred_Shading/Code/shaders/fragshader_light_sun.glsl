#version 330

in vec2 fragTexCoords;

out vec4 fragColor;

uniform vec3 uniform_cameraPosition; // camera position

uniform sampler2D uniform_colorTexture;
uniform sampler2D uniform_normalTexture;
uniform sampler2D uniform_positionTexture;
uniform sampler2D uniform_depthTexture;

// settings
uniform int uniform_enableSun;
uniform int uniform_currentTexture;

void main()
{
  // retrieve data from gbuffers
  vec3 color =      texture(uniform_colorTexture, fragTexCoords).xyz;
  vec3 normal = normalize(
                    texture(uniform_normalTexture, fragTexCoords).xyz
              );
  vec3 position =   texture(uniform_positionTexture, fragTexCoords).xyz;
  vec3 depth =      texture(uniform_depthTexture, fragTexCoords).xyz;

  vec3 l = normalize(vec3(-0.65, 0.40, 0.25));
  vec3 diff = color.xyz * max(0.0, dot(normal.xyz, l)) * 0.75; // diffuse

  vec3 v = normalize(uniform_cameraPosition - position);
  vec3 h = normalize(l + v);
  float spec = pow(max(0.0, dot(h, normal)), 16.0) * 0.3; // specular

  vec3 ambi = color.xyz * 0.25; // ambient

  fragColor = vec4(vec3(diff + spec + ambi), 1.0);

  if (uniform_currentTexture == 1) fragColor = vec4(color, 1.0);
  if (uniform_currentTexture == 2) fragColor = vec4(normal, 1.0); // map? (* 0.5 + 0.5)
  if (uniform_currentTexture == 3) fragColor = vec4(position, 1.0);
  if (uniform_currentTexture == 4) fragColor = vec4(depth, 1.0);
  if (uniform_enableSun == 0) fragColor = vec4(0, 0, 0, 0);
}
