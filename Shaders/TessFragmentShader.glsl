#version 410 core

out vec3 color;

in ES_OUT
{
	vec2 uv;
	vec2 tessuv;
} fs_in;

uniform sampler2D heightMap;

void main(){
	//color = vec3(fs_in.tessuv, 1);
	//color.rg -= vec2(int(color.r), int(color.g));

	float col = texture( heightMap, fs_in.uv).r;
	color.b = col;
	color.r = fs_in.uv.x;
	color.g = fs_in.uv.y;
}