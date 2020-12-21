//
// Created by Rafik on 28.11.2020.
//

#include "Renderer.h"
#include <chrono>

#include <glm/gtx/string_cast.hpp>

Renderer::Renderer()
{
    m_ShaderModel = nullptr;
    m_PerModelData = nullptr;
}

Renderer::~Renderer()
{

}

bool Renderer::Init()
{
    m_ShaderModel = new Shader("../resources/shaders/VertexShaderModel.vs", "../resources/shaders/FragmentShaderModel.fs");
    m_PerModelData = new Buffer(101 * sizeof(glm::mat4) + 100 * sizeof(glm::fdualquat), nullptr, GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW);

    m_ShaderModel->SetBlockBinding(0, "PerModelData");
    m_PerModelData->SetBufferBindingID(0);

    for (int i = 0; i < 100; i++)
    {
        m_AnimationTransforms.push_back(glm::mat4(1.0f));
        m_AnimationTransformsDQ.push_back(glm::dual_quat_identity<float, glm::qualifier::highp>());
    }

    m_PerModelData->UploadDataInBytes(m_AnimationTransforms.data(), 0, 100 * sizeof(glm::mat4));
    m_PerModelData->UploadDataInBytes(m_AnimationTransformsDQ.data(), 100 * sizeof(glm::mat4), 100 * sizeof(glm::fdualquat));

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

        if (animState->GetAnimation())
        {
            animState->AddTime(deltaTime);

            if (animState->GetSkinningType() == SkinningType::LINEAR_BLEND_SKINNING)
            {
                m_ShaderModel->SetUniform1i("SkinningMode", 0);
                node->m_Model->m_Skeleton.TransformBonesMat4(m_AnimationTransforms, animState->GetTime(), animState->GetAnimation());
                m_PerModelData->UploadDataInBytes(m_AnimationTransforms.data(), 0, node->m_Model->GetNumBones() * sizeof(glm::mat4));
            }
            else // DUAL_QUATERNION_SKINNING || CENTER_OF_ROTATION_SKINNING
            {
                m_ShaderModel->SetUniform1i("SkinningMode", 1);
                node->m_Model->m_Skeleton.TransformBonesDualQuat(m_AnimationTransformsDQ, animState->GetTime(), animState->GetAnimation());
                m_PerModelData->UploadDataInBytes(m_AnimationTransformsDQ.data(), 100 * sizeof(glm::mat4), node->m_Model->GetNumBones() * sizeof(glm::fdualquat));
            }
        }
        else
        {
            m_ShaderModel->SetUniform1i("SkinningMode", 0);
            node->m_Model->m_Skeleton.GetTPoseTranformation(m_AnimationTransforms);
            m_PerModelData->UploadDataInBytes(m_AnimationTransforms.data(), 0, node->m_Model->GetNumBones() * sizeof(glm::mat4));
        }

        m_PerModelData->UploadDataInBytes(&node->GetGlobalMatrix()[0][0], 100 * sizeof(glm::mat4) + 100 * sizeof(glm::fdualquat), sizeof(glm::mat4));

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
    if (width <= 1.0f || height <= 1.0f) return;

    m_ShaderModel->Bind();
    m_ShaderModel->SetUniformMat4("Projection", glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 10000.0f));
    m_ShaderModel->Unbind();
}

void Renderer::UpdateCamera(Camera *camera)
{
    m_ShaderModel->Bind();
    auto tmp = camera->GetViewMatrix();
    m_ShaderModel->SetUniformMat4("View", tmp);
    m_ShaderModel->Unbind();
}
