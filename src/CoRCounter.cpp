//
// Created by Rafał on 28.11.2020.
//

#include "CoRCounter.h"

#include <string>
#include <fstream>

#define FLOAT_ZERO_EPSILON 0.000001

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
 
Triangle::Triangle(unsigned int alpha, unsigned int beta, unsigned int gamma, const std::vector<Vertex>& vertices, const std::vector<std::vector<float>>& weights, int numBones)
    : Alpha(alpha), Beta(beta), Gamma(gamma)
{
    Center = (vertices[Alpha].Position + vertices[Beta].Position + vertices[Gamma].Position) / 3.0f;

    AverageWeights.resize(numBones);
    for (int i = 0; i < numBones; i++)
        AverageWeights[i] = (weights[Alpha][i] + weights[Beta][i] + weights[Gamma][i]) / 3.0f;
    
    glm::vec3 AB = vertices[Beta].Position - vertices[Alpha].Position;
    glm::vec3 AC = vertices[Gamma].Position - vertices[Alpha].Position;
    Area = 0.5f * glm::length(glm::cross(AB, AC));
}

unsigned int& Triangle::operator[](unsigned int i)
{
    assert(i < 3);
    return *(&Alpha + i);
}

CoRCounter::CoRCounter(float sigma, float subdivisionTreshold, int numberOfThreads)
    : m_Sigma(sigma), m_SubdivisionTreshold(subdivisionTreshold), m_Subdivision(true), m_NumberOfThreads(numberOfThreads)
{

}

std::vector<glm::vec3> CoRCounter::GenerateCoRs(const std::vector<unsigned int>& indices, const std::vector<Vertex>& vertices)
{
    std::vector<glm::vec3> cors(vertices.size());

    // Check if CoRs are already generated
    if (LoadCoRs(cors)) 
        return cors;

    // Prepare weights for each vertex
    m_Weights.resize(vertices.size(), std::vector<float>(m_NumberOfBones, 0.0f));
    for (int i = 0; i < vertices.size(); i++)
        for (int j = 0; j < NUM_BONES_PER_VERTEX; j++)
            m_Weights[i][vertices[i].IDs[j]] = vertices[i].Weights[j];
    
    // Subdivide mesh
    std::vector<Vertex> dividedVertices = vertices;
    std::vector<unsigned int> dividedIndices = indices;
    if (m_Subdivision)
        Subdivide(dividedVertices, dividedIndices);

    for (int i = 0; i < dividedIndices.size() / 3; i++)
        m_Triangles.push_back({ dividedIndices[i * 3], dividedIndices[i * 3 + 1], dividedIndices[i * 3 + 2], dividedVertices, m_Weights, m_NumberOfBones });

    // Dispatch threads
    std::vector<std::future<void>> threads;
    unsigned int intervall = std::ceil((float)vertices.size() / (float)m_NumberOfThreads);

    for (int from = 0; from < vertices.size(); from += intervall)
    {
        unsigned int to = std::min(from + intervall, (unsigned int)vertices.size());
        threads.push_back( std::async(std::launch::async, &CoRCounter::CountCoR, this, from, to, std::ref(vertices), std::ref(cors)) );
    }

    for (int i = 0; i < threads.size(); i++)
        threads[i].wait();

    // Save CoRs and clear
    SaveCoRs(cors);

    m_Triangles.clear();
    m_Weights.clear();

    m_Triangles.shrink_to_fit();
    m_Weights.shrink_to_fit();

    return cors;
}

void CoRCounter::CountCoR(unsigned int begin, unsigned int end, const std::vector<Vertex>& vertices, std::vector<glm::vec3>& cors) const
{
    for ( ; begin < end; begin++)
    {
        glm::vec3 numerator{ 0 };
        float denominator{ 0 };

        for (const Triangle& t : m_Triangles)
        {
            float sim = Similarity(vertices[begin], begin, t);

            float areaTimesSim = t.Area * sim;
            numerator += areaTimesSim * t.Center;
            denominator += areaTimesSim;
        }

        glm::vec3 cor(0);
        if (denominator != 0)
            cor = numerator / denominator;

        cors[begin] = cor;
    }
}

