//
// Created by Rafał on 28.11.2020.
//

#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <glad/glad.h>
#include "Buffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
    unsigned int m_ID;


public:
    VertexArray();
    ~VertexArray();

    void BindVertexBuffer(const Buffer &VBO, const VertexBufferLayout &VBL);
    void BindIndexBuffer(const Buffer &IBO);

    void Bind();
    void Unbind();
};


#endif //VERTEXARRAY_H
