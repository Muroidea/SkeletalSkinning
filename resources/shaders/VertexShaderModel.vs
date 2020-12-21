#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec3 Normal;
out vec2 TexCoords;

uniform mat4 View;
uniform mat4 Projection;

uniform int SkinningMode;

const int MAX_BONES = 100;
layout (std140) uniform PerModelData
{
	mat4[MAX_BONES] Bones;
	mat2x4[MAX_BONES] BonesDQ;
	mat4 Model;
};

void main()
{
	mat4 boneTransform;
	vec4 pos;

	if (SkinningMode == 0)
	{
		boneTransform = Bones[aBoneIDs[0]] * aWeights[0];
		boneTransform += Bones[aBoneIDs[1]] * aWeights[1];
		boneTransform += Bones[aBoneIDs[2]] * aWeights[2];
		boneTransform += Bones[aBoneIDs[3]] * aWeights[3];
		
		pos = boneTransform * vec4(aPos, 1.0);
	}
	else if (SkinningMode == 1)
	{
		mat2x4 dq0 = BonesDQ[aBoneIDs[0]];
		mat2x4 dq1 = BonesDQ[aBoneIDs[1]];
		mat2x4 dq2 = BonesDQ[aBoneIDs[2]];
		mat2x4 dq3 = BonesDQ[aBoneIDs[3]];

		//if(dot(dq0[0], dq1[0]) < 0.0) dq1 *= -1.0;
		//if(dot(dq0[0], dq2[0]) < 0.0) dq2 *= -1.0;
		//if(dot(dq0[0], dq3[0]) < 0.0) dq3 *= -1.0;

		dq1 *= sign(dot(dq0[0], dq1[0]));
		dq2 *= sign(dot(dq0[0], dq2[0]));
		dq3 *= sign(dot(dq0[0], dq3[0]));

		mat2x4 blendedDQ = dq0 * aWeights[0];
		blendedDQ += dq1 * aWeights[1];
		blendedDQ += dq2 * aWeights[2];
		blendedDQ += dq3 * aWeights[3];

		float len = length(blendedDQ[0]);
		blendedDQ /= len;

		pos = vec4(aPos.xyz + 2.0 * cross(blendedDQ[0].xyz, cross(blendedDQ[0].xyz, aPos.xyz) + blendedDQ[0].w * aPos.xyz) + // rotation part
					2.0 * (blendedDQ[0].w * blendedDQ[1].xyz - blendedDQ[1].w * blendedDQ[0].xyz + cross(blendedDQ[0].xyz, blendedDQ[1].xyz)), 1.0); // translation part
	}

	gl_Position = Projection * View * Model * pos;
	Normal = aNormal;
	TexCoords = aTexCoords;
}
