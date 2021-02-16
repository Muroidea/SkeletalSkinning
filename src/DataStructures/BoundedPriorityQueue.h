//
// Created by Rafał on 28.12.2020.
//

#ifndef BOUNDEDPRIORITYQUEUE_H
#define BOUNDEDPRIORITYQUEUE_H

#include <map>
#include <vector>

template <typename T, typename Container = std::multimap<float, T>>
class BoundedPriorityQueue
{
private:
    Container m_Queue;
    std::size_t m_MaxSize;

public:
    BoundedPriorityQueue(std::size_t maxSize);

    void Enqueue(const T& value, float priority);
    T Dequeue();

    std::size_t Size() const;
    std::size_t MaxSize() const;
    bool Empty() const;

    float Lowest() const;
    float Highest() const;

    std::vector<T> ToVector() const;
};

template <typename T, typename Container>
BoundedPriorityQueue<T, Container>::BoundedPriorityQueue(std::size_t maxSize)
{
    m_MaxSize = maxSize;
}

template <typename T, typename Container>
void BoundedPriorityQueue<T, Container>::Enqueue(const T& value, float priority) 
{

    m_Queue.insert(std::make_pair(priority, value));

    if (Size() > MaxSize()) 
        m_Queue.erase(std::prev(m_Queue.end()));
}

template <typename T, typename Container>
T BoundedPriorityQueue<T, Container>::Dequeue() 
{
    T result = m_Queue.begin()->second;
    m_Queue.erase(m_Queue.begin());

    return result;
}

template <typename T, typename Container>
std::size_t BoundedPriorityQueue<T, Container>::Size() const 
{
    return m_Queue.size();
}

template <typename T, typename Container>
bool BoundedPriorityQueue<T, Container>::Empty() const 
{
    return m_Queue.empty();
}

template <typename T, typename Container>
std::size_t BoundedPriorityQueue<T, Container>::MaxSize() const 
{
    return m_MaxSize;
}

template <typename T, typename Container>
float BoundedPriorityQueue<T, Container>::Highest() const
{
    return Empty() ? std::numeric_limits<float>::infinity() : m_Queue.begin()->first;
}

template <typename T, typename Container>
float BoundedPriorityQueue<T, Container>::Lowest() const
{
    return Empty() ? std::numeric_limits<float>::infinity() : m_Queue.rbegin()->first;
}

template<typename T, typename Container>
std::vector<T> BoundedPriorityQueue<T, Container>::ToVector() const
{
    std::vector<T> res;

    for (auto elem : m_Queue)
        res.push_back(elem.second);

    return res;
}

#endif // !BOUNDEDPRIORITYQUEUE_H
