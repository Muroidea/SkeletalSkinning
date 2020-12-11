//
// Created by Rafał on 28.11.2020.
//

#include "Buffer.h"

Buffer::Buffer(unsigned int elementCount, size_t elementSize, const void *data, GLenum bufferType, GLenum usage)
        : m_ElementCount(elementCount), m_ElementSize(elementSize), m_BufferType(bufferType)
{
    glGenBuffers(1, &m_ID);
    Bind();
    glBufferData(m_BufferType, m_ElementCount * m_ElementSize, data, usage);
    Unbind();
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &m_ID);
}

void Buffer::Bind() const
{
    glBindBuffer(m_BufferType, m_ID);
}

void Buffer::Unbind() const
{
    glBindBuffer(m_BufferType, 0);
}

unsigned int Buffer::GetCount() const
{
    return m_ElementCount;
}

void Buffer::UploadData(unsigned int offset, unsigned int elementCount, const void *data)
{
    Bind();
    glBufferSubData(m_BufferType, offset, elementCount * m_ElementSize, data);

    Unbind();
}
