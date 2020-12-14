//
// Created by Rafał on 28.11.2020.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>

class Buffer
{
private:
    unsigned int m_ID;

    unsigned int m_ElementCount;
    size_t m_ElementSize;

    const GLenum m_BufferType;

public:
    Buffer(unsigned int elementCount, size_t size, const void *data, GLenum bufferType, GLenum usage = GL_STATIC_DRAW);
    Buffer(size_t size, const void* data, GLenum bufferType, GLenum usage = GL_STATIC_DRAW);
    ~Buffer();

    void Bind() const;
    void Unbind() const;

    void UploadData(const void* data, unsigned int offset = 0, unsigned int elementCount = 1);
    void UploadDataInBytes(const void* data, unsigned int offset, unsigned int size);

    void SetBufferBindingID(unsigned int binding);

    unsigned int GetCount() const;

private:
    void Init(const void* data, GLenum usage);
};


#endif // !BUFFER_H
