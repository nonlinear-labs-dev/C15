#pragma once

#include "Types.h"
#include <vector>

template <typename T> class RingBuffer
{
 public:
  RingBuffer(size_t numElements)
      : m_buffer(numElements)
  {
  }

  T& push(const T& e)
  {
    T& ret = m_buffer[m_writeHead % m_buffer.size()];
    ret = e;
    m_writeHead++;
    return ret;
  }

  void push(const T* e, size_t numFrames)
  {
    auto idx = m_writeHead % m_buffer.size();
    auto cap = m_buffer.size() - idx;
    auto todoNow = std::min(cap, numFrames);
    std::copy(e, e + todoNow, m_buffer.data() + idx);
    m_writeHead += todoNow;

    if(auto rest = numFrames - todoNow)
      push(e + todoNow, rest);
  }

  const T* peek() const
  {
    if(m_writeHead > m_readHead)
      return &(m_buffer[m_readHead % m_buffer.size()]);

    return nullptr;
  }

  const T& pop()
  {
    return m_buffer[m_readHead++ % m_buffer.size()];
  }

  size_t pop(T* target, size_t targetSize, uint64_t readHead) const
  {
    auto content = m_writeHead - readHead;
    auto idx = readHead % m_buffer.size();
    auto chunkSize = m_buffer.size() - idx;
    if(auto todo = std::min(std::min<uint64_t>(chunkSize, content), targetSize))
    {
      std::copy(m_buffer.data() + idx, m_buffer.data() + idx + todo, target);
      return todo;
    }
    return 0;
  }

  uint64_t getWriteHead() const
  {
    return m_writeHead;
  }

  bool empty() const
  {
    return m_writeHead <= m_readHead;
  }

 private:
  std::vector<T> m_buffer;
  uint64_t m_readHead = 0;
  uint64_t m_writeHead = 0;
};
