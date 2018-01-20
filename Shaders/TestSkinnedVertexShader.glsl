#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec4 boneWeights;
layout(location = 3) in vec4 boneIndices;
layout(location = 4) in mat4 matModel;

uniform mat4 matView;
uniform mat4 matProj;
uniform sampler2D heightMap;
uniform int boneCount;

layout(std430, binding = 1) readonly buffer ssbo1
{
    mat4 matBones[];
}bones;

out VS_OUT
{
	vec2 uv;
} vs_out;

void main()
{
	vec4 pos = vec4(vertexPosition_modelspace, 1);

	int index = int(round(boneIndices.x));
	mat4x4 newMat = bones.matBones[boneCount * gl_InstanceID + index] * boneWeights.x;
       
    index = int(round(boneIndices.y));
	newMat += bones.matBones[boneCount * gl_InstanceID + index] * boneWeights.y;
	
    index = int(round(boneIndices.z));
	newMat += bones.matBones[boneCount * gl_InstanceID + index] * boneWeights.z;
	
    index = int(round(boneIndices.w));
	newMat += bones.matBones[boneCount * gl_InstanceID + index] * boneWeights.w;

	gl_Position = matProj * matView * matModel * newMat * pos;
	vs_out.uv = vertexUV;
}