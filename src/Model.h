//
// Created by Rafał on 28.11.2020.
//

#ifndef MODEL_H
#define MODEL_H

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "BasicOpenGL/VertexArray.h"
#include "BasicOpenGL/Texture.h"
#include "Animation/Animation.h"
#include "Animation/Skeleton.h"
#include "CoRCounter.h"

class Model
{
private:

    struct MeshEntry
    {
        MeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = 0;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    std::string m_Name;

    VertexArray *m_VAO;
    Buffer *m_VBO;
    Buffer *m_IBO;

    std::vector<MeshEntry> m_Entries;

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Material*> m_Materials;

public:
    Skeleton m_Skeleton;

    Model(std::string name);
    ~Model();

    bool LoadModel(const std::string& filename);

    void Render();

    inline unsigned int GetNumBones() const { return m_Skeleton.m_NumBones; }
    inline const std::string& GetName() const { return m_Name; }

private:
    bool InitFromScene(const aiScene* scene, const std::string& filename);

    void LoadMeshInfo(unsigned int meshIndex, const aiMesh* mesh, const aiNode* node);
    void LoadMaterials(const aiScene* scene, const std::string& filename);
    
    void LoadBones(unsigned int meshIndex, const aiMesh* mesh, const aiNode* node);
    void ProcessNode(const aiNode* node, const std::map<std::string, unsigned int>& boneMapping, int parentID);

    void Clear();
};

#endif //MODEL_H
