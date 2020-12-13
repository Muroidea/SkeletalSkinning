#include "GameObject.h"

#include "GUI/GUI.h"
#include <imgui.h>

#include <glm/gtx/euler_angles.hpp>

unsigned int GameObject::s_Counter = 0;

GameObject::GameObject(std::string name, Model* model, AnimationState* animationState)
	: m_Name(name), m_Model(model)
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

	for (int i = 0; i < m_Children.size(); i++)
		delete m_Children[i];
}

void GameObject::AddChild(GameObject* child)
{
	child->SetParent(this);
	m_Children.push_back(child);
}

void GameObject::SetParent(GameObject * parent)
{
	m_Parent = parent;
}

void GameObject::RemoveChild()
{
	m_Children.erase(std::find(m_Children.begin(), m_Children.end(), m_ChildToRemove));
	delete m_ChildToRemove;
	m_ChildToRemove = nullptr;
}

void GameObject::SetToRemove()
{
	m_Parent->m_ChildToRemove = this;
}

void GameObject::SetAnimationState(AnimationState* animationState)
{
	if (m_AnimationState)
		delete m_AnimationState;

	m_AnimationState = animationState;
}

void GameObject::Update(bool dirtyFlag)
{
	if (m_ChildToRemove)
		RemoveChild();

	if (!m_Enabled) return;

	dirtyFlag |= m_DirtyFlag;
	if (dirtyFlag)
	{
		m_Global = GetLocalMatrix();
		m_DirtyFlag = false;
	}

	for (int i = 0; i < m_Children.size(); i++)
		m_Children[i]->Update(dirtyFlag, m_Global);
}

void GameObject::Update(bool dirtyFlag, const glm::mat4& parent)
{
	if (m_ChildToRemove)
		RemoveChild();

	if (!m_Enabled) return;

	dirtyFlag |= m_DirtyFlag;
	if (dirtyFlag)
	{
		m_Global = GetLocalMatrix() * parent;
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
	glm::mat4 transformation = glm::translate(glm::mat4(1.0f), m_LocalPosition);
	transformation *= glm::toMat4(glm::quat(glm::radians(m_LocalRotation)));
	transformation = glm::scale(transformation, m_LocalScale);

	return transformation;
}

glm::mat4 GameObject::GetGlobalMatrix()
{
	return m_Global;
}

glm::vec3 GameObject::GetLocalPosition()
{
	return glm::vec3(m_LocalPosition);
}

void GameObject::SetLocalPosition(glm::vec3 position)
{
	m_LocalPosition = position;
	m_DirtyFlag = true;
}

glm::vec3 GameObject::GetLocalRotation()
{
	return m_LocalRotation;
}

void GameObject::SetLocalRotation(glm::vec3 rotation)
{
	m_LocalRotation = rotation;
	m_DirtyFlag = true;
}

glm::vec3 GameObject::GetLocalScale()
{
	return m_LocalScale;
}

void GameObject::SetLocalScale(glm::vec3 scale)
{
	m_LocalScale = scale;
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
    for (int i = 0; i < m_Children.size(); i++)
    {
		m_Children[i]->DrawTreeGUIRecursive(selectedNode);
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
		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)this, nodeFlags, "%s", m_Name.c_str());

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
		ImGui::TreeNodeEx((void*)(intptr_t)this, nodeFlags, "%s", m_Name.c_str());

		if (ImGui::IsItemClicked())
			selectedNode = this;
	}
}

void GameObject::DrawNodeGUI()
{
	glm::vec3 position = GetLocalPosition();
	glm::vec3 rotation = GetLocalRotation();
	glm::vec3 scale = GetLocalScale();

	float float_low = -1000000.0f, float_high = 1000000.0f;

	char buffer[25]; 
	strcpy(buffer, m_Name.c_str());
	ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.0f);
	if(ImGui::InputText("##Name", buffer, (int)(sizeof(buffer)/sizeof(*buffer))))
		m_Name = buffer;
	ImGui::PopItemWidth();

	ImGui::SameLine();

	if (ImGui::Button("Add child", ImVec2(80.0f, 20.0f)))
		AddChild(new GameObject());

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	ImGui::Checkbox(" Enable/Disable", &m_Enabled); 
		
	ImGui::NewLine();
	ImGui::Text("Local transform: "); ImGui::SameLine(); 
	if (ImGui::Button("Reset", ImVec2(80.0f, 20.0f)))
	{
		SetLocalPosition(glm::vec3(0.0f));
		SetLocalRotation(glm::vec3(0.0f));
		SetLocalScale(glm::vec3(1.0f));
	}
	
	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (GUI::DrawVec3("Position", position))
		SetLocalPosition(position);

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (GUI::DrawVec3("Rotation", rotation))
		SetLocalRotation(rotation);

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (GUI::DrawVec3("Scale", scale))
		SetLocalScale(scale);
}

bool GameObject::operator==(const GameObject& other)
{
    return m_ID == other.m_ID;
}

