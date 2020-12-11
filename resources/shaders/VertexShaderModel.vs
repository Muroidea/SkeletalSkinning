#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec3 Normal;
out vec2 TexCoords;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

const int MAX_BONES = 100;
uniform mat4[MAX_BONES] Bones;

void main()
{
	mat4 boneTransform = Bones[aBoneIDs[0]] * aWeights[0];
	boneTransform += Bones[aBoneIDs[1]] * aWeights[1];
	boneTransform += Bones[aBoneIDs[2]] * aWeights[2];
	boneTransform += Bones[aBoneIDs[3]] * aWeights[3];

	vec4 pos = boneTransform * vec4(aPos, 1.0);
	gl_Position = Projection * View * Model * pos;
	Normal = aNormal;
	TexCoords = aTexCoords;
}