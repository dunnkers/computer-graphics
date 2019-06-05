#version 330
out vec2 fsUv;

// full screen triangle vertices.
const vec2 verts[3] = vec2[](vec2(-1, -1), vec2(3, -1), vec2(-1, 3));
const vec2 uvs[3] = vec2[](vec2(0, 0), vec2(2, 0), vec2(0, 2));

void main()
{
  fsUv = uvs[gl_VertexID];
  gl_Position = vec4( verts[gl_VertexID], 0.0, 1.0);

//  float x = -1.0 + float((gl_VertexID & 1) << 2);
//  float y = -1.0 + float((gl_VertexID & 2) << 1);
//  fsUv.x = (x+1.0)*0.5;
//  fsUv.y = (y+1.0)*0.5;
//  gl_Position = vec4(x, y, 0, 1);
}
