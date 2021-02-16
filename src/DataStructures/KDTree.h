//
// Created by Rafał on 28.12.2020.
//

#ifndef KDTREE_H
#define KDTREE_H

#include "BoundedPriorityQueue.h"
#include "KDNode.h"

#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>

template <std::size_t N, typename ElemType>
class KDTree 
{
private:
    KDNode<N, ElemType>* m_Root;
    std::size_t m_Size;

public:
    KDTree();
    KDTree(std::vector<ElemType>& values);
    ~KDTree();

    std::size_t Dimension() const;
    std::size_t Size() const;

    bool Empty() const;
    bool Contains(const ElemType& value) const;

    void Insert(const ElemType& value);
    ElemType& operator[](const ElemType& value);

    std::vector<ElemType> kNNValue(const ElemType& value, std::size_t k) const;

private:
    KDNode<N, ElemType>* BuildTree(typename std::vector<ElemType>::iterator start, typename std::vector<ElemType>::iterator end, int level);
    void Free(KDNode<N, ElemType>* node);

    KDNode<N, ElemType>* FindNode(KDNode<N, ElemType>* node, const ElemType& value) const;
    void NearestNeighborRecurse(const KDNode<N, ElemType>* node, const ElemType& key, typename BoundedPriorityQueue<ElemType>& queue) const;
};


template <std::size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() 
    : m_Root(nullptr), m_Size(0) { }

template <std::size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree(typename std::vector<ElemType>& values)
{
    m_Root = BuildTree(values.begin(), values.end(), 0);
    m_Size = values.size();
}

template <std::size_t N, typename ElemType>
KDNode<N, ElemType>* KDTree<N, ElemType>::BuildTree(typename std::vector<ElemType>::iterator start, typename std::vector<ElemType>::iterator end, int level)
{
    if (start >= end) 
        return nullptr;

    int axis = level % N;

    auto mid = start + (end - start) / 2;
    std::nth_element(start, mid, end, 
        [axis](const ElemType& first, const ElemType& second)
        {
            return first[axis] < second[axis];
        }
    );

    while (mid > start && (mid - 1)[axis] == mid[axis])
        --mid;

    KDNode<N, ElemType>* newNode = new KDNode<N, ElemType>(level, *mid);
    newNode->m_Left = BuildTree(start, mid, level + 1);
    newNode->m_Right = BuildTree(mid + 1, end, level + 1);

    return newNode;
}

template <std::size_t N, typename ElemType>
void KDTree<N, ElemType>::Free(KDNode<N, ElemType>* node)
{
    if (node == nullptr) return;

    Free(node->m_Left);
    Free(node->m_Right);

    delete node;
}

template <std::size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() 
{
    Free(m_Root);
}

template <std::size_t N, typename ElemType>
std::size_t KDTree<N, ElemType>::Dimension() const 
{
    return N;
}

template <std::size_t N, typename ElemType>
std::size_t KDTree<N, ElemType>::Size() const 
{
    return m_Size;
}

template <std::size_t N, typename ElemType>
bool KDTree<N, ElemType>::Empty() const 
{
    return m_Size == 0;
}

template <std::size_t N, typename ElemType>
KDNode<N, ElemType>* KDTree<N, ElemType>::FindNode(KDNode<N, ElemType>* node, const ElemType& value) const 
{
    if (node == nullptr || node->m_Value == value)
        return node;

    const ElemType& currValue = node->m_Value;
    int level = node->m_Level;
    if (value[level % N] < currValue[level % N]) 
        return node->m_Left == nullptr ? node : FindNode(node->m_Left, value);
    else 
        return node->m_Right == nullptr ? node : FindNode(node->m_Right, value);
}

template <std::size_t N, typename ElemType>
bool KDTree<N, ElemType>::Contains(const ElemType& value) const
{
    KDNode<N, ElemType>* node = FindNode(m_Root, value);
    return node != nullptr && node->m_Value == value;
}

template <std::size_t N, typename ElemType>
void KDTree<N, ElemType>::Insert(const ElemType& value) 
{
    KDNode<N, ElemType>* targetNode = FindNode(m_Root, value);
    if (targetNode == nullptr)
    { 
        m_Root = new KDNode<N, ElemType>(0, value);
        m_Size = 1;
    }
    else 
    {
        if (targetNode->m_Value == value)
            targetNode->m_Value = value;
        else 
        { 
            int level = targetNode->m_Level;
            KDNode<N, ElemType>* newNode = new KDNode<N, ElemType>(level + 1, value);

            if (value[level % N] < targetNode->m_Value[level % N])
                targetNode->m_Left = newNode;
            else
                targetNode->m_Right = newNode;

            m_Size++;
        }
    }
}

template <std::size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::operator[](const ElemType& value)
{
    auto node = FindNode(m_Root, value);
    if (node != nullptr && node->m_Value == value)
        return node->m_Value;
    else 
    {
        Insert(value);
        if (node == nullptr) 
            return m_Root->m_Value;
        else 
            return (node->m_Left != nullptr && node->m_Left->m_Value == value) ? node->m_Left->m_Value : node->m_Right->m_Value;
    }
}

template <std::size_t N, typename ElemType>
void KDTree<N, ElemType>::NearestNeighborRecurse(const KDNode<N, ElemType>* node, const ElemType& key, typename BoundedPriorityQueue<ElemType>& queue) const
{
    if (node == nullptr) return;
    const ElemType& value = node->m_Value;

    queue.Enqueue(value, Distance<N>(value, key));

    int level = node->m_Level;
    bool isLeftTree;
    if (key[level % N] < value[level % N]) 
    {
        NearestNeighborRecurse(node->m_Left, key, queue);
        isLeftTree = true;
    }
    else 
    {
        NearestNeighborRecurse(node->m_Right, key, queue);
        isLeftTree = false;
    }

    if (queue.Size() < queue.MaxSize() || fabs(key[level % N] - value[level % N]) < queue.Lowest()) 
    {
        if (isLeftTree) 
            NearestNeighborRecurse(node->m_Right, key, queue);
        else 
            NearestNeighborRecurse(node->m_Left, key, queue);
    }
}

template <std::size_t N, typename ElemType>
std::vector<ElemType> KDTree<N, ElemType>::kNNValue(const ElemType& key, std::size_t k) const
{
    BoundedPriorityQueue<ElemType> queue(k);
    if (Empty()) 
        return std::vector<ElemType>();

    NearestNeighborRecurse(m_Root, key, queue);
    /*
    std::map<ElemType, int> counter;
    while (!queue.Empty()) 
        counter[queue.Dequeue()]++;

    ElemType result;
    std::vector<ElemType> res;
    int cnt = -1;
    for (const auto& p : counter) 
    {
        if (p.second > cnt) 
        {
            result = p.first;
            cnt = p.second;
        }
    }*/


    return queue.ToVector();
}

#endif // !KDTREE_H
