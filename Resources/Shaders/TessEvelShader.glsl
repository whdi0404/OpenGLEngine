#version 430 core
//fractional_even_spacing
//quads
layout (triangles, fractional_even_spacing, ccw) in;


in CS_OUT
{
	vec2 uv;
	vec3 normal;
	vec3 tangent;
} es_in[];

out ES_OUT
{
	vec2 uv;
	vec3 normal;
	vec2 tessuv;
} es_out;

//gl_TessCoord.x, gl_TessCoord.y, gl_TessCoord.z에 세개 정점의 보간 정보가 들어있음.

void main()
{
	es_out.uv = 
    gl_TessCoord.x * es_in[0].uv +
    gl_TessCoord.y * es_in[1].uv + 
    gl_TessCoord.z * es_in[2].uv;

	es_out.tessuv = es_out.uv;

    gl_Position = gl_TessCoord.x * gl_in[0].gl_Position +
    gl_TessCoord.y * gl_in[1].gl_Position + 
    gl_TessCoord.z * gl_in[2].gl_Position;

	es_out.normal = vec3(0,0,0);
}