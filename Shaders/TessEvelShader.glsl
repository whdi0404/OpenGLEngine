#version 430 core
//fractional_even_spacing
layout (quads, fractional_even_spacing, ccw) in;
uniform mat4 matView;
uniform mat4 matProj;
uniform float heightScale;

uniform sampler2D heightMap;

in CS_OUT
{
	vec2 uv;
} es_in[];
out ES_OUT
{
	vec2 uv;
	vec2 tessuv;
} es_out;

//gl_TessCoord.x, gl_TessCoord.y, gl_TessCoord.z에 세개 정점의 보간 정보가 들어있음.

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
   	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
	vec2 uv1 = mix(es_in[1].uv,es_in[0].uv,gl_TessCoord.x);
	vec2 uv2 = mix(es_in[2].uv,es_in[3].uv,gl_TessCoord.x);
	es_out.uv = mix(uv1, uv2, gl_TessCoord.y); 
	es_out.tessuv = gl_TessCoord.xy;

	vec4 p1 = mix(gl_in[1].gl_Position,gl_in[0].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,gl_in[3].gl_Position, gl_TessCoord.x);
	gl_Position = mix(p1, p2, gl_TessCoord.y);

	gl_Position.y = texture2D(heightMap, es_out.uv).r * heightScale;

	gl_Position = matProj * matView * gl_Position;
}