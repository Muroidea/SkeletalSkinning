#ifndef ANIMATIONSTATE_H
#define ANIMATIONSTATE_H

#include "Animation.h"

enum class SkinningType { LINEAR_BLEND_SKINNING, DUAL_QUATERNION_SKINNING, CENTERS_OF_ROTATION_SKINNING };

class AnimationState
{
private:
	Animation* m_Animation;

	bool m_Enabled;
	bool m_Loop;

	float m_LocalTime; // in ticks
	float m_TimeScale;

	SkinningType m_SkinningType;

public:
	AnimationState(Animation* animation);
	~AnimationState();

	void SetAnimation(Animation *animation);
	Animation* GetAnimation() const;

	void SetEnabled(bool enabled);
	bool GetEnabled() const;

	void SetLoop(bool enabled);
	bool GetLoop() const;

	void AddTime(float deltaTime); // in secons
	bool HasEnded() const;

	void SetTime(float ticks); // in ticks 
	float GetTime() const;

	void SetScale(float scale);
	float GetScale() const;

	void SetSkinningType(SkinningType type);
	SkinningType GetSkinningType() const;

	friend class GameObjectGUI;
};

#endif // !ANIMATIONSTATE_H