#include "AnimationSequence.h"

AnimationSequence::AnimationSequence()
	: m_CurrentAnimation(0), m_Loop(false), m_Finished(false)
{
}

void AnimationSequence::AddAnimation(AnimationState* animation)
{
	m_AnimationSequence.push_back(animation);
}

void AnimationSequence::AddTime(float deltaTime)
{
	if (!m_Enabled) return;

	if (m_AnimationSequence.size() > 0 && m_CurrentAnimation < m_AnimationSequence.size())
	{
		m_AnimationSequence[m_CurrentAnimation]->AddTime(deltaTime);

		float time = m_AnimationSequence[m_CurrentAnimation]->GetDuration() - m_AnimationSequence[m_CurrentAnimation]->m_CutEnd - m_AnimationSequence[m_CurrentAnimation]->GetTime();
		if (time < m_AnimationSequence[m_CurrentAnimation]->m_BlendLength &&
			m_AnimationSequence.size() > m_CurrentAnimation + 1)
		{
			m_AnimationSequence[m_CurrentAnimation + 1]->AddTime(deltaTime);
			m_Blend = true;
			m_Factor = (time < 0) ? 0.0f : time / m_AnimationSequence[m_CurrentAnimation]->m_BlendLength;			
		}
		else
		{
			m_Blend = false;
		}

		if (time <= 0.1f)
		{
			m_CurrentAnimation++;
			if (m_CurrentAnimation >= m_AnimationSequence.size() && m_Loop)
			{
				m_CurrentAnimation = 0;
				for (auto i : m_AnimationSequence)
				{
					i->SetEnabled(true);
					i->SetTime(i->m_CutBegin);
				}
			}
			else if (m_CurrentAnimation >= m_AnimationSequence.size())
			{
				m_Enabled = false;
				m_Finished = true;
			}
		}	
	}
}

AnimationState* AnimationSequence::GetFirst() const
{
	return m_AnimationSequence[m_CurrentAnimation];
}

AnimationState* AnimationSequence::GetSecond() const
{
	return (m_AnimationSequence.size() > m_CurrentAnimation + 1) ? m_AnimationSequence[m_CurrentAnimation + 1] : nullptr;
}

void AnimationSequence::SetEnabled(bool enabled, bool reset)
{
	m_Enabled = enabled;
	reset |= (m_CurrentAnimation >= m_AnimationSequence.size());

	if (reset)
		m_CurrentAnimation = 0;

	for (auto i : m_AnimationSequence)
	{
		i->SetEnabled(enabled);
		if (reset)
			i->SetTime(i->m_CutBegin);
	}
}

void AnimationSequence::SetLoop(bool loop)
{
	m_Loop = loop;
}

void AnimationSequence::SetSkinningType(SkinningType type)
{
	m_SkinningType = type;
}
