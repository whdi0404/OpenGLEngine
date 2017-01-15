#version 410 core

out vec4 color;

in VS_OUT
{
	vec2 uv;
} fs_in;

uniform sampler2D tex;

void main()
{
	color = texture( tex, fs_in.uv );
}