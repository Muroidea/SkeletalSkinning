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


public:
    Renderer();
    ~Renderer();

    bool Init();
    void Render(GameObject *rootNode, double deltaTime);
    void RenderRecursive(GameObject* node, double deltaTime);

    void Resize(unsigned int width, unsigned int height);
    void UpdateCamera(Camera *camera);
};

#endif //RENDERER_H
