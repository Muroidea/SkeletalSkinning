//
// Created by Rafał on 28.11.2020.
//

#ifndef VERTEXBUFFERLAYOUT_H
#define VERTEXBUFFERLAYOUT_H

#include <glad/glad.h>
#include <vector>

enum class VertexType
{
    BOOL, INTEGER, FLOAT,
    VEC2I, VEC3I, VEC4I,
    VEC2F, VEC3F, VEC4F,
    MAT3, MAT4
};

unsigned int GetVertexTypeSize(VertexType vertexType);
unsigned int GetVertexTypeCount(VertexType vertexType);
GLenum ConvertVertexTypeToOpenGL(VertexType type);

struct VertexBufferElement
{
    VertexType type;
    bool normalized;
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Attributes;
    unsigned int m_Stride;

public:
    VertexBufferLayout() : m_Stride(0) {};
    ~VertexBufferLayout() = default;

    void PushElement(VertexType type, bool normalized = false)
    {
        m_Attributes.push_back( {type, normalized} );
        m_Stride += GetVertexTypeSize(type);
    }

    inline const std::vector<VertexBufferElement> GetAttributes() const& { return m_Attributes; };
    inline unsigned int GetStride() const { return m_Stride; };
};

#endif //VERTEXBUFFERLAYOUT_H
