#ifndef ANIMATIONSTATE_H
#define ANIMATIONSTATE_H

#include "Animation.h"

class AnimationState
{
private:
	Animation* m_Animation;

	bool m_Enabled;
	bool m_Loop;
	bool m_Dirty;

	float m_LocalTime; // in ticks
	float m_TimeScale;

	std::vector<glm::mat4> m_Transforms;
	std::vector<glm::mat4> m_FinalTransforms;

public:
	AnimationState(Animation* animation, int numOfBones);
	~AnimationState();

	void SetAnimation(Animation *animation);
	Animation* GetAnimation() const;

	std::vector<glm::mat4>& GetTranforms();

	void SetEnabled(bool enabled);
	bool GetEnabled() const;

	void SetLoop(bool enabled);
	bool GetLoop() const;

	bool HasEnded() const;
	void SetDirty(bool dirty);
	bool IsDirty();

	void AddTime(float deltaTime); // in secons

	void SetTime(float ticks); // in ticks 
	float GetTime() const;

	void SetScale(float scale);
	float GetScale() const;

	void RenderGUI();
};

#endif // !ANIMATIONSTATE_H