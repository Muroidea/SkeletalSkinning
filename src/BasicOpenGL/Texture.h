//
// Created by Rafał on 28.11.2020.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

enum class TextureType { DIFFUSE, SPECULAR, NORMALS };

class Texture
{
private:
    unsigned int m_ID;
    TextureType m_TextureType;

public:
    Texture(const std::string& path, TextureType textureType, bool transparency = false);
    Texture(unsigned int id, TextureType textureType);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    TextureType GetTextureType() const;
};

struct Material
{
    ~Material()
    {
        if (Diffuse)
        {
            delete Diffuse;
            Diffuse = nullptr;
        }

        if (Specular)
        {
            delete Specular;
            Specular = nullptr;
        }

        if (Normals)
        {
            delete Normals;
            Normals = nullptr;
        }
    }

    Texture* Diffuse = nullptr;
    Texture* Specular = nullptr;
    Texture* Normals = nullptr;
};

#endif //TEXTURE_H
