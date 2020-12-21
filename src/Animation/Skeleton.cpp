#include "Skeleton.h"

void Bone::SetOffset(glm::mat4 offset)
{
    Offset = offset;

    //glm::mat3x3 rot(offset);
    //glm::vec3 tran(offset[3]);
    //OffsetDQ = glm::fdualquat(glm::quat(rot), tran);//glm::dualquat_cast(glm::mat3x4(offset));
    
    OffsetDQ = glm::normalize(glm::fdualquat(glm::normalize(glm::quat_cast(offset)), glm::vec3(offset[3])));
    if (OffsetDQ.dual.w == -0)
        OffsetDQ.dual.w = 0;
}

void Bone::SetDefaultTransformation(glm::mat4 defaultTransformation)
{
    DefaultTransformation = defaultTransformation;

    //glm::mat3x3 rot(defaultTransformation);
    //glm::vec3 tran(defaultTransformation[3]);
    //DefaultTransformationDQ = glm::fdualquat(glm::quat(rot), tran);//glm::dualquat_cast(glm::mat3x4(defaultTransformation));

    DefaultTransformationDQ = glm::normalize(glm::fdualquat(glm::normalize(glm::quat_cast(defaultTransformation)), glm::vec3(defaultTransformation[3])));
    if (DefaultTransformationDQ.dual.w == -0)
        DefaultTransformationDQ.dual.w = 0;
}

void Skeleton::SetGlobalInverseMatrix(glm::mat4 globalInverseMatrix)
{
    m_GlobalInverseTransform = globalInverseMatrix;

    glm::mat3x3 rot(globalInverseMatrix);
    glm::vec3 tran(globalInverseMatrix[3]);
    m_GlobalInverseTransformDQ = glm::fdualquat(glm::quat(rot), tran);//glm::dualquat_cast(glm::mat3x4(globalInverseMatrix));

    m_GlobalInverseTransformDQ = glm::normalize(glm::fdualquat(glm::normalize(glm::quat_cast(globalInverseMatrix)), glm::vec3(globalInverseMatrix[3])));
    if (m_GlobalInverseTransformDQ.dual.w == -0)
        m_GlobalInverseTransformDQ.dual.w = 0;
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

void Skeleton::GetBoneTransformationMat4(glm::mat4& transform, float animationTime, const Bone& parentBone, Bone& bone, const Animation* animation)
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
        boneTransformation = glm::translate(glm::mat4(1.0f), first.Translation);
        boneTransformation *= glm::toMat4(first.Rotation);
        boneTransformation *= glm::scale(glm::mat4(1.0f), first.Scaling);
    }

    bone.Transformation = parentBone.Transformation * boneTransformation;

    transform = m_GlobalInverseTransform * bone.Transformation * bone.Offset;
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

void Skeleton::GetBoneTransformationDualQuat(glm::fdualquat& transform, float animationTime, const Bone& parentBone, Bone& bone, const Animation* animation)
{
    glm::fdualquat boneTransformation = bone.DefaultTransformationDQ;

    KeyFrame first, second;
    animation->GetBoneKeyFrames(first, second, bone.Name, animationTime);

    if (first.Time > -1.0 && second.Time > -1.0)
    {
        float deltaTime = second.Time - first.Time;
        float factor = (animationTime - first.Time) / deltaTime;

        //Interpolating values
        boneTransformation = glm::lerp(first.DQ, second.DQ, factor);
    }
    else if (first.Time > -1.0)
    {
        boneTransformation = first.DQ;
    }

    bone.TransformationDQ = parentBone.TransformationDQ * boneTransformation;

    transform = m_GlobalInverseTransformDQ* bone.TransformationDQ* bone.OffsetDQ;
}

void Skeleton::GetTPoseTranformation(std::vector<glm::mat4>& transforms)
{
    for (unsigned int i = 0; i < m_Bones.size(); i++)
        transforms[i] = glm::mat4(1.0f);
}