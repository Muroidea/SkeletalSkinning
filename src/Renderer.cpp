//
// Created by Rafik on 28.11.2020.
//

#include "Renderer.h"
#include <chrono>

Renderer::Renderer()
{
    m_ShaderModel = nullptr;
}

Renderer::~Renderer()
{

}

bool Renderer::Init()
{
    m_ShaderModel = new Shader("../resources/shaders/VertexShaderModel.vs", "../resources/shaders/FragmentShaderModel.fs");

    for (int i = 0; i < 100; i++)
        m_AnimationTransforms.push_back(glm::mat4(1.0f));

    m_ShaderModel->SetUniformMat4("Bones", m_AnimationTransforms[0], m_AnimationTransforms.size());
    
    return true;
}

void Renderer::Render(GameObject *rootNode, double deltaTime)
{
    m_ShaderModel->Bind();

    auto children = rootNode->GetChildren();
    for (int i = 0; i < rootNode->GetNumChildren(); i++)
    {
        RenderRecursive(children[i], deltaTime);
    }
}

void Renderer::RenderRecursive(GameObject *node, double deltaTime)
{
    if (!node->GetEnabled()) return;

    if (node->m_Model)
    {
		auto& animState = node->m_AnimationState;
		animState->AddTime(deltaTime);

		if (animState->GetAnimation())
			node->m_Model->m_Skeleton.TransformBones(m_AnimationTransforms, animState->GetTime(), animState->GetAnimation());
		else
			node->m_Model->m_Skeleton.GetTPoseTranformation(m_AnimationTransforms);

        m_ShaderModel->SetUniformMat4("Model", node->GetGlobalMatrix());
        m_ShaderModel->SetUniformMat4("Bones", *m_AnimationTransforms.data(), node->m_Model->GetNumBones());
        node->m_Model->Render();
    }

    auto children = node->GetChildren();
    for (int i = 0; i < node->GetNumChildren(); i++)
    {
        RenderRecursive(children[i], deltaTime);
    }
}

void Renderer::Resize(unsigned int width, unsigned int height)
{
    m_ShaderModel->Bind();
    m_ShaderModel->SetUniformMat4("Projection", glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 10000.0f));
    m_ShaderModel->Unbind();
}

void Renderer::UpdateCamera(Camera *camera)
{
    m_ShaderModel->Bind();
    m_ShaderModel->SetUniformMat4("View", camera->GetViewMatrix());
    m_ShaderModel->Unbind();
}
