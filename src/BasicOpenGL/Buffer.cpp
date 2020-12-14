//
// Created by Rafał on 28.11.2020.
//

#include "Buffer.h"

Buffer::Buffer(unsigned int elementCount, size_t size, const void *data, GLenum bufferType, GLenum usage)
        : m_ElementCount(elementCount), m_ElementSize(size), m_BufferType(bufferType)
{
    Init(data, usage);
}

Buffer::Buffer(size_t size, const void* data, GLenum bufferType, GLenum usage)
    : m_ElementCount(1), m_ElementSize(size), m_BufferType(bufferType)
{
    Init(data, usage);
}

void Buffer::Init(const void* data, GLenum usage)
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

void Buffer::UploadData(const void* data, unsigned int offsetCount, unsigned int elementCount)
{
    Bind();
    glBufferSubData(m_BufferType, offsetCount * m_ElementSize, elementCount * m_ElementSize, data);
    Unbind();
}

void Buffer::UploadDataInBytes(const void* data, unsigned int offset, unsigned int size)
{
    Bind();
    glBufferSubData(m_BufferType, offset, size, data);
    Unbind();
}

void Buffer::SetBufferBindingID(unsigned int binding)
{
    glBindBufferBase(m_BufferType, binding, m_ID);
}
