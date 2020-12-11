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
    std::vector<glm::mat4> m_AnimationTransforms;

public:
    Renderer();
    ~Renderer();

    bool Init();
    void Render(GameObject *rootNode); 
    void RenderRecursive(GameObject* node);

    void Update(GameObject* rootNode, double deltaTime);
    void UpdateRecursive(GameObject* node, double deltaTime);

    void Resize(unsigned int width, unsigned int height);
    void UpdateCamera(Camera *camera);
};

#endif //RENDERER_H
