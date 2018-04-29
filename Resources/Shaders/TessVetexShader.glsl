#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 vertexUV;
layout(location = 4) in mat4 matModel;

out VS_OUT
{
	vec2 uv;
	vec3 normal;
	vec3 tangent;
} vs_out;

void main()
{
	gl_Position = matModel * vec4(vertexPosition_modelspace, 1);
	vs_out.uv = vertexUV;
	vs_out.normal = normal;
	vs_out.tangent = tangent;
}