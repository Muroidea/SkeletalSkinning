//
// Created by Rafał on 28.11.2020.
//

#include "Model.h"

#include <algorithm>
#include <map>
#include <iostream>
#include <chrono>

#include <glm/gtx/string_cast.hpp>

bool Vertex::AddBoneData(unsigned int boneID, float weight)
{
    for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; i++)
    {
        if (Weights[i] == 0.0f)
        {
            IDs[i] = boneID;
            Weights[i] = weight;
            return true;
        }
    }

    return false;
}

Model::Model(std::string name)
    : m_Name(name)
{
    m_VAO = nullptr;
    m_IBO = nullptr;
    m_VBO = nullptr;
}


Model::~Model()
{
    Clear();
}


void Model::Clear()
{
    for (unsigned int i = 0; i < m_Materials.size(); i++)
    {
        delete m_Materials[i];
        m_Materials[i] = nullptr;
    }

    if (m_VBO)
    {
        delete m_VBO;
        m_VBO = nullptr;
    }

    if (m_IBO)
    {
        delete m_IBO;
        m_IBO = nullptr;
    }

    if (m_VAO)
    {
        delete m_VAO;
        m_VAO = nullptr;
    }

    m_Entries.clear();
    m_Vertices.clear();
    m_Indices.clear();
    m_Materials.clear();
}


bool Model::LoadModel(const std::string& filename)
{
    Clear();

    Assimp::Importer importer;
    const aiScene* scene;

    scene = importer.ReadFile(filename.c_str(),
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices);

    bool ret = false;
    if (scene)
        ret = InitFromScene(scene, filename);
    else
        printf("Error parsing %s: %s", filename.c_str(), importer.GetErrorString());

    LoadMaterials(scene, filename);

    if (ret)
    {
        m_VAO = new VertexArray();

        m_VBO = new Buffer(m_Vertices.size(), sizeof(m_Vertices[0]), m_Vertices.data(), GL_ARRAY_BUFFER);
        m_IBO = new Buffer(m_Indices.size(), sizeof(m_Indices[0]), m_Indices.data(), GL_ELEMENT_ARRAY_BUFFER);

        VertexBufferLayout vertexBufferLayout;
        vertexBufferLayout.PushElement(VertexType::VEC3F); // Position
        vertexBufferLayout.PushElement(VertexType::VEC3F); // Normals
        vertexBufferLayout.PushElement(VertexType::VEC2F); // TexCoord
        vertexBufferLayout.PushElement(VertexType::VEC4I); // TexCoord
        vertexBufferLayout.PushElement(VertexType::VEC4F); // TexCoord

        m_VAO->BindVertexBuffer(*m_VBO, vertexBufferLayout);
        m_VAO->BindIndexBuffer(*m_IBO);
        m_VAO->Unbind();
    }

    m_Indices.clear();
    m_Vertices.clear();
    m_Indices.shrink_to_fit();
    m_Vertices.shrink_to_fit();

    importer.FreeScene();

    return ret;
}
 
bool Model::InitFromScene(const aiScene* scene, const std::string& filename)
{
    m_Entries.resize(scene->mNumMeshes);

    unsigned int numVertices = 0;
    unsigned int numIndices = 0;

    for (unsigned int i = 0; i < m_Entries.size(); i++)
    {
        m_Entries[i].NumIndices = scene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = numVertices;
        m_Entries[i].BaseIndex = numIndices;

        numVertices += scene->mMeshes[i]->mNumVertices;
        numIndices += m_Entries[i].NumIndices;
    }

    // Reserve space in the vectors for the vertex attributes and indices
    m_Vertices.reserve(numVertices);
    m_Indices.reserve(numIndices);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++)
        LoadMeshInfo(i, scene->mMeshes[i], scene->mRootNode);

    return true;
}

void Model::LoadMeshInfo(unsigned int meshIndex, const aiMesh* mesh, const aiNode* node)
{
    Vertex vertex;
    glm::vec3 vector3;
    glm::vec2 vector2;

    // Populate the vertex attribute vectors
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // Position
        vector3 = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.Position = vector3;

        // Normals
        if (mesh->HasNormals())
            vector3 = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        else
            vector3 = glm::vec3(0.0f);
        vertex.Normal = vector3;

        // TexCoords
        if (mesh->mTextureCoords[0])
        {
            vector2 = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else
            vector2 = glm::vec2(0.0f, 0.0f);
        vertex.TexCoords = vector2;


        m_Vertices.push_back(vertex);
    }

    if (mesh->HasBones())
    {
        m_Skeleton.SetGlobalInverseMatrix(glm::inverse(glm::transpose(glm::make_mat4(&node->mTransformation.a1))));
        LoadBones(meshIndex, mesh, node);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        m_Indices.push_back(face.mIndices[0]);
        m_Indices.push_back(face.mIndices[1]);
        m_Indices.push_back(face.mIndices[2]);
    }
}

