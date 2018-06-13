#version 410 core

out vec3 color;

in GS_OUT
{
	vec2 uv;
	vec3 normal;
	vec2 tessuv;
} fs_in;

uniform sampler2D heightMap;
uniform sampler2D normalMap;
uniform sampler2D albedoMap;
uniform float heightScale;

void main(){
	//color = vec3(fs_in.tessuv, 1);
	//color.rg -= vec2(int(color.r), int(color.g));

	//float col = texture( heightMap, fs_in.uv).r;
	//color = vec3(fs_in.uv, 0);
	//color.g = col;

	//color = vec3(0,0,0);
	//if(fs_in.uv.r <=0 || fs_in.uv.r >= 1)
	//	color.r = 1;
	//if(fs_in.uv.g <=0 || fs_in.uv.g >= 1)
	//	color.g = 1;

	//vec3 normal = normalize(cross(fs_in.d1, fs_in.d2));

	vec3 light = normalize(vec3(0.0f,1.0f,0));
	//color = texture( normalMap, fs_in.uv).rgb;

	float l = dot(texture( normalMap, fs_in.uv).rgb, light);
	color = vec3(l,l,l);//texture( normalMap, fs_in.uv).rgb;//fs_in.normal;//texture( albedoMap, fs_in.uv).rgb;//
}