#version 410 core

out vec4 color;

in VS_OUT
{
	vec2 uv;
} fs_in;

uniform sampler2D tex;

void main()
{
	color = vec4(fs_in.uv,0,1);//texture( tex, fs_in.uv );//
}