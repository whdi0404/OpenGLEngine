#version 430 core

layout (vertices = 4) out;

in VS_OUT
{
	vec2 uv;
} cs_in[];
out CS_OUT
{
	vec2 uv;
} cs_out[];

uniform vec3 eyePosition;
uniform float dynamicTessFactor = 64;

uniform vec2 screen_size = vec2(1280, 720);
uniform mat4 matView;
uniform mat4 matProj;

vec4 project(vec4 vertex){
    vec4 result = matProj * matView * vertex;
    result /= result.w;
    return result;
}

vec2 screen_space(vec4 vertex){
    return (clamp(vertex.xy, -1.3, 1.3)+1) * (screen_size*0.5);
}

float level(vec2 v0, vec2 v1){
     return clamp(distance(v0, v1)/dynamicTessFactor, 1, 16);
 }

 float level2(float v0, float v1){
	//return 1;
    return clamp(pow(1 / (v0 + v1), 1.5f) * 1024.f, 1, dynamicTessFactor);
 }

 bool offscreen(vec4 vertex){
    if(vertex.z < -0.5){
        return true;
    }
    return any(
        lessThan(vertex.xy, vec2(-1.7)) ||
        greaterThan(vertex.xy, vec2(1.7))
    );  
}

void main(void)
{
	if (gl_InvocationID == 0)
	{
		//vec4 v0 = project(gl_in[0].gl_Position);
		//vec4 v1 = project(gl_in[1].gl_Position);
		//vec4 v2 = project(gl_in[2].gl_Position);
		//vec4 v3 = project(gl_in[3].gl_Position);

		//if(all(bvec4(
        //     offscreen(v0),
        //     offscreen(v1),
        //     offscreen(v2),
        //     offscreen(v3)
        //))){
        //     gl_TessLevelInner[0] = 0;
        //     gl_TessLevelInner[1] = 0;
        //     gl_TessLevelOuter[0] = 0;
        //     gl_TessLevelOuter[1] = 0;
        //     gl_TessLevelOuter[2] = 0;
        //     gl_TessLevelOuter[3] = 0;
        //}
        //else{

			float ss0 = distance(gl_in[0].gl_Position.xyz, eyePosition.xyz);
			float ss1 = distance(gl_in[1].gl_Position.xyz, eyePosition.xyz);
			float ss2 = distance(gl_in[2].gl_Position.xyz, eyePosition.xyz);
			float ss3 = distance(gl_in[3].gl_Position.xyz, eyePosition.xyz);

			float e0 = level2(ss1, ss2);
			float e1 = level2(ss0, ss1);
			float e2 = level2(ss3, ss0);
			float e3 = level2(ss2, ss3);
			
			gl_TessLevelInner[0] = mix(e1, e2, 0.5f);
			gl_TessLevelInner[1] = mix(e0, e3, 0.5f);

			gl_TessLevelOuter[0] = e0;
			gl_TessLevelOuter[1] = e1;
			gl_TessLevelOuter[2] = e2;
			gl_TessLevelOuter[3] = e3;

			// = gl_TessLevelInner[1] = (e0 + e1 + e2 + e3) * 0.25f;
		//}
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	cs_out[gl_InvocationID].uv = cs_in[gl_InvocationID].uv;
}