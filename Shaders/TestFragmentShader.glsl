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

	float col = texture( heightMap, fs_in.uv).r * 5.0f;
	color.r = col--;
	color.g = max(0, col--);
	color.b = max(0, col--);
}