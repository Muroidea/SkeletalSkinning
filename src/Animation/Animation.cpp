#include "Animation.h"

#include <iostream>

#include <glm/gtx/quaternion.hpp>

Animation::Animation(std::string name)
    : m_Name(name)
{
}

Animation::~Animation()
{
}

void Animation::LoadAnimation(const aiAnimation* animation)
{
    aiNodeAnim* nodeAnim;
    std::string boneName;

    m_Duration = animation->mDuration;
    m_TickPerSecond = animation->mTicksPerSecond;

    for (int i = 0; i < animation->mNumChannels; i++)
    {
        nodeAnim = animation->mChannels[i];
        boneName = nodeAnim->mNodeName.C_Str();

        if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
            m_BoneMapping[boneName] = std::vector<KeyFrame>();
        else
            assert(0);

        std::vector<KeyFrame>& keyFrame = m_BoneMapping[boneName];

        for (int j = 0; j < nodeAnim->mNumPositionKeys; j++)
        {
            keyFrame.push_back(KeyFrame());

            keyFrame[j].Time = nodeAnim->mPositionKeys[j].mTime;

            keyFrame[j].Translation = glm::vec3(
                nodeAnim->mPositionKeys[j].mValue.x,
                nodeAnim->mPositionKeys[j].mValue.y,
                nodeAnim->mPositionKeys[j].mValue.z);
            
            keyFrame[j].Rotation = glm::quat(
                nodeAnim->mRotationKeys[j].mValue.w,
                nodeAnim->mRotationKeys[j].mValue.x,
                nodeAnim->mRotationKeys[j].mValue.y,
                nodeAnim->mRotationKeys[j].mValue.z);

            keyFrame[j].Scaling = glm::vec3(
                nodeAnim->mScalingKeys[j].mValue.x,
                nodeAnim->mScalingKeys[j].mValue.y,
                nodeAnim->mScalingKeys[j].mValue.z);

            keyFrame[j].DQ = glm::fdualquat(
                keyFrame[j].Rotation,
                keyFrame[j].Translation);
        }
    }
}

void Animation::GetBoneKeyFrames(KeyFrame& first, KeyFrame& second, const std::string& boneName, double animationTime) const
{
    auto& boneKeyFramesIt = m_BoneMapping.find(boneName);
    if (boneKeyFramesIt == m_BoneMapping.end())
        return;

    if (boneKeyFramesIt->second.size() == 1)
        first = boneKeyFramesIt->second[0];
    else
    {
        first = boneKeyFramesIt->second[animationTime];
        second = boneKeyFramesIt->second[animationTime + 1];
    }
}
