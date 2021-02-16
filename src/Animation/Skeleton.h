#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>

#include "AnimationState.h"
#include "AnimationSequence.h"

struct Bone
{
public:
    std::string Name;
    unsigned int ParentID;

    glm::mat4 Offset;
    glm::mat4 DefaultTransformation;
    glm::mat4 Transformation;

    glm::fdualquat OffsetDQ;
    glm::fdualquat DefaultTransformationDQ;
    glm::fdualquat TransformationDQ;

public:
    Bone() : ParentID(0), Offset(1.0f), DefaultTransformation(1.0f), Transformation(1.0f), 
        OffsetDQ(glm::dual_quat_identity<float, glm::qualifier::highp>()), 
        DefaultTransformationDQ(glm::dual_quat_identity<float, glm::qualifier::highp>()), 
        TransformationDQ(glm::dual_quat_identity<float, glm::qualifier::highp>()) { }

    void SetOffset(glm::mat4 offset);
    void SetDefaultTransformation(glm::mat4 defaultTransformation);
};

class Skeleton
{
public:
    unsigned int m_NumBones = 0;
    std::vector<Bone> m_Bones;

    glm::mat4 m_GlobalInverseTransform;
    glm::fdualquat m_GlobalInverseTransformDQ;

public:
    void SetGlobalInverseMatrix(glm::mat4 globalInverseMatrix);

    void TransformBonesMat4(std::vector<glm::mat4>& transforms, float time, const Animation* animation);
    void TransformBonesDualQuat(std::vector<glm::fdualquat>& transforms, float time, const Animation* animation);

    void TransformBonesMat4(std::vector<glm::mat4>& transforms, const AnimationSequence* animSequence);
    void TransformBonesDualQuat(std::vector<glm::fdualquat>& transforms, const AnimationSequence* animSequence);

    void GetTPoseTranformation(std::vector<glm::mat4>& transforms);

private:
    void GetBoneTransformationMat4(glm::mat4& transform, float animationTime, const Bone& parentBone, Bone& bone, const Animation* animation);
    void GetBoneTransformationDualQuat(glm::fdualquat& transform, float animationTime, const Bone& parentBone, Bone& bone, const Animation* animation);

    //void TransformBonesMat4(std::vector<glm::mat4>& transforms, const AnimationState* anim1, const AnimationState* anim2);
    //void TransformBonesDualQuat(std::vector<glm::fdualquat>& transforms, const AnimationState* anim1, const AnimationState* anim2);
};

#endif // !SKELETON_H
