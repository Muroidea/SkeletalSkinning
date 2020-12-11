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
    Buffer(unsigned int elementCount, size_t elementSize, const void *data, GLenum bufferType, GLenum usage = GL_STATIC_DRAW);
    ~Buffer();
    void Bind() const;
    void Unbind() const;

    void UploadData(unsigned int offset, unsigned int elementCount, const void *data);

    unsigned int GetCount() const;
};


#endif // !BUFFER_H
