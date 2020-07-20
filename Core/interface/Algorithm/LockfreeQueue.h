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
    volatile QueueNode* next;

    /// type data
    T data;
  };

public:
  /// Initialize with dummy node
  Queue() { m_head = m_tail = new QueueNode(); }

  /// Release the queue list
  ~Queue()
  {
    while (m_head != nullptr) {
      QueueNode* temp = m_head;
      m_head          = temp->next;
      delete temp;
    }
  }

  /// Adds an node to the tail of the queue.
  /// @param Item The item to add.
  /// @return true if the node was added, false for some reason.
  bool EnQueue(T& InData) {}

  /// check the queue is empty
  /// @return true if empty, false for no empty
  bool IsEmpty() { return (m_head->next == nullptr); }

private:
  /// atomic head and  tail
  std::atomic<QueueNode*> m_head;
  std::atomic<QueueNode*> m_tail;

  /// delete copy constructor and  assign operator
  HAWL_DISABLE_COPY(Queue)
};
}
}