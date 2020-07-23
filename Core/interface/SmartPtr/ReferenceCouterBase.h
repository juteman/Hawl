/*
 * Copyright (c) 2020 juteman
 *
 * This file is part of ReForge
 * (see https://github.com/juteman/Hawl).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#pragma once
#include "BaseType.h"
#include "Common.h"
#include <assert.h>
#include <atomic>
#include <system_error>

namespace Hawl {
namespace SmartPtr {
enum class PtrThreadSafeMode
{
  ThreadNoSafe,
  ThreadSafe
};

/// base reference couter class
/// include about share reference counter and the
/// weak reference counter
template<PtrThreadSafeMode Mode>
class RefCntBase;

/// Thread safe mode
template<>
class RefCntBase<PtrThreadSafeMode::ThreadSafe>
{
public:
  inline explicit RefCntBase() {}

  virtual ~RefCntBase() {}

  /// Get the number of share reference count
  inline INT32 GetSharedRefCnt() const
  {
    return m_shareRefCnt.load(std::memory_order_relaxed);
  }

  /// Add the number of share reference count
  inline void AddShareRefCnt() { ++m_shareRefCnt; }

  /// add a share reference to this counter
  /// ONLY when at least one reference
  /// @return true if the reference added success
  bool ConditionallyAddShareRefCnt()
  {
    for (;;) {
      // Peek the number of ShareReference
      // why use compare exchange?
      // because on the multiple thread the value will change
      INT32 shareRefCntRecord = m_shareRefCnt.load(std::memory_order_relaxed);
      if (shareRefCntRecord == 0)
        [[unlikely]]
        // if no reference, must not add a reference on it
        return false;
      else {
        if (m_shareRefCnt.compare_exchange_weak(shareRefCntRecord,
                                                shareRefCntRecord + 1))
          [[likely]] { return true; }
      }
    }
  }

  /// Release the share reference
  inline void ReleaseShareRef()
  {
    assert(m_shareRefCnt.load(std::memory_order_relaxed) > 0);
    if (--m_shareRefCnt == 0) {
      // all the reference release
      // destory the object
      DestoryObject();

      // no share reference. decrement weak reference counter
      ReleaseShareRef();
    }
  }

  inline void AddWeakReference() { ++m_weakRefCnt; }

  inline void ReleaseWeakReference()
  {
    assert(m_weakRefCnt.load() > 0);
    if (--m_weakRefCnt == 0) {
      delete this;
    }
  }

private:
  /// The number of shared references to the object.
  /// When count equal 0, the object will be destory
  std::atomic<INT32> m_shareRefCnt;

  /// The number of weak references to the object.
  /// The count will see any shared references as one.
  std::atomic<INT32> m_weakRefCnt;

  virtual void DestoryObject() = 0;
  HAWL_DISABLE_COPY(RefCntBase)
};

} // !SmartPtr

} // !Hawl