//
// Created by Rafał on 28.11.2020.
//

#include "Model.h"

#include <algorithm>
#include <map>
#include <iostream>
#include <chrono>

#include "Profiler.h"
#include "Application.h"

#include <glm/gtx/string_cast.hpp>

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
        aiProcess_JoinIdenticalVertices);

    bool ret = false;
    if (scene)
        ret = InitFromScene(scene, filename);
    else
        printf("Error parsing %s: %s", filename.c_str(), importer.GetErrorString());

    LoadMaterials(scene, filename);

    // Generate CoR's
    std::vector<glm::vec3> cors(m_Vertices.size(), glm::vec3(0.0f));
    
    CoRCounter corCounter(0.1, 0.15, 8);
    corCounter.SetNumberOfBones(m_Skeleton.m_NumBones);
    corCounter.SetSubdivision(AppState::s_Subdivide);

    std::string name = filename.substr(0, filename.find_last_of("."));
    name = name.substr(name.find_last_of("/") + 1, name.length());
    corCounter.SetCoRsPath(name);
    name += " \n";
    {
        PROFILE_SCOPE(name.c_str());
        cors = corCounter.GenerateCoRs(m_Indices, m_Vertices);
    }
    
    for (int i = 0; i < m_Vertices.size(); i++)
        m_Vertices[i].CoR = std::move(cors[i]);
    

    // Prepare OpenGL stuff
    if (ret)
    {
        m_VAO = new VertexArray();

        m_VBO = new Buffer(m_Vertices.size(), sizeof(m_Vertices[0]), m_Vertices.data(), GL_ARRAY_BUFFER);
        m_IBO = new Buffer(m_Indices.size(), sizeof(m_Indices[0]), m_Indices.data(), GL_ELEMENT_ARRAY_BUFFER);

        VertexBufferLayout vertexBufferLayout;
        vertexBufferLayout.PushElement(VertexType::VEC3F); // Position
        vertexBufferLayout.PushElement(VertexType::VEC2F); // TexCoord
        vertexBufferLayout.PushElement(VertexType::VEC4I); // BonesID
        vertexBufferLayout.PushElement(VertexType::VEC4F); // Weights
        vertexBufferLayout.PushElement(VertexType::VEC3F); // CoRs

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
        aiTextureType::aiTextureType_DIFFUSE
    };

    // Initialize the materials
    for (unsigned int i = 0 ; i < scene->mNumMaterials ; i++)
    {
        Material *newMaterial = new Material();

        aiMaterial* material = scene->mMaterials[i];
        aiString texture_path;

        for (auto type : textureTypes)
        {
            if (material->GetTextureCount(type) > 0 || m_Name == "Willie")
            {
                material->GetTexture(type, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);

                if (m_Name == "Willie") texture_path = "/WillieColor.png";

                std::string path(texture_path.data);
                slashIndex = path.find_last_of("/");

                path = path.substr(slashIndex + 1, path.length() - slashIndex);

                std::string fullPath = dir + "/textures/" + path;

                if (type == aiTextureType_DIFFUSE)
                    newMaterial->Diffuse = new Texture(fullPath.c_str(), TextureType::DIFFUSE);
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
        }
        /*
        GLuint m_TimerGL;
        glGenQueries(GL_TIME_ELAPSED, &m_TimerGL);
        glBeginQuery(GL_TIME_ELAPSED, m_TimerGL);
        */

        glDrawElementsBaseVertex(GL_TRIANGLES,
            m_Entries[i].NumIndices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
            m_Entries[i].BaseVertex);
        /*
        glEndQuery(GL_TIME_ELAPSED);

        // retrieving the recorded elapsed time
        // wait until the query result is available
        int done = 0;
        while (!done) {
            glGetQueryObjectiv(m_TimerGL,
                GL_QUERY_RESULT_AVAILABLE,
                &done);
        }

        // get the query result
        GLuint64 elapsed_time;
        glGetQueryObjectui64v(m_TimerGL, GL_QUERY_RESULT, &elapsed_time);
        printf("Time Elapsed: %f ms \n", elapsed_time / 1000000.0);

        glDeleteQueries(1, &m_TimerGL);*/
    }

    m_VAO->Unbind();
}

