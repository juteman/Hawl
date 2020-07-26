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
#include <type_traits>
#include <utility>

namespace Hawl::SmartPtr {

/// base reference counter class
/// include about share reference counter and the
/// weak reference counter
class RefCntUtilityBase
{
public:
  inline explicit RefCntUtilityBase(UINT32 shareCnt = 1,
                                    UINT32 weakCnt  = 1) noexcept
  {
    m_shareRefCnt.store(shareCnt);
    m_weakRefCnt.store(weakCnt);
  }

  virtual ~RefCntUtilityBase() noexcept {}

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
  inline RefCntUtilityBase* ConditionallyAddShareRefCnt()
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
        [[likely]]
        {
          ++m_weakRefCnt;
          return this;
        }
    }
    return nullptr;
  }

  /// Release the share reference
  inline void ReleaseSharedRef()
  {
    assert((m_shareRefCnt.load(std::memory_order_relaxed) > 0) &&
           (m_weakRefCnt.load(std::memory_order_relaxed) > 0));
    if (--m_shareRefCnt == 0)
      DestroyObject();

    if (--m_weakRefCnt) {
      DestroyRefCnt();
    }
  }

  inline void AddWeakReference() { ++m_weakRefCnt; }

  inline void ReleaseWeakReference()
  {
    assert(m_weakRefCnt.load() > 0);
    if (--m_weakRefCnt == 0) {
      DestroyRefCnt();
    }
  }

  /// Destroy the contained object
  virtual void DestroyObject() noexcept = 0;

  /// Destroy the this instance
  virtual void DestroyRefCnt() noexcept = 0;

protected:
  /// The number of shared references to the object.
  /// When count equal 0, the object will be destroy
  std::atomic<INT32> m_shareRefCnt;

  /// The number of weak references to the object.
  /// The count will see any shared references as one.
  std::atomic<INT32> m_weakRefCnt;

  HAWL_DISABLE_COPY(RefCntUtilityBase)
};

/// Deleter type of reference counter utility to
/// delete the contained object
template<typename T, typename DeleterType>
class RefCntDeleter : public RefCntUtilityBase
{
public:
  /// rename the type
  typedef T ValueType;

  ValueType   m_value; // this type expected to be a pointer
  DeleterType m_deleter;

  RefCntDeleter(ValueType value, DeleterType deleter)
    : m_value{ value }
    , m_deleter{ std::move(deleter) }
  {}

  void DestroyObject() noexcept override
  {
    m_deleter(m_value);
    m_value = nullptr;
  }

  void DestroyRefCnt() noexcept override { this->~RefCntDeleter(); }

  // ReSharper disable once CppCStyleCast
  void* GetDeleter() const noexcept { return (void*)m_deleter; }
};

/// RefCntInst is used to hold the instance of T
/// To allocate the object and count in a single memory
template<typename T>
class RefCntInst : public RefCntUtilityBase
{
public:
  typedef T                                                          ValueType;
  typedef typename std::aligned_storage<sizeof(T), alignof(T)>::type StorgeType;

  mutable StorgeType m_memory;

  ValueType* GetValue()
  {
    return static_cast<ValueType*>(static_cast<void*>(&m_memory));
  }

  template<typename... Args>
  RefCntInst(Args&&... args)
    : RefCntUtilityBase{}
  {
    new (&m_memory) ValueType(std::forward<Args>(args)...);
  }

  void DestroyObject() noexcept override { GetValue()->~ValueType(); }

  void DestroyRefCnt() noexcept override { this->~RefCntInst(); }

  void* GetDeleter() const noexcept { return nullptr; }
};

/// The default deleter
template<typename T>
struct DefaultDeleter
{
  inline void operator()(T* object) const { delete object; }
};

/// Create with default deleter
template<typename ValueType>
inline RefCntUtilityBase*
NewDefaultRefCnt(ValueType* object)
{
  return new RefCntDeleter<ValueType, DefaultDeleter<ValueType>>(
    object, DefaultDeleter<ValueType>());
}

// Create with custom deleter
template<typename ValueType, typename DeleterType>
inline RefCntUtilityBase*
NewCustomRefCnt(ValueType* object, DeleterType&& deleter)
{
  return new RefCntDeleter<ValueType,
                           typename std::remove_reference_t<DeleterType>::Type>(
    object, std::forward<DeleterType>(deleter));
}
}
