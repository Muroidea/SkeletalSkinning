#include "GameObject.h"

#include <imgui.h>

unsigned int GameObject::s_Counter = 0;

GameObject::GameObject(GameObject* parent, Model* model, AnimationState* animationState)
	: m_Parent(parent), m_Model(model)
{
    m_ID = s_Counter;
    s_Counter++;

	if (animationState == nullptr)
		m_AnimationState = new AnimationState(nullptr, (model) ? model->GetNumBones() : 0);
	else
		m_AnimationState = animationState;
}

GameObject::~GameObject()
{
	if (m_AnimationState)
		delete m_AnimationState;
}

void GameObject::AddChild(GameObject* child)
{
	m_Children.push_back(child);
}

void GameObject::SetAnimationState(AnimationState* animationState)
{
	if (m_AnimationState)
		delete m_AnimationState;

	m_AnimationState = animationState;
}

void GameObject::Update(bool dirtyFlag)
{
	if (!m_Enabled) return;

	dirtyFlag |= m_DirtyFlag;
	if (dirtyFlag)
	{
		m_Global = m_Local;
		m_DirtyFlag = false;
	}

	for (int i = 0; i < m_Children.size(); i++)
		m_Children[i]->Update(dirtyFlag, m_Global);
}

void GameObject::Update(bool dirtyFlag, const glm::mat4& parent)
{
	if (!m_Enabled) return;

	dirtyFlag |= m_DirtyFlag;
	if (dirtyFlag)
	{
		m_Global = m_Local * parent;
		m_DirtyFlag = false;
	}

	for (int i = 0; i < m_Children.size(); i++)
		m_Children[i]->Update(dirtyFlag, m_Global);
}

int GameObject::GetNumChildren()
{
	return m_Children.size();
}

const std::vector<GameObject*>& GameObject::GetChildren()
{
	return m_Children;
}

glm::mat4 GameObject::GetLocalMatrix()
{
	return m_Local;
}

glm::mat4 GameObject::GetGlobalMatrix()
{
	return m_Global;
}

void GameObject::SetLocalTransform(glm::mat4 transform)
{
	m_Local = transform;
	m_DirtyFlag = true;
}

glm::vec3 GameObject::GetLocalPosition()
{
	return glm::vec3(m_Local[3]);
}

void GameObject::SetLocalPosition(glm::vec3 position)
{
	m_Local[3][0] = position.x;
	m_Local[3][1] = position.y;
	m_Local[3][2] = position.z;

	m_DirtyFlag = true;
}

glm::quat GameObject::GetLocalRotation()
{
	glm::mat3 rotation = glm::mat3(m_Local);
	glm::vec3 scale = GetLocalScale();

	rotation[0] /= scale.x;
	rotation[1] /= scale.y;
	rotation[2] /= scale.z;

	return glm::normalize(glm::quat_cast(rotation));
}

void GameObject::SetLocalRotation(glm::quat rotation)
{
	rotation = glm::normalize(rotation);
	glm::mat4 rot = glm::toMat4(rotation);
	glm::vec3 scale = GetLocalScale();

	m_Local[0] = rot[0] * scale.x;
	m_Local[1] = rot[1] * scale.y;
	m_Local[2] = rot[2] * scale.z;

	m_DirtyFlag = true;
}

glm::vec3 GameObject::GetLocalScale()
{
	float x = glm::length(m_Local[0]);
	float y = glm::length(m_Local[1]);
	float z = glm::length(m_Local[2]);

	return glm::vec3(x, y, z);
}

void GameObject::SetLocalScale(glm::vec3 localScale)
{
	auto current = GetLocalScale();

	m_Local[0] *= localScale.x / current.x;
	m_Local[1] *= localScale.y / current.y;
	m_Local[2] *= localScale.z / current.z;

	m_DirtyFlag = true;
}

void GameObject::SetEnabled(bool enabled)
{
	m_Enabled = enabled;
	m_DirtyFlag = true;
}

bool GameObject::GetEnabled() const
{
	return m_Enabled;
}

