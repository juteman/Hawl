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

/// base reference couter class
/// include about share reference counter and the
/// weak reference counter
class RefCntBase
{
public:
  inline explicit RefCntBase() noexcept
  {
    m_shareRefCnt.store(1);
    m_weakRefCnt.store(1);
  }

  virtual ~RefCntBase() noexcept {}

  /// Get the number of share reference count
  inline INT32 GetSharedRefCnt() const
  {
    return m_shareRefCnt.load(std::memory_order_relaxed);
  }

  /// Add the number of share reference count
  inline void AddShareRefCnt()
  {
    ++m_shareRefCnt;
    ++m_weakRefCnt;
  }

  /// add a share reference to this counter
  /// ONLY when at least one reference
  /// @return if success add shared reference return the
  /// self pointer else return nullptr
  inline RefCntBase* ConditionallyAddShareRefCnt()
  {
    for (INT32 shareRefCntRecord =
           m_shareRefCnt.load(std::memory_order_relaxed);
         shareRefCntRecord != 0;
         shareRefCntRecord = m_shareRefCnt.load(std::memory_order_relaxed)) {
      // Peek the number of ShareReference
      // why use compare exchange?
      // because on the multiple thread the value will change
      if (m_shareRefCnt.compare_exchange_weak(shareRefCntRecord,
                                              shareRefCntRecord + 1))
        [[likely]] { return this; }
    }
    return nullptr;
  }

  /// Release the share reference
  inline void ReleaseShareRef()
  {
    assert((m_shareRefCnt.load(std::memory_order_relaxed) > 0) &&
           (m_weakRefCnt.load(std::memory_order_relaxed) > 0));
    if (--m_shareRefCnt == 0)
      DestoryObject();

    if (--m_weakRefCnt) {
      DestoryRefCnt();
    }
  }

  inline void AddWeakReference() { ++m_weakRefCnt; }

  inline void ReleaseWeakReference()
  {
    assert(m_weakRefCnt.load() > 0);
    if (--m_weakRefCnt == 0) {
      DestoryRefCnt();
    }
  }

  /// Destory the contained object
  virtual void DestoryObject() noexcept = 0;

  /// Destory the this instance
  virtual void DestoryRefCnt() noexcept = 0;

protected:
  /// The number of shared references to the object.
  /// When count equal 0, the object will be destory
  std::atomic<INT32> m_shareRefCnt;

  /// The number of weak references to the object.
  /// The count will see any shared references as one.
  std::atomic<INT32> m_weakRefCnt;

private:
  HAWL_DISABLE_COPY(RefCntBase)
};

} // !SmartPtr

} // !Hawl