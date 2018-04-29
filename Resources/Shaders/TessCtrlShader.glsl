#version 430 core

layout (vertices = 3) out;

in VS_OUT
{
	vec2 uv;
	vec3 normal;
	vec3 tangent;
} cs_in[];

out CS_OUT
{
	vec2 uv;
	vec3 normal;
	vec3 tangent;
} cs_out[];

uniform vec3 eyePosition;
uniform float tessellationCount;
uniform float dynamicTessFactor = 1;
uniform float distWeight = 0.08f;

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
	float level = clamp(distance(v0, v1) * distWeight, 1, tessellationCount * dynamicTessFactor * 0.7f);
	//float level = tessellationCount * dynamicTessFactor;
	return level;
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

		if(all(bvec3(
             offscreen(v0),
             offscreen(v1),
             offscreen(v2)
        ))){
             gl_TessLevelInner[0] = 0;
             gl_TessLevelOuter[0] = 0;
             gl_TessLevelOuter[1] = 0;
             gl_TessLevelOuter[2] = 0;
        }
        else{

			vec2 ss0 = screen_space(v0);
             vec2 ss1 = screen_space(v1);
             vec2 ss2 = screen_space(v2);

             float e0 = level(ss1, ss2);
             float e1 = level(ss2, ss0);
             float e2 = level(ss0, ss1);

             //gl_TessLevelInner[0] = (e0 + e1 + e2) * 0.33f;
             //gl_TessLevelOuter[0] = e0;
             //gl_TessLevelOuter[1] = e1;
             //gl_TessLevelOuter[2] = e2;

			 gl_TessLevelInner[0] = (e0 + e1 + e2) * 0.33f;
             gl_TessLevelOuter[0] = e0;
             gl_TessLevelOuter[1] = e1;
             gl_TessLevelOuter[2] = e2;
		}
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	cs_out[gl_InvocationID].uv = cs_in[gl_InvocationID].uv;
	cs_out[gl_InvocationID].normal = cs_in[gl_InvocationID].normal;
	cs_out[gl_InvocationID].tangent = cs_in[gl_InvocationID].tangent;
}