void Model::LoadBones(unsigned int meshIndex, const aiMesh* mesh, const aiNode* node)
{
    std::map<std::string, unsigned int> boneMapping;

    for (int i = 0; i < mesh->mNumBones; i++)
    {
        std::string boneName = mesh->mBones[i]->mName.C_Str();
        boneMapping[boneName] = i;
        m_Skeleton.m_Bones.push_back(Bone());
    }
    
    ProcessNode(node, boneMapping, m_Skeleton.m_NumBones);

    for (int i = 0; i < mesh->mNumBones; i++)
    {
        int boneID = boneMapping[m_Skeleton.m_Bones[i].Name];
        m_Skeleton.m_Bones[i].SetOffset(glm::transpose(glm::make_mat4(&mesh->mBones[boneID]->mOffsetMatrix.a1)));
        
        for (unsigned int j = 0; j < mesh->mBones[boneID]->mNumWeights; j++)
        {
            unsigned int vertexID = m_Entries[meshIndex].BaseVertex + mesh->mBones[boneID]->mWeights[j].mVertexId;
            float weight = mesh->mBones[boneID]->mWeights[j].mWeight;
            m_Vertices[vertexID].AddBoneData(i, weight);
        }
    }
}

void Model::ProcessNode(const aiNode* node, const std::map<std::string, unsigned int>& boneMapping, int parentID)
{
    auto res = boneMapping.find(node->mName.C_Str());
    if (res != boneMapping.end())
    {
        Bone& bone = m_Skeleton.m_Bones[m_Skeleton.m_NumBones];
        bone.Name = node->mName.C_Str();
        bone.ParentID = parentID;
        bone.SetDefaultTransformation(glm::transpose(glm::make_mat4(&node->mTransformation.a1)));
        parentID = m_Skeleton.m_NumBones;
        m_Skeleton.m_NumBones++;
    }

    for (int i = 0; i < node->mNumChildren; i++)
        ProcessNode(node->mChildren[i], boneMapping, parentID);
}

void Model::LoadMaterials(const aiScene* scene, const std::string& filename)
{
    // Extract the directory part from the file name
    std::string::size_type slashIndex = filename.find_last_of("/");
    std::string dir = filename.substr(0, slashIndex);
    
    aiTextureType textureTypes[] = {
        aiTextureType::aiTextureType_DIFFUSE,
        aiTextureType::aiTextureType_SPECULAR,
        aiTextureType::aiTextureType_NORMALS
    };

    // Initialize the materials
    for (unsigned int i = 0 ; i < scene->mNumMaterials ; i++)
    {
        Material *newMaterial = new Material();

        aiMaterial* material = scene->mMaterials[i];
        aiString texture_path;

        for (auto type : textureTypes)
        {
            if (material->GetTextureCount(type) > 0)
            {
                material->GetTexture(type, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);

                std::string path(texture_path.data);
                slashIndex = path.find_last_of("/");

                path = path.substr(slashIndex + 1, path.length() - slashIndex);

                std::string fullPath = dir + "/textures/" + path;

                if (type == aiTextureType_DIFFUSE)
                    newMaterial->Diffuse = new Texture(fullPath.c_str(), TextureType::DIFFUSE);
                else if (type == aiTextureType_SPECULAR)
                    newMaterial->Specular = new Texture(fullPath.c_str(), TextureType::SPECULAR);
                else if (type == aiTextureType_NORMALS)
                    newMaterial->Normals = new Texture(fullPath.c_str(), TextureType::NORMALS);
            }
        }

        m_Materials.push_back(newMaterial);
    }
}

void Model::Render()
{
    m_VAO->Bind();

    for (unsigned int i = 0; i < m_Entries.size(); i++)
    {
        if (m_Materials.size() > 0)
        {
            unsigned int matIndex = m_Entries[i].MaterialIndex;

            if (m_Materials[matIndex]->Diffuse)
                m_Materials[matIndex]->Diffuse->Bind(0);

            if (m_Materials[matIndex]->Specular)
                m_Materials[matIndex]->Specular->Bind(1);

            if (m_Materials[matIndex]->Normals)
                m_Materials[matIndex]->Normals->Bind(2);
        }

        glDrawElementsBaseVertex(GL_TRIANGLES,
            m_Entries[i].NumIndices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
            m_Entries[i].BaseVertex);
    }

    m_VAO->Unbind();
}

