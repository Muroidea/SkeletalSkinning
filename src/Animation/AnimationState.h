#ifndef ANIMATIONSTATE_H
#define ANIMATIONSTATE_H

#include "Animation.h"

class AnimationState
{
private:
	Animation* m_Animation;

	bool m_Enabled;
	bool m_Loop;

	float m_LocalTime; // in ticks
	float m_TimeScale;

public:
	AnimationState(Animation* animation);
	~AnimationState();

	void SetAnimation(Animation *animation);
	Animation* GetAnimation() const;

	void SetEnabled(bool enabled);
	bool GetEnabled() const;

	void SetLoop(bool enabled);
	bool GetLoop() const;

	bool HasEnded() const;
	void AddTime(float deltaTime); // in secons

	void SetTime(float ticks); // in ticks 
	float GetTime() const;

	void SetScale(float scale);
	float GetScale() const;

	void RenderGUI();
};

#endif // !ANIMATIONSTATE_H