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
uniform float tessellationCount;
uniform float dynamicTessFactor = 1;

uniform vec2 screen_size = vec2(1280, 720);
uniform mat4 matView;
uniform mat4 matProj;

vec4 project(vec4 vertex){
    vec4 result = matProj * matView * vertex;
    result /= result.w;
    return result;
}

vec2 screen_space(vec4 vertex){
    return (clamp(vertex.xy, -1.3, 1.3)) * (screen_size*0.5);
}

float level(vec2 v0, vec2 v1){
     //return 1;
	 return clamp(distance(v0, v1) * 0.125f, 1, dynamicTessFactor * tessellationCount * 2);
 }

 bool offscreen(vec4 vertex){
    if(vertex.z < -0.5){
        return true;
    }
    return any(
        lessThan(vertex.xy, vec2(-2.5)) ||
        greaterThan(vertex.xy, vec2(2.5))
    );  
}

void main(void)
{
	if (gl_InvocationID == 0)
	{
		vec4 v0 = project(gl_in[0].gl_Position);
		vec4 v1 = project(gl_in[1].gl_Position);
		vec4 v2 = project(gl_in[2].gl_Position);
		vec4 v3 = project(gl_in[3].gl_Position);

		if(all(bvec4(
             offscreen(v0),
             offscreen(v1),
             offscreen(v2),
             offscreen(v3)
        ))){
             gl_TessLevelInner[0] = 0;
             gl_TessLevelInner[1] = 0;
             gl_TessLevelOuter[0] = 0;
             gl_TessLevelOuter[1] = 0;
             gl_TessLevelOuter[2] = 0;
             gl_TessLevelOuter[3] = 0;
        }
        else{

			vec2 ss0 = screen_space(v0);
             vec2 ss1 = screen_space(v1);
             vec2 ss2 = screen_space(v2);
             vec2 ss3 = screen_space(v3);

             float e0 = level(ss1, ss2);
             float e1 = level(ss0, ss1);
             float e2 = level(ss3, ss0);
             float e3 = level(ss2, ss3);

             gl_TessLevelInner[0] = mix(e1, e2, 0.5);
             gl_TessLevelInner[1] = mix(e0, e3, 0.5);
             gl_TessLevelOuter[0] = e0;
             gl_TessLevelOuter[1] = e1;
             gl_TessLevelOuter[2] = e2;
             gl_TessLevelOuter[3] = e3;
		}
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	cs_out[gl_InvocationID].uv = cs_in[gl_InvocationID].uv;
}