float CoRCounter::Similarity(const Vertex& p, int vertexIndex, const Triangle& v) const
{
    float sim = 0;

    for (int i = 0; i < NUM_BONES_PER_VERTEX; i++)
    {
        if (p.Weights[i] <= FLOAT_ZERO_EPSILON)
            continue;

        for (int j = 0; j < m_NumberOfBones; j++)
        {
            if (p.IDs[i] != j)
            {
                if (m_Weights[vertexIndex][j] < FLOAT_ZERO_EPSILON || v.AverageWeights[j] < FLOAT_ZERO_EPSILON || v.AverageWeights[p.IDs[i]] < FLOAT_ZERO_EPSILON)
                    continue;

                float exponent = p.Weights[i] * v.AverageWeights[j] - m_Weights[vertexIndex][j] * v.AverageWeights[p.IDs[i]];
                exponent *= exponent;
                exponent /= m_Sigma * m_Sigma;

                sim += p.Weights[i] * m_Weights[vertexIndex][j] * v.AverageWeights[p.IDs[i]] * v.AverageWeights[j] * std::exp(-exponent);
            }
        }
    }

    return sim;
}

void CoRCounter::Subdivide(std::vector<Vertex>& dividedVertices, std::vector<unsigned int>& dividedIndices)
{
    for (int t = 0; t < dividedIndices.size(); t += 3)
    {
        for (int start = 0; start < 3; start++)
        {
            int end = (start + 1) % 3;

            unsigned int v0Index = dividedIndices[t + start];
            unsigned int v1Index = dividedIndices[t + end];

            std::vector<float>& w0 = m_Weights[v0Index];
            std::vector<float>& w1 = m_Weights[v1Index];

            if (SkinningWeightDistance(w0, w1) >= m_SubdivisionTreshold)
            {
                // Vertex
                glm::vec3 v0 = dividedVertices[v0Index].Position;
                glm::vec3 v1 = dividedVertices[v1Index].Position;
                Vertex newVertex;
                newVertex.Position = 0.5f * (v0 + v1);
                unsigned int newVertexIndex = dividedVertices.size();
                dividedVertices.push_back(newVertex);

                // Weights
                std::vector<float> result(w0.size(), 0.0f);
                for (int i = 0; i < w0.size(); i++)
                    result[i] = (w0[i] + w1[i]) * 0.5f;

                m_Weights.push_back(result);

                // Indices
                unsigned int v2Index = dividedIndices[t + ((end + 1) % 3)];

                dividedIndices.push_back(v0Index);
                dividedIndices.push_back(v2Index);
                dividedIndices.push_back(newVertexIndex);

                dividedIndices[t + start] = newVertexIndex;

                t -= 3;

                break;
            }
        }
    }

    printf("Divided\n");
}

float CoRCounter::WeightLength(const std::vector<float>& weights) const
{
    float length = 0;
    for (int i = 0; i < weights.size(); i++)
        length += weights[i] * weights[i];

    return std::sqrt(length);
}

float CoRCounter::SkinningWeightDistance(const std::vector<float>& weights1, const std::vector<float>& weights2) const
{
    std::vector<float> result(weights1.size(), 0.0f);
    for (int i = 0; i < weights1.size(); i++)
        result[i] = weights1[i] - weights2[i];

    return WeightLength(result);
}

void CoRCounter::SetNumberOfBones(int numOfBones)
{
    m_NumberOfBones = numOfBones;
}

void CoRCounter::SetCoRsPath(std::string corsPath)
{
    m_CoRsPath = corsPath;
}

void CoRCounter::SetSubdivision(bool sub)
{
    m_Subdivision = sub;
}

void CoRCounter::SaveCoRs(std::vector<glm::vec3>& cors) const
{
    unsigned long corSize = cors.size();

    std::ofstream fileBin(m_CoRsPath + ".cors", std::ios::out | std::ios::binary);

    fileBin << corSize;
    fileBin.write(reinterpret_cast<char*>(cors.data()), 3 * sizeof(float) * cors.size());

    fileBin.close();


    std::ofstream outputTxt(m_CoRsPath + ".txt", std::ios::out);

    outputTxt << std::to_string(corSize) << "\n";
    for (auto& cor : cors) 
        outputTxt << std::to_string(cor.x) << ", " << std::to_string(cor.y) << ", " << std::to_string(cor.z) << "\n";

    outputTxt.close();
}

bool CoRCounter::LoadCoRs(std::vector<glm::vec3>& cors) const
{
    unsigned int corSize = 0;

    std::ifstream file(m_CoRsPath + ".cors", std::ifstream::in | std::ifstream::binary);
    if (!file.is_open())
        return false;

    file >> corSize;
    file.read(reinterpret_cast<char*>(cors.data()), 3 * sizeof(float) * corSize);

    return true;
}