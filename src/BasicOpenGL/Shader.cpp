//
// Created by Rafał on 28.11.2020.
//

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>


Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    std::string sourceVS = LoadShader(vertexShaderPath);
    std::string sourceFS = LoadShader(fragmentShaderPath);

    m_ID = CreateShader(sourceVS, sourceFS);
}

Shader::~Shader()
{
    glDeleteProgram(m_ID);
}

std::string Shader::LoadShader(const std::string& path)
{
    std::ifstream stream(path);

    std::string line;
    std::stringstream shader;
    while(std::getline(stream, line))
        shader << line << "\n";

    return shader.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        auto info = new char[length];
        glGetShaderInfoLog(shader, length, &length, info);
        std::cout << "WARNING: " << ((type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT") << "shader compilation\n" << info << std::endl;
        delete[] info;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();

    unsigned int VShader = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int FShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, VShader);
    glAttachShader(program, FShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *info = new char[length];
        glGetProgramInfoLog(program, length, &length, info);
        std::cout << "WARNING: program linking\n" << info << std::endl;
        delete[] info;
    }

    glDeleteShader(VShader);
    glDeleteShader(FShader);

    return program;
}

void Shader::Bind() const
{
    glUseProgram(m_ID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetBlockBinding(unsigned int binding, std::string blockName)
{
    unsigned int index = glGetUniformBlockIndex(m_ID, blockName.c_str());
    glUniformBlockBinding(m_ID, index, binding);
}

void Shader::SetUniform1f(const std::string &name, float x)
{
    glUniform1f(GetUniformLocation(name), x);
}

void Shader::SetUniform3f(const std::string &name, float x, float y, float z)
{
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetUniform4f(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void Shader::SetUniformMat2x4(const std::string& name, const glm::mat2x4& matrix, int count)
{
    glUniformMatrix2x4fv(GetUniformLocation(name), count, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniformMat4(const std::string &name, const glm::mat4& matrix, int count)
{
    glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniform1i(const std::string &name, int i)
{
    glUniform1i(GetUniformLocation(name), i);
}

int Shader::GetUniformLocation(const std::string &name)
{
    if(m_Uniforms.find(name) != m_Uniforms.end())
        return m_Uniforms.find(name)->second;

    int location = glGetUniformLocation(m_ID, name.c_str());
    if (location == -1)
        std::cout << "WARNING: uniform '" << name << "' doesn't exist!" << std::endl;

    m_Uniforms.emplace(name, location);
    return location;
}


