#include "GameObject.h"

unsigned int GameObject::s_Counter = 0;

GameObject::GameObject(std::string name, Model* model, AnimationState* animationState)
	: m_Name(name), m_Model(model)
{
    m_ID = s_Counter;
    s_Counter++;

	if (animationState == nullptr)
		m_AnimationState = new AnimationState(nullptr);
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
	child->m_Parent = this;
	m_Children.push_back(child);
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

bool GameObject::operator==(const GameObject& other)
{
    return m_ID == other.m_ID;
}

