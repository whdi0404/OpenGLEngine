#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in mat4 matModel;

uniform mat4 matView;
uniform mat4 matProj;

out VS_OUT
{
	vec2 uv;
} vs_out;

void main()
{
	gl_Position = matProj * matView * matModel * vec4(vertexPosition_modelspace, 1);

	vs_out.uv = vertexUV;
}