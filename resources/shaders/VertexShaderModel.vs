#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in ivec4 aBoneIDs;
layout (location = 3) in vec4 aWeights;
layout (location = 4) in vec3 aCenterOfRotation;

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

vec4 QuatAdd(vec4 q1, vec4 q2);
mat3 QuatToRotationMatrix(vec4 quat);

void main()
{
	mat4 boneTransform;
	vec4 pos;

	if (SkinningMode == 0) // LinearBlendSkinning
	{
		boneTransform = Bones[aBoneIDs[0]] * aWeights[0];
		boneTransform += Bones[aBoneIDs[1]] * aWeights[1];
		boneTransform += Bones[aBoneIDs[2]] * aWeights[2];
		boneTransform += Bones[aBoneIDs[3]] * aWeights[3];
		
		pos = boneTransform * vec4(aPos, 1.0);
	}
	else if (SkinningMode == 1) // DualQuaternionSkinning
	{
		mat2x4 dq0 = BonesDQ[aBoneIDs[0]];
		mat2x4 dq1 = BonesDQ[aBoneIDs[1]];
		mat2x4 dq2 = BonesDQ[aBoneIDs[2]];
		mat2x4 dq3 = BonesDQ[aBoneIDs[3]];

		dq1 *= sign(dot(dq0[0], dq1[0]));
		dq2 *= sign(dot(dq0[0], dq2[0]));
		dq3 *= sign(dot(dq0[0], dq3[0]));

		mat2x4 blendedDQ = dq0 * aWeights[0];
		blendedDQ += dq1 * aWeights[1];
		blendedDQ += dq2 * aWeights[2];
		blendedDQ += dq3 * aWeights[3];

		blendedDQ /= length(blendedDQ[0]);

		pos = vec4(aPos.xyz + 2.0 * cross(blendedDQ[0].xyz, cross(blendedDQ[0].xyz, aPos.xyz) + blendedDQ[0].w * aPos.xyz) + // rotation part
					2.0 * (blendedDQ[0].w * blendedDQ[1].xyz - blendedDQ[1].w * blendedDQ[0].xyz + cross(blendedDQ[0].xyz, blendedDQ[1].xyz)), 1.0); // translation part
	}
	else // CenterOfRotationSkinning
	{
		vec4 quatRotation = vec4(0);
		mat4 lbs = mat4(0);

	
		if (aCenterOfRotation == vec3(0.0, 0.0, 0.0))
		{
			for (int i = 0; i < 4; i++) 
				lbs += aWeights[i] * Bones[aBoneIDs[i]];

			boneTransform = lbs;
		}
		else
		{
			for (int i = 0; i < 4; i++) 
			{
				quatRotation = QuatAdd(quatRotation, aWeights[i] * BonesDQ[aBoneIDs[i]][0]);
				lbs += aWeights[i] * Bones[aBoneIDs[i]];
			}

			quatRotation /= length(quatRotation);
			mat3 quatRotationMatrix = QuatToRotationMatrix(quatRotation);

			vec4 translation = vec4((lbs * vec4(aCenterOfRotation, 1.0) - vec4(quatRotationMatrix * aCenterOfRotation, 0.0)).xyz, 1.0);

			boneTransform = mat4(quatRotationMatrix);
			boneTransform[3] = translation;
		}

		pos = boneTransform * vec4(aPos, 1.0);
	}

	gl_Position = Projection * View * Model * pos;
	TexCoords = aTexCoords;
}

vec4 QuatAdd(vec4 q1, vec4 q2)
{
	if (dot(q1, q2) >= 0) 
		return q1 + q2;
	else 
		return q1 - q2;
}

mat3 QuatToRotationMatrix(vec4 quat)
{
	float x2 = quat.x * quat.x; 
    float y2 = quat.y * quat.y; 
    float z2 = quat.z * quat.z; 
    
    float xy = quat.x * quat.y; 
    float xz = quat.x * quat.z; 
    float yz = quat.y * quat.z; 
    float wx = quat.w * quat.x; 
    float wy = quat.w * quat.y; 
    float wz = quat.w * quat.z;

	return mat3(
		vec3(
		1.0f - 2.0f * (y2 + z2), 
		2.0f * (xy + wz), 
		2.0f * (xz - wy) ), 
		
		vec3(
		2.0f * (xy - wz), 
		1.0f - 2.0f * (x2 + z2), 
		2.0f * (yz + wx) ),
		
		vec3(
		2.0f * (xz + wy), 
		2.0f * (yz - wx), 
		1.0f - 2.0f * (x2 + y2) )
	);
}