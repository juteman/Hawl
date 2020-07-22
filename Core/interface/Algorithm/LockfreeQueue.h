/**
 *  Copyright 2020 juteman
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#pragma once
#include "Common.h"
#include <atomic>
#include <utility>
namespace Hawl {
namespace Algorithm {

enum class QueueModel
{
  SPSC,
  MPMC
};

/// lock free queue implement
template<typename T>
class Queue
{
private:
  /// Queue node implement
  struct QueueNode
  {
    /// construct node with null
    QueueNode()
      : next{ nullptr }
    {}

    explicit QueueNode(const T& InData)
      : next{ nullptr }
      , data{ InData }
    {}

    explicit QueueNode(const T&& InData)
      : next{ nullptr }
      , data{ std::move(InData) }
    {}

    /// pointer to the next node of the Queue
    std::atomic<QueueNode*> next;

    /// type data
    T data;
  };

public:
  /// Initialize with dummy node
  Queue()
  {
    QueueNode* dummyNode = new QueueNode();
    m_head.store(dummyNode, std::memory_order_relaxed);
    m_tail.store(dummyNode, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_seq_cst);
  }

  /// Release the queue list
  ~Queue()
  {
    while (m_head != nullptr) {
      QueueNode* temp = m_head.load(std::memory_order_relaxed);
      m_head          = temp->next;
      delete temp;
    }
  }

  /// Adds an node to the tail of the queue.
  /// @param Item The item to add.
  /// @return true if the node was added, false for some thread race
  /// condition
  bool EnQueue(T const& InData)
  {
    // Create new Node and check if create success
    QueueNode* newNode = new QueueNode(InData);

    if (nullptr == newNode)
      return false;

    // concurrent link queue
    while (true) {
      QueueNode* tail = m_tail.load(std::memory_order_acquire);

      QueueNode* tailNext = tail->next.load(std::memory_order_acquire);

      if (tail == m_tail.load(std::memory_order_acquire))
        [[likely]]
        {
          if (tailNext == nullptr) {
            // insert node
            if (tail->next.compare_exchange_weak(tailNext, newNode)) {
              // set the tail node
              return m_tail.compare_exchange_strong(tail, newNode);
            }
          }

          // if tailNext is not nullptr, fetch the Queue tails to next
          else {
            m_tail.compare_exchange_strong(tail, tailNext);
          }
        }
    }
  }

  /// pop the element from the queue head
  /// @param outData get the queue head element
  /// @return true dequeue success, else the queue is empty
  bool DeQueue(T& outData)
  {
    while (true) {
      QueueNode* head     = m_head.load(std::memory_order_acquire);
      QueueNode* tail     = m_tail.load(std::memory_order_acquire);
      QueueNode* headNext = head->next.load(std::memory_order_acquire);

      if (head == m_head.load(std::memory_order_acquire))
        [[likely]]
        {
          if (head == tail) {
            if (headNext == nullptr)
              return false;

            // if the head next is not null. must be another
            // thread EnQeue the element. So fetch tail pointer to the next
            m_head.compare_exchange_strong(tail, headNext);
          }
          else {
            if (headNext == nullptr)
              // it means anothor thread take the element.
              // and the queue is empty. So just continue, and if
              // no element again, will return false.
              continue;

            outData = headNext->data;
            if (m_head.compare_exchange_weak(head, headNext)) {
              free(head);
              return true;
            }
          }
        }
    }
  }

  /// Get the  head  data not remove from Queue
  /// @param outData get the data of head
  /// @return if queue empty get false, else get true
  bool Peek(T& outData)
  {
    /// if head is change by other thread use deque, fetch head to m_head again
    /// because Peek don't change the head. it will be thread safe
    for (auto head = m_head.load(std::memory_order_acquire); head != nullptr;
         head      = m_head.load(std::memory_order_acquire)) {
      outData = head->next.load();
      if (outData)
        return true;
    }

    return false;
  }

  /// check is lock free queue
  /// @return true for lock free
  bool Islockfree() const
  {
    return m_head.is_lock_free() && m_tail.is_lock_free();
  }

  /// check the queue is empty
  /// @return true if empty, false for no empty
  bool IsEmpty() { return (m_head.load() == m_tail.load()); }

private:
  /// atomic head and  tail
  std::atomic<QueueNode*> m_head;
  std::atomic<QueueNode*> m_tail;

  /// delete copy constructor and  assign operator
  HAWL_DISABLE_COPY(Queue)
};
}
}