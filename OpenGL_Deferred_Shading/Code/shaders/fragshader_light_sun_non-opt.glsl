#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 fragColor;

uniform vec3 uniform_cameraPosition; // camera position

uniform sampler2D textureDiff;

void main()
{
  vec3 color =  texture(textureDiff, fragTexCoords).xyz;
  vec3 normal = fragNormal;
  vec3 position = fragPosition;

  vec3 l = normalize(vec3(-0.65, 0.40, 0.25));
  vec3 diff = color.xyz * max(0.0, dot(normal.xyz, l)) * 0.75; // diffuse

  vec3 v = normalize(uniform_cameraPosition - position);
  vec3 h = normalize(l + v);
  float spec = pow(max(0.0, dot(h, normal)), 16.0) * 0.3; // specular

  vec3 ambi = color.xyz * 0.25; // ambient

  fragColor = vec4(vec3(diff + spec + ambi), 1.0);
}
