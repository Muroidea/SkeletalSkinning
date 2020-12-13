#include "AnimationState.h"

AnimationState::AnimationState(Animation* animation)
{
    m_Animation = animation;
    m_Enabled = false;
    m_Loop = false;
    m_LocalTime = 0.0f;
    m_TimeScale = 1.0f;
}

AnimationState::~AnimationState()
{
    m_Animation = nullptr;
}

void AnimationState::SetAnimation(Animation* animation)
{
    m_Animation = animation;
    m_Enabled = false;
    m_Loop = false;
    m_LocalTime = 0.0f;
}

Animation* AnimationState::GetAnimation() const
{
    return m_Animation;
}

void AnimationState::SetEnabled(bool enabled)
{
    m_Enabled = enabled;
}

bool AnimationState::GetEnabled() const
{
    return m_Enabled;
}

void AnimationState::SetLoop(bool enabled)
{
    m_Loop = enabled;
}

bool AnimationState::GetLoop() const
{
    return m_Loop;
}

bool AnimationState::HasEnded() const
{
    if (m_Animation->GetDuration() <= m_LocalTime && !m_Loop)
        return true;
}

void AnimationState::AddTime(float deltaTime)
{
    if (!m_Enabled) return;
    // if m_Scale < 0.0f than reverse

    m_LocalTime += deltaTime * m_TimeScale * m_Animation->GetTicksPerSecond();

    if (m_Animation->GetDuration() <= m_LocalTime && !m_Loop)
    {
        m_Enabled = false;
        m_LocalTime = m_Animation->GetDuration();
    }
    else if (m_Animation->GetDuration() <= m_LocalTime && m_Loop)
        m_LocalTime = 0.0f;
}

void AnimationState::SetTime(float time)
{
    m_LocalTime = time;
}

float AnimationState::GetTime() const
{
    return (m_LocalTime - 0.01f < 0) ? m_LocalTime : m_LocalTime - 0.01f;
}

void AnimationState::SetScale(float scale)
{
    m_TimeScale = scale;
}

float AnimationState::GetScale() const
{
    return m_TimeScale;
}