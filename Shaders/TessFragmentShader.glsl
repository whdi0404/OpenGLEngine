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

	/*float col = texture( heightMap, fs_in.uv).r;
	color.b = fs_in.uv.g;
	color.r = fs_in.uv.r;
	color.g = col;*/
	color = vec3(0,0,0);
	if(fs_in.uv.r <=0 || fs_in.uv.r >= 1)
		color.r = 1;
	if(fs_in.uv.g <=0 || fs_in.uv.g >= 1)
		color.g = 1;
}