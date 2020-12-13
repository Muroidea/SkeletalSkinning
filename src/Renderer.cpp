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

void Renderer::Render(GameObject *rootNode)
{
    m_ShaderModel->Bind();

    auto children = rootNode->GetChildren();
    for (int i = 0; i < rootNode->GetNumChildren(); i++)
    {
        RenderRecursive(children[i]);
    }
}

void Renderer::RenderRecursive(GameObject *node)
{
    if (!node->GetEnabled()) return;

    if (node->m_Model)
    {
        m_ShaderModel->SetUniformMat4("Model", node->GetGlobalMatrix());
        m_ShaderModel->SetUniformMat4("Bones", *node->m_AnimationState->GetTranforms().data(), node->m_AnimationState->GetTranforms().size());
        node->m_Model->Render();
    }

    auto children = node->GetChildren();
    for (int i = 0; i < node->GetNumChildren(); i++)
    {
        RenderRecursive(children[i]);
    }
}

void Renderer::Resize(unsigned int width, unsigned int height)
{
    m_ShaderModel->Bind();
    m_ShaderModel->SetUniformMat4("Projection", glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 10000.0f));
    m_ShaderModel->Unbind();
}

void Renderer::Update(GameObject *rootNode, double deltaTime)
{
    auto children = rootNode->GetChildren();
    for (int i = 0; i < rootNode->GetNumChildren(); i++)
    {
        UpdateRecursive(children[i], deltaTime);
    }
}

void Renderer::UpdateRecursive(GameObject* node, double deltaTime)
{
    if (!node->GetEnabled() || !node->m_Model) return;
    auto& animState = node->m_AnimationState;

    animState->AddTime(deltaTime);

    if (animState->GetAnimation() && animState->IsDirty())
    {
        //auto t1 = std::chrono::high_resolution_clock::now();
        node->m_Model->m_Skeleton.TransformBones(animState->GetTranforms(), animState->GetTime(), animState->GetAnimation());
        /*auto t2 = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

        std::cout << duration << "\n";*/
    }
    else if (animState->IsDirty())
    {
        node->m_Model->m_Skeleton.GetTPoseTranformation(animState->GetTranforms());
    }

    auto children = node->GetChildren();
    for (int i = 0; i < node->GetNumChildren(); i++)
    {
        UpdateRecursive(children[i], deltaTime);
    }
}

void Renderer::UpdateCamera(Camera *camera)
{
    m_ShaderModel->Bind();
    m_ShaderModel->SetUniformMat4("View", camera->GetViewMatrix());
    m_ShaderModel->Unbind();
}
