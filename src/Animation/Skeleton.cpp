#include "Skeleton.h"

#include <glm/gtx/string_cast.hpp>

void Bone::SetOffset(glm::mat4 offset)
{
    Offset = offset;

    OffsetDQ = glm::normalize(glm::fdualquat(glm::normalize(glm::quat_cast(offset)), glm::vec3(offset[3])));
}

void Bone::SetDefaultTransformation(glm::mat4 defaultTransformation)
{
    DefaultTransformation = defaultTransformation;

    DefaultTransformationDQ = glm::normalize(glm::fdualquat(glm::normalize(glm::quat_cast(defaultTransformation)), glm::vec3(defaultTransformation[3])));
}

void Skeleton::SetGlobalInverseMatrix(glm::mat4 globalInverseMatrix)
{
    m_GlobalInverseTransform = globalInverseMatrix;

    m_GlobalInverseTransformDQ = glm::normalize(glm::fdualquat(glm::normalize(glm::quat_cast(globalInverseMatrix)), glm::vec3(globalInverseMatrix[3])));
}

void Skeleton::TransformBonesMat4(std::vector<glm::mat4>& transforms, float time, const Animation* animation)
{
    GetBoneTransformationMat4(transforms[0], time, Bone(), m_Bones[0], animation);

    for (unsigned int i = 1; i < m_Bones.size(); i++)
    {
        Bone& bone = m_Bones[i];
        GetBoneTransformationMat4(transforms[i], time, m_Bones[bone.ParentID], bone, animation);
    }
}

void Skeleton::TransformBonesDualQuat(std::vector<glm::fdualquat>& transforms, float time, const Animation* animation)
{
    GetBoneTransformationDualQuat(transforms[0], time, Bone(), m_Bones[0], animation);

    for (unsigned int i = 1; i < m_Bones.size(); i++)
    {
        Bone& bone = m_Bones[i];
        GetBoneTransformationDualQuat(transforms[i], time, m_Bones[bone.ParentID], bone, animation);
    }
}

void Skeleton::TransformBonesMat4(std::vector<glm::mat4>& transforms, const AnimationSequence* animSequence)
{
    if (!animSequence->IsEnabled()) return;

    auto anim1 = animSequence->GetFirst();
    TransformBonesMat4(transforms, anim1->GetTime(), anim1->GetAnimation());
    
    if (animSequence->IsBlended())
    {
        std::vector<glm::mat4> transformsTmp;
        transformsTmp = transforms;

        auto anim2 = animSequence->GetSecond();
        if (anim2)
        {
            TransformBonesMat4(transforms, anim2->GetTime(), anim2->GetAnimation());

            float factor = animSequence->GetFactor();
            for (int i = 0; i < transforms.size(); i++)
            {
                transforms[i] = transforms[i] * (1.0f - factor) + transformsTmp[i] * factor;
            }

        }
    }
}

void Skeleton::TransformBonesDualQuat(std::vector<glm::fdualquat>& transforms, const AnimationSequence* animSequence)
{
    if (!animSequence->IsEnabled()) return;

    auto anim1 = animSequence->GetFirst();
    TransformBonesDualQuat(transforms, anim1->GetTime(), anim1->GetAnimation());

    if (animSequence->IsBlended())
    {
        std::vector<glm::fdualquat> transformsTmp;
        transformsTmp = transforms;

        auto anim2 = animSequence->GetSecond();
        if (anim2)
        {
            TransformBonesDualQuat(transforms, anim2->GetTime(), anim2->GetAnimation());

            float factor = animSequence->GetFactor();
            for (int i = 0; i < transforms.size(); i++)
            {
                transforms[i] = transforms[i] * (1.0f - factor) + transformsTmp[i] * factor;
            }
        }
    }
}

void Skeleton::GetBoneTransformationMat4(glm::mat4& transform, float animationTime, const Bone& parentBone, Bone& bone, const Animation* animation)
{
    glm::mat4 boneTransformation = bone.DefaultTransformation;

    KeyFrame first, second;
    animation->GetBoneKeyFrames(first, second, bone.Name, animationTime);

    if (first.Time > -1.0 && second.Time > -1.0)
    {
        float deltaTime = second.Time - first.Time;
        float factor = ((animationTime - first.Time) < 0 ? 0 : (animationTime - first.Time)) / deltaTime;

        //Interpolating values

        // Scalling calculation
        glm::vec3 interpolatedScalling = first.Scaling + factor * (second.Scaling - first.Scaling);
        glm::mat4 scallingMat = glm::scale(glm::mat4(1.0f), interpolatedScalling);

        // Rotation calculation
        glm::quat interpolatedRotation = glm::lerp(first.Rotation, second.Rotation, factor);
        interpolatedRotation = glm::normalize(interpolatedRotation);
        glm::mat4 rotationMat = glm::toMat4(interpolatedRotation);

        // Translation calculation
        glm::vec3 interpolatedTranslation = first.Translation + factor * (second.Translation - first.Translation);
        glm::mat4 translationgMat = glm::translate(glm::mat4(1.0f), interpolatedTranslation);

        boneTransformation = translationgMat * rotationMat;// *scallingMat;
    }
    else if (first.Time > -1.0)
    {
        boneTransformation = glm::translate(glm::mat4(1.0f), first.Translation);
        boneTransformation *= glm::toMat4(first.Rotation);
        //boneTransformation *= glm::scale(glm::mat4(1.0f), first.Scaling);
    }

    bone.Transformation = parentBone.Transformation * boneTransformation;

    transform = m_GlobalInverseTransform * bone.Transformation * bone.Offset;
}

void Skeleton::GetBoneTransformationDualQuat(glm::fdualquat& transform, float animationTime, const Bone& parentBone, Bone& bone, const Animation* animation)
{
    glm::fdualquat boneTransformation = bone.DefaultTransformationDQ;

    KeyFrame first, second;
    animation->GetBoneKeyFrames(first, second, bone.Name, animationTime);

    if (first.Time > -1.0 && second.Time > -1.0)
    {
        float deltaTime = second.Time - first.Time;
        float factor = ((animationTime - first.Time) < 0 ? 0 : (animationTime - first.Time)) / deltaTime;

        //Interpolating values
        //boneTransformation = glm::lerp(first.DQ, second.DQ, factor);

        // Rotation calculation
        glm::quat interpolatedRotation = glm::lerp(first.Rotation, second.Rotation, factor);
        interpolatedRotation = glm::normalize(interpolatedRotation);

        // Translation calculation
        glm::vec3 interpolatedTranslation = first.Translation + factor * (second.Translation - first.Translation);

        boneTransformation = glm::fdualquat( interpolatedRotation, interpolatedTranslation);
    }
    else if (first.Time > -1.0)
    {
        boneTransformation = glm::fdualquat(first.Rotation, first.Translation);
        //boneTransformation = first.DQ;
    }

    bone.TransformationDQ = parentBone.TransformationDQ * boneTransformation;

    transform = m_GlobalInverseTransformDQ * bone.TransformationDQ* bone.OffsetDQ;
}

void Skeleton::GetTPoseTranformation(std::vector<glm::mat4>& transforms)
{
    for (unsigned int i = 0; i < m_Bones.size(); i++)
        transforms[i] = glm::mat4(1.0f);
}
