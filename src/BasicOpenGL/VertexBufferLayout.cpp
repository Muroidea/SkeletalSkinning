//
// Created by Rafik on 30.11.2020.
//

#include "VertexBufferLayout.h"

unsigned int GetVertexTypeSize(VertexType vertexType)
{
    switch (vertexType)
    {
        case VertexType::BOOL    :	return sizeof(bool);
        case VertexType::INTEGER :	return sizeof(int);
        case VertexType::FLOAT   :	return sizeof(float);
        case VertexType::VEC2I   :	return sizeof(int)   * 2;
        case VertexType::VEC3I   :	return sizeof(int)   * 3;
        case VertexType::VEC4I   :	return sizeof(int)   * 4;
        case VertexType::VEC2F   :	return sizeof(float) * 2;
        case VertexType::VEC3F   :	return sizeof(float) * 3;
        case VertexType::VEC4F   :	return sizeof(float) * 4;
        case VertexType::MAT3    :	return sizeof(float) * 3 * 3;
        case VertexType::MAT4    :	return sizeof(float) * 4 * 4;
    }

    return 0;
}

unsigned int GetVertexTypeCount(VertexType vertexType)
{
    switch (vertexType)
    {
        case VertexType::BOOL    :	return 1;
        case VertexType::INTEGER :	return 1;
        case VertexType::FLOAT   :	return 1;
        case VertexType::VEC2I   :	return 2;
        case VertexType::VEC3I   :	return 3;
        case VertexType::VEC4I   :	return 4;
        case VertexType::VEC2F   :	return 2;
        case VertexType::VEC3F   :	return 3;
        case VertexType::VEC4F   :	return 4;
        case VertexType::MAT3    :	return 3 * 3;
        case VertexType::MAT4    :	return 4 * 4;
    }

    return 0;
}

GLenum ConvertVertexTypeToOpenGL(VertexType type)
{
    switch (type)
    {
        case VertexType::BOOL    :	return GL_BOOL;
        case VertexType::INTEGER :	return GL_INT;
        case VertexType::VEC2I   :	return GL_INT;
        case VertexType::VEC3I   :	return GL_INT;
        case VertexType::VEC4I   :	return GL_INT;
        case VertexType::FLOAT   :	return GL_FLOAT;
        case VertexType::VEC2F   :	return GL_FLOAT;
        case VertexType::VEC3F   :	return GL_FLOAT;
        case VertexType::VEC4F   :	return GL_FLOAT;
        case VertexType::MAT3    :	return GL_FLOAT;
        case VertexType::MAT4    :	return GL_FLOAT;
    }

    return 0;
}