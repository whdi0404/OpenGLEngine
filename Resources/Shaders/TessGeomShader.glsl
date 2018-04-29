#version 430 core

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

uniform float heightScale;

uniform sampler2D heightMap;

uniform mat4 matView;
uniform mat4 matProj;

in ES_OUT
{
	vec2 uv;
	vec3 normal;
	vec2 tessuv;
} gs_in[];

out GS_OUT
{
	vec2 uv;
	vec3 normal;
	vec2 tessuv;
} gs_out;

mat3 mat3_emu(mat4 m4) {
  return mat3(
      m4[0][0], m4[0][1], m4[0][2],
      m4[1][0], m4[1][1], m4[1][2],
      m4[2][0], m4[2][1], m4[2][2]);
}

void main(void)
{
	//노말은 GeometryShader에서 구하자.

	vec4 v1 = gl_in[0].gl_Position;
	v1.y = texture2D(heightMap, gs_in[0].uv).r * heightScale;
	
	vec4 v2 = gl_in[1].gl_Position;
	v2.y = texture2D(heightMap, gs_in[1].uv).r * heightScale;
	
	vec4 v3 = gl_in[2].gl_Position;
	v3.y = texture2D(heightMap, gs_in[2].uv).r * heightScale;
   
    vec3 A = v2.xyz - v1.xyz;
    vec3 B = v3.xyz - v1.xyz;

	gs_out.tessuv = gs_in[0].tessuv;

	gs_out.normal = normalize(cross(A,B));
	gl_Position = matProj * matView * v1;
	gs_out.uv = gs_in[0].uv;
    EmitVertex();
    gl_Position = matProj * matView * v2;
    gs_out.uv = gs_in[1].uv;
	EmitVertex();
    gl_Position = matProj * matView * v3;
    gs_out.uv = gs_in[2].uv;
	EmitVertex();
    EndPrimitive();
}