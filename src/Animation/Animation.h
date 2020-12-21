//
// Created by Rafa³ on 02.12.2020.
//

#ifndef ANIMATION_H
#define ANIMATION_H

#include <unordered_map>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/dual_quaternion.hpp>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct KeyFrame
{
	double Time = -1.0;
	glm::vec3 Translation;
	glm::quat Rotation;
	glm::vec3 Scaling;

	glm::dualquat DQ;
};

class Animation
{
private:
	std::string m_Name;
	std::unordered_map<std::string, std::vector<KeyFrame>> m_BoneMapping;

	float m_Duration;
	float m_TickPerSecond;

public:
	Animation() = default;
	Animation(std::string name);
	~Animation();

	void LoadAnimation(const aiAnimation* animation);

	void GetBoneKeyFrames(KeyFrame& first, KeyFrame& second, const std::string& boneName, double animationTime) const;

	inline std::string GetName() const { return m_Name; }
	inline float GetDuration() const { return m_Duration; }
	inline float GetTicksPerSecond() const { return m_TickPerSecond; }
};

#endif // !ANIMATION_H



