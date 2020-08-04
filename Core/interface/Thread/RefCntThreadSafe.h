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

#include "BaseType.h"
#include "Common.h"
#include <atomic>

namespace Hawl
{
/// Class use reference counting to ensure thread safe
class RefCntThreadSafe
{
  public:
    RefCntThreadSafe()
    {
        m_counter.store(0);
    }

    RefCntThreadSafe(const RefCntThreadSafe &other)
    {
        m_counter = other.m_counter;
    }

    /// Increment the counter by 1
    /// @return the newer counter number
    INT32 Increment()
    {
        return ++m_counter;
    }

    /// Decrement the counter by 1
    /// @return the newer counter number
    INT32 Decrement()
    {
        return --m_counter;
    }

    /// Add a number to a counter
    /// @param value number to be added
    /// @return the newer counter number
    INT32 Add(INT32 value)
    {
        return m_counter.fetch_add(value, std::memory_order_relaxed);
    }

    /// Sub a number to a counter
    /// @param value number to be sub
    /// @return the newer counter number
    INT32 Sub(INT32 value)
    {
        return m_counter.fetch_sub(value, std::memory_order_relaxed);
    }

    /// Get the current counter number
    UINT32 GetCnt()
    {
        return m_counter.load(std::memory_order_relaxed);
    }

  private:
    /// counter for thread safe
    volatile std::atomic<INT32> m_counter{};

    void operator=(const RefCntThreadSafe &) = delete;
};
} // namespace Hawl