#ifndef ANIMATIONSEQUENCE_H
#define ANIMATIONSEQUENCE_H

#include "AnimationState.h"

class AnimationSequence
{
private:
	std::vector<AnimationState*> m_AnimationSequence;
	int m_CurrentAnimation;

	bool m_Loop;
	bool m_Enabled;

	bool m_Blend;
	bool m_Finished;
	float m_Factor;

	SkinningType m_SkinningType;

public:
	AnimationSequence();

	void AddAnimation(AnimationState* animation);
	void AddTime(float deltaTime); // in seconds

	AnimationState* GetFirst() const;
	AnimationState* GetSecond() const;
	
	inline bool IsEnabled() const { return m_Enabled; }
	void SetEnabled(bool enabled, bool reset = false);

	void SetLoop(bool loop);

	inline bool IsBlended() const { return m_Blend; }

	inline float GetFactor() const { return m_Factor; }

	void SetSkinningType(SkinningType type);
	inline SkinningType GetSkinningType() const { return m_SkinningType; }

	inline int GetNumberOfAnimation() const { return m_AnimationSequence.size(); }
	inline bool IsFinished() const { return m_Finished; }
};

#endif // !ANIMATIONSEQUENCE_H