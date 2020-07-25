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

#include "ReferenceCounterBase.h"
#include <cstdlib>
#include <type_traits>
namespace Hawl {
namespace SmartPtr {

/// SharePtrTraits is to solve the problem
/// about the operate*() return a refernce to T
/// if T is void ,it need to return void. But not *void
template<typename T>
struct SharedPtrTraits
{
  typedef T& reference_type;
};

template<>
struct SharedPtrTraits<void>
{
  typedef void reference_type;
};

template<>
struct SharedPtrTraits<void const>
{
  typedef void reference_type;
};

template<>
struct SharedPtrTraits<void volatile>
{
  typedef void reference_type;
};

template<>
struct SharedPtrTraits<void const volatile>
{
  typedef void reference_type;
};

/// SharePtr reference-counted authoritative object pointer.
/// It will be thread safe because the counter is atomic
template<typename T>
class SharedPtr
{
public:
  typedef SharedPtr<T> ThisType;
  typedef T            ValueType;

protected:
  RefCntUtilityBase* m_pRefCnt;
  T*                 m_pObject;

public:
  /// Init a blank SharePtr
  SharedPtr() noexcept
    : m_pRefCnt{ nullptr }
    , m_pObject{ nullptr }
  {}

  SharedPtr(std::nullptr_t) noexcept
    : m_pRefCnt{ nullptr }
    , m_pObject{ nullptr }
  {}

  /// Take the ownership of the pointer
  /// and set the reference count
  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ValueType*>::value>::type>
  explicit SharedPtr(U* pValue)
    : m_pObject{ U }
    , m_pRefCnt{ NewDefaultRefCnt(pValue) }
  {}

  /// Take the ownership of the pointer
  /// and set the reference count with custom deleter
  template<typename U,
           typename DeleterType,
           typename = typename std::enable_if<
             std::is_convertible<U*, ValueType*>::value>::type>
  shared_ptr(U* pValue, DeleterType&& deleter)
    : m_pObject{ U }
    , m_pRefCnt{ NewCustomRefCnt(pValue, std::forward<DeleterType>(deleter)) }
  {}

  template<typename DeleterType>
  shared_ptr(std::nullptr_t, DeleterType deleter)
    : m_pValue(nullptr)
    , m_pRefCnt(NewCustomRefCnt(nullptr, std::forward<DeleterType>(deleter)))
  {}

  /// SharePtr counsturct with self type
  /// need to addition of this function
  /// TODO
};
} // !SmartPtr
} //! Hawl