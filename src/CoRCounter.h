//
// Created by Rafał on 28.11.2020.
//

#ifndef CORCOUNTER_H
#define CORCOUNTER_H

#include <glm/glm.hpp>

#include "DataStructures/KDTree.h"

#include <vector>
#include <future>
#include <algorithm>
#include <string>

#define NUM_BONES_PER_VERTEX 4

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 TexCoords;

    unsigned int IDs[NUM_BONES_PER_VERTEX] = { 0 };
    float Weights[NUM_BONES_PER_VERTEX] = { 0.0f };

    glm::vec3 CoR;

    bool AddBoneData(unsigned int boneID, float weight);
};

struct Triangle
{
    unsigned int Alpha, Beta, Gamma;

    glm::vec3 Center;
    std::vector<float> AverageWeights;
    float Area;

    Triangle(unsigned int alpha, unsigned  int beta, unsigned  int gamma, const std::vector<Vertex>& vertices, const std::vector<std::vector<float>>& weights, int numBones);

    unsigned int& operator[](unsigned int i);
};

class CoRCounter
{
private:
    float m_Sigma;
    float m_SubdivisionTreshold;
    bool m_Subdivision;
    int m_NumberOfThreads;

    std::vector<Triangle> m_Triangles;
    std::vector<std::vector<float>> m_Weights;
    int m_NumberOfBones;
    std::string m_CoRsPath;

public:
    CoRCounter(float sigma = 0.1f, float subdivisionTreshold = 0.1f, int numberOfThreads = 1);

    std::vector<glm::vec3> GenerateCoRs(const std::vector<unsigned int>& indices, const std::vector<Vertex>& vertices);

    void SetNumberOfBones(int numOfBones);
    void SetCoRsPath(std::string corsPath);
    void SetSubdivision(bool sub);

private:
    void CountCoR(unsigned int begin, unsigned int end, const std::vector<Vertex>& vertices, std::vector<glm::vec3>& cors) const;
    float Similarity(const Vertex& p, int vertexIndex, const Triangle& b) const;
    void Subdivide(std::vector<Vertex>& dividedVertices, std::vector<unsigned int>& dividedIndices);

    float WeightLength(const std::vector<float>& weights) const;
    float SkinningWeightDistance(const std::vector<float>& weights1, const std::vector<float>& weights2) const;

    void SaveCoRs(std::vector<glm::vec3>& cors) const;
    bool LoadCoRs(std::vector<glm::vec3>& cors) const;
};

#endif // !CORCOUNTER_H
