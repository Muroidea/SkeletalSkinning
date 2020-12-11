#include "Skeleton.h"

void Skeleton::TransformBones(std::vector<glm::mat4>& transforms, float time, const Animation* animation)
{
    GetBoneTransformation(transforms[0], time, Bone(), m_Bones[0], animation);

    for (unsigned int i = 1; i < m_Bones.size(); i++)
    {
        Bone& bone = m_Bones[i];
        GetBoneTransformation(transforms[i], time, m_Bones[bone.ParentID], bone, animation);
    }
}

void Skeleton::GetTPoseTranformation(std::vector<glm::mat4>& transforms)
{
    for (unsigned int i = 0; i < m_Bones.size(); i++)
        transforms[i] = glm::mat4(1.0f);
}

void Skeleton::GetBoneTransformation(glm::mat4& transform, float animationTime, const Bone& parentBone, Bone& bone, const Animation* animation)
{
    glm::mat4 boneTransformation = bone.DefaultTransformation;

    KeyFrame first, second;
    animation->GetBoneKeyFrames(first, second, bone.Name, animationTime);

    if (first.Time > -1.0 && second.Time > -1.0)
    {
        float deltaTime = second.Time - first.Time;
        float factor = (animationTime - first.Time) / deltaTime;

        //Interpolating values

        // Scalling calculation
        glm::vec3 interpolatedScalling = first.Scaling + factor * (second.Scaling - first.Scaling);
        glm::mat4 scallingMat = glm::scale(glm::mat4(1.0f), interpolatedScalling);

        // Rotation calculation
        glm::quat interpolatedRotation = glm::slerp(first.Rotation, second.Rotation, factor);
        interpolatedRotation = glm::normalize(interpolatedRotation);
        glm::mat4 rotationMat = glm::toMat4(interpolatedRotation);

        // Translation calculation
        glm::vec3 interpolatedTranslation = first.Translation + factor * (second.Translation - first.Translation);
        glm::mat4 translationgMat = glm::translate(glm::mat4(1.0f), interpolatedTranslation);

        boneTransformation = translationgMat * rotationMat * scallingMat;
    }
    else if (first.Time > -1.0)
    {
        glm::mat4 scallingMat = glm::scale(glm::mat4(1.0f), first.Scaling);
        glm::mat4 rotationMat = glm::toMat4(first.Rotation);
        glm::mat4 translationgMat = glm::translate(glm::mat4(1.0f), first.Translation);

        boneTransformation = translationgMat * rotationMat * scallingMat;
    }

    bone.Transformation = parentBone.Transformation * boneTransformation;

    transform = m_GlobalInverseTransform * bone.Transformation * bone.Offset;
}