void GameObject::DrawTreeGUIRoot(GameObject *&selectedNode)
{
	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	ImGuiTreeNodeFlags nodeFlags = baseFlags;
	bool isSelected = false;
	if (selectedNode && this)
		isSelected = *selectedNode == *this;

	if (isSelected) nodeFlags |= ImGuiTreeNodeFlags_Selected;

	ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNodeEx((void*)(intptr_t)this, nodeFlags, "RootNode"))
    {
		if (ImGui::IsItemClicked())
			selectedNode = this;

        for (int i = 0; i < m_Children.size(); i++)
        {
			m_Children[i]->DrawTreeGUIRecursive(selectedNode);
        }

        ImGui::TreePop();
    }
}

void GameObject::DrawTreeGUIRecursive(GameObject *&selectedNode)
{
	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	ImGuiTreeNodeFlags nodeFlags = baseFlags;
	bool isSelected = false;

	if (selectedNode && this) isSelected = *selectedNode == *this;
	if (isSelected) nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (GetNumChildren() > 0)
	{
		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)this, nodeFlags, "%s %d", m_Model->GetName().c_str(), m_ID);

		if (ImGui::IsItemClicked())
			selectedNode = this;

		if (nodeOpen)
		{
			for (int i = 0; i < m_Children.size(); i++)
			{
				m_Children[i]->DrawTreeGUIRecursive(selectedNode);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)this, nodeFlags, "%s %d", m_Model->GetName().c_str(), m_ID);

		if (ImGui::IsItemClicked())
			selectedNode = this;
	}
}

void GameObject::DrawNodeGUI()
{
	glm::vec3 position = GetLocalPosition();
	glm::quat rotation = GetLocalRotation();
	glm::vec3 scale = GetLocalScale();

	bool positionChanged = false;
	bool rotationChanged = false;
	bool scaleChanged = false;

	float float_low = -1000000.0f, float_high = 1000000.0f;

	ImGui::Checkbox(" Enable/Disable", &m_Enabled);

	ImGui::NewLine();
	ImGui::Text("Local transform: "); ImGui::SameLine(); 
	if (ImGui::Button("Reset", ImVec2(80.0f, 20.0f)))
		SetLocalTransform(glm::mat4(1.0f));
	
	ImGui::NewLine();
	ImGui::Text("Position: ");

	ImGui::PushID("Position");
	ImGui::DragScalar("X ", ImGuiDataType_Float, &position.x, 1.0f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) positionChanged = true;
	ImGui::DragScalar("Y ", ImGuiDataType_Float, &position.y, 1.0f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) positionChanged = true;
	ImGui::DragScalar("Z ", ImGuiDataType_Float, &position.z, 1.0f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) positionChanged = true;
	ImGui::PopID();


	ImGui::NewLine();
	ImGui::Text("Rotation: ");

	ImGui::PushID("Rotation");
	ImGui::DragScalar("W ", ImGuiDataType_Float, &rotation.w, 0.1f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) rotationChanged = true;
	ImGui::DragScalar("X ", ImGuiDataType_Float, &rotation.x, 0.1f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) rotationChanged = true;
	ImGui::DragScalar("Y ", ImGuiDataType_Float, &rotation.y, 0.1f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) rotationChanged = true;
	ImGui::DragScalar("Z ", ImGuiDataType_Float, &rotation.z, 0.1f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) rotationChanged = true;
	ImGui::PopID();
	

	ImGui::NewLine();
	ImGui::Text("Scale: ");

	ImGui::PushID("Scale");
	ImGui::DragScalar("X ", ImGuiDataType_Float, &scale.x, 0.1f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) scaleChanged = true;
	ImGui::DragScalar("Y ", ImGuiDataType_Float, &scale.y, 0.1f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) scaleChanged = true;
	ImGui::DragScalar("Z ", ImGuiDataType_Float, &scale.z, 0.1f, &float_low, &float_high, "%f");
	if (ImGui::IsItemEdited()) scaleChanged = true;
	ImGui::PopID();

	if (positionChanged)
		SetLocalPosition(position);

	if (rotationChanged)
		SetLocalRotation(rotation);

	if (scaleChanged)
		SetLocalScale(scale);
}

bool GameObject::operator==(const GameObject& other)
{
    return m_ID == other.m_ID;
}

