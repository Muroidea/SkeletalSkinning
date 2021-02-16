//
// Created by Rafał on 28.12.2020.
//

#ifndef KDNODE_H
#define KDNODE_H

#include "BoundedPriorityQueue.h"

#include <glm/glm.hpp>

#include <cmath>
#include <algorithm>

template <std::size_t N, typename ElemType>
class KDNode 
{
public:
    ElemType m_Value;
    int m_Level;

    KDNode* m_Left;
    KDNode* m_Right;

public:
    KDNode(int level, ElemType value)
        : m_Level(level), m_Left(nullptr), m_Right(nullptr), m_Value(value) {}

    std::size_t Size() const;

    float& operator[](std::size_t index);
    float operator[](std::size_t index) const;
};

template <std::size_t N, typename ElemType>
std::size_t KDNode<N, ElemType>::Size() const
{
    return N;
}

template <std::size_t N, typename ElemType>
float& KDNode<N, ElemType>::operator[] (std::size_t index)
{
    return m_Value[index];
}

template <std::size_t N, typename ElemType>
float KDNode<N, ElemType>::operator[] (std::size_t index) const
{
    return m_Value[index];
}

template <std::size_t N, typename ElemType>
float Distance(const ElemType& first, const ElemType& second)
{
    float result = 0.0;
    for (std::size_t i = 0; i < N; ++i)
        result += (first[i] - second[i]) * (first[i] - second[i]);
    return result;
}

#endif // !KDNODE_H
