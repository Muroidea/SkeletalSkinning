//
// Created by Rafał on 28.11.2020.
//

#include "VertexArray.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::BindVertexBuffer(const Buffer &VBO, const VertexBufferLayout &VBL)
{
    Bind();
    VBO.Bind();

    auto attribute = VBL.GetAttributes();
    unsigned int m = 0;
    for(int i = 0; i < attribute.size(); i++)
    {
        if (ConvertVertexTypeToOpenGL(attribute[i].type) == GL_FLOAT)
        {
            glVertexAttribPointer(i, GetVertexTypeCount(attribute[i].type), ConvertVertexTypeToOpenGL(attribute[i].type),
                attribute[i].normalized, VBL.GetStride(), (const void*)(m));
        }
        else if (ConvertVertexTypeToOpenGL(attribute[i].type) == GL_INT)
        {
            glVertexAttribIPointer(i, GetVertexTypeCount(attribute[i].type), ConvertVertexTypeToOpenGL(attribute[i].type),
                VBL.GetStride(), (const void*)(m));
        }

        glEnableVertexAttribArray(i);
        m += GetVertexTypeSize(attribute[i].type);
    }
}

void VertexArray::BindIndexBuffer(const Buffer &IBO)
{
    Bind();
    IBO.Bind();
}

void VertexArray::Bind()
{
    glBindVertexArray(m_ID);
}

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}


