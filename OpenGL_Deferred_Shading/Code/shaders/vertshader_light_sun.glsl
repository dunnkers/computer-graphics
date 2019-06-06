#version 330

out vec2 fragTexCoords;

// used to draw a full-screen quad.
// vertices
const vec2 verts[3] = vec2[](
    vec2(-1, -1),
    vec2(3, -1),
    vec2(-1, 3)
);

// uv-coords
const vec2 uvs[3] = vec2[](
    vec2(0, 0),
    vec2(2, 0),
    vec2(0, 2)
);

void main()
{
  gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);
  fragTexCoords = uvs[gl_VertexID];
}
