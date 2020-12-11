#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Animation.h"

struct Bone
{
    std::string Name;
    unsigned int ParentID;
    glm::mat4 Offset;
    glm::mat4 Transformation;
    glm::mat4 DefaultTransformation;

    Bone() : ParentID(0), Offset(1.0f), Transformation(1.0f), DefaultTransformation(1.0f) { }
};

class Skeleton
{
public:
    unsigned int m_NumBones = 0;
    std::vector<Bone> m_Bones;

    glm::mat4 m_GlobalInverseTransform;

public:
    void TransformBones(std::vector<glm::mat4>& transforms, float time, const Animation* animation);
    void GetTPoseTranformation(std::vector<glm::mat4>& transforms);

private:
    void GetBoneTransformation(glm::mat4& transform, float animationTime, const Bone& parentBone, Bone& bone, const Animation* animation);
};

#endif // !SKELETON_H
