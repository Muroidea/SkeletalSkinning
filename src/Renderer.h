//
// Created by Rafał on 28.11.2020.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "Camera.h"
#include "Model.h"
#include "GameObject.h"
#include "BasicOpenGL/Shader.h"
#include "BasicOpenGL/VertexArray.h"
#include "BasicOpenGL/Buffer.h"
#include "Animation/Animation.h"

class Renderer
{
private:
    Shader *m_ShaderModel;
    Buffer *m_PerModelData;

    std::vector<glm::mat4> m_AnimationTransforms;
    std::vector<glm::fdualquat> m_AnimationTransformsDQ;

    std::vector<glm::mat3x4> m_AnimationTransformsCRSMat;
    std::vector<glm::quat> m_AnimationTransformsCRSQuat;

    int divider = 0;
    double counterLBS = 0.0;
    double counterDQS = 0.0;
    double counterCRS = 0.0;

public:
    Renderer();
    ~Renderer();

    bool Init();
    void Render(GameObject *rootNode, double deltaTime);
    void RenderRecursive(GameObject* node, double deltaTime);
    void Reset();

    void Resize(unsigned int width, unsigned int height);
    void UpdateCamera(Camera *camera);
};

#endif //RENDERER_H
