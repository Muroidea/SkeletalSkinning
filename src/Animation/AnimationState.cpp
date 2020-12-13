#include "AnimationState.h"

#include <imgui.h>

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

void AnimationState::RenderGUI()
{
    ImGui::Text("Animation state: "); ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 1));
    if (m_Animation && ImGui::Button("Clear", ImVec2(50.0f, 15.0f)))
        SetAnimation(nullptr);
	ImGui::PopStyleVar(1);

    ImGui::NewLine();

    if (m_Animation)
        ImGui::Button(m_Animation->GetName().c_str(), ImVec2(ImGui::GetWindowContentRegionWidth(), 20.0f));
    else
    {
        ImGui::NewLine();
        ImGui::Button("Animation (empty)", ImVec2(ImGui::GetWindowContentRegionWidth(), 20.0f));
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Animation"))
        {
            IM_ASSERT(payload->DataSize == sizeof(long long));
            long long address = *(const long long*)payload->Data;
            SetAnimation(reinterpret_cast<Animation*>(address));
        }
        ImGui::EndDragDropTarget();
    }

    if (m_Animation)
    {
        ImGui::NewLine();

        ImGui::Checkbox("Enable/Disable", &m_Enabled); ImGui::SameLine();
        ImGui::Checkbox("Play in loop", &m_Loop);

        ImGui::NewLine();

        ImGui::Text("Duration in ticks: %f", m_Animation->GetDuration());
        ImGui::Text("Duration in seconds: %.2f", m_Animation->GetDuration() / m_Animation->GetTicksPerSecond());
        ImGui::Text("Ticks per second: %f", m_Animation->GetTicksPerSecond());

        ImGui::NewLine();

		ImGui::DragFloat("Time scale", &m_TimeScale, 0.1f, 0.1f, 20.0f, "%.1f", ImGuiSliderFlags_None);
		ImGui::SliderFloat("Time in ticks", &m_LocalTime, 0.0f, m_Animation->GetDuration(), "%.1f");
    }
}