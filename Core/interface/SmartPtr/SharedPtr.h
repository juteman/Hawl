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
#include <type_traits>

namespace Hawl::SmartPtr {
/// SharePtrTraits is to solve the problem
/// about the operate*() return a reference to T
/// if T is void ,it need to return void. But not *void
template<typename T>
struct SharedPtrTraits
{
  typedef T& referenceType;
};

template<>
struct SharedPtrTraits<void>
{
  typedef void referenceType;
};

template<>
struct SharedPtrTraits<void const>
{
  typedef void referenceType;
};

template<>
struct SharedPtrTraits<void volatile>
{
  typedef void referenceType;
};

template<>
struct SharedPtrTraits<void const volatile>
{
  typedef void referenceType;
};

/// SharePtr reference-counted authoritative object pointer.
/// It will be thread safe because the counter is atomic
template<typename T>
class SharedPtr
{
public:
  typedef SharedPtr<T>                               ThisType;
  typedef T                                          ObjectType;
  typedef typename SharedPtrTraits<T>::referenceType ReferenceType;

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
             std::is_convertible<U*, ObjectType*>::value>::type>
  explicit SharedPtr(U* pValue)
    : m_pRefCnt{ NewDefaultRefCnt(pValue) }
    , m_pObject{ pValue }
  {}

  /// Take the ownership of the pointer
  /// and set the reference count with custom deleter
  template<typename U,
           typename DeleterType,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  SharedPtr(U* pValue, DeleterType&& deleter)
    : m_pRefCnt{ NewCustomRefCnt(pValue, std::forward<DeleterType>(deleter)) }
    , m_pObject{ pValue }
  {}

  template<typename DeleterType>
  SharedPtr(std::nullptr_t, DeleterType deleter)
    : m_pRefCnt{ NewCustomRefCnt(nullptr, std::forward<DeleterType>(deleter)) }
    , m_pObject{ nullptr }
  {}

  /// SharePtr constructor with self type
  /// This function will trigger reference shared counter increments
  [[maybe_unused]] SharedPtr(const SharedPtr& sharedPtr)
    : m_pRefCnt{ sharedPtr.m_pRefCnt }
    , m_pObject{ sharedPtr.m_pObject }
  {
    if (m_pRefCnt)
      m_pRefCnt->AddShareRefCnt();
  }

  /// SharedPtr constructor with another instance of SharedPtr
  /// This function will trigger reference shared counter increments
  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  SharedPtr(const SharedPtr<U>& sharedPtr)
    : m_pRefCnt{ sharedPtr.m_pRefCnt }
    , m_pObject{ sharedPtr.m_pObject }
  {
    if (m_pRefCnt)
      m_pRefCnt->AddShareRefCnt();
  }

  /// Shares ownership of a pointer with another instance of
  /// sharedPtr while storing a potentially different pointer.
  template<typename U>
  SharedPtr(const SharedPtr<U>& sharedPtr, ObjectType* pObject) noexcept
    : m_pRefCnt{ sharedPtr.m_pRefCnt }
    , m_pObject{ pObject }
  {
    if (m_pRefCnt)
      m_pRefCnt->AddShareRefCnt();
  }

  /// SharedPtr constructor with right reference
  SharedPtr(SharedPtr&& sharedPtr) noexcept
    : m_pRefCnt{ sharedPtr.m_pRefCnt }
    , m_pObject{ sharedPtr.m_pObject }
  {
    sharedPtr.m_pObject = nullptr;
    sharedPtr.m_pRefCnt = nullptr;
  }

  /// WeakPtr constructor
  /// TODO

  /// Deconstruct
  ~SharedPtr()
  {
    if (m_pRefCnt)
      m_pRefCnt->ReleaseSharedRef();
  }

  /// operator assignment to self type
  SharedPtr& operator=(const SharedPtr& sharedPtr) noexcept
  {
    // if sharedPtr is not equal to this
    // just swap the pointer
    if (&sharedPtr != this)
      ThisType(sharedPtr).Swap(*this);
    return *this;
  }

  /// Copy another SharedPtr to this object.
  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  SharedPtr& operator=(const SharedPtr<U>& sharedPtr) noexcept
  {
    if (!EqualOwnership(sharedPtr))
      ThisType(sharedPtr).Swap(*this);
    return *this;
  }

  /// Copy with right reference
  SharedPtr& operator=(SharedPtr&& sharedPtr) noexcept
  {
    if (&sharedPtr != this)
      ThisType(std::move(sharedPtr)).Swap(*this);

    return *this;
  }

  /// Reset and releases the owned pointer.
  void Reset() noexcept { ThisType().Swap(*this); }

  /// reset and  releases the owned pointer and takes ownership of the
  /// passed in pointer.
  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  void Reset(U* pObject)
  {
    ThisType(pObject).Swap(*this);
  }

  /// reset and  releases the owned pointer and takes ownership of the
  /// passed in pointer.
  template<typename U,
           typename Deleter,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  void Reset(U* pObject, Deleter deleter)
  {
    ThisType(pObject, deleter).Swap(*this);
  }

  /// To exchange SharedPtr information
  void Swap(ThisType& sharedPtr)
  {
    ObjectType* const pObject = sharedPtr.m_pObject;
    sharedPtr.m_pObject       = m_pObject;
    m_pObject                 = pObject;

    RefCntUtilityBase* const pCntUtilityBase = sharedPtr.m_pRefCnt;
    sharedPtr.m_pRefCnt                      = m_pRefCnt;
    m_pRefCnt                                = pCntUtilityBase;
  }

  /// @return true if the given SharedPtr own the same T pointer that we do.
  template<typename U>
  bool EqualOwnership(const SharedPtr<U>& sharedPtr) const
  {
    // Compare address of m_pRefCnt
    return m_pRefCnt = sharedPtr.m_pRefCnt;
  }

  /// Returns the owner pointer dereferenced
  /// @example:
  ///  SharedPtr<int> ptr(new int(3));
  ///  int x = *ptr;
  ReferenceType operator*() const noexcept { return *m_pObject; }

  /// Allows access to the owned pointer via ->
  /// @example
  ///  struct X{ void fun(); };
  ///  SharedPtr<int> ptr(new X);
  ///  ptr->fun();
  ReferenceType* operator->() const noexcept
  {
    assert(m_pObject);
    return m_pObject;
  }

  explicit operator bool() const noexcept { return (m_pObject != nullptr); }

  /// Get method
  ///@return  the pointer of owned pointer
  ReferenceType* Get() const noexcept { return m_pObject; }

  /// Count method
  /// @return the number of shared pointer
  [[nodiscard]] INT32 Count() const noexcept
  {
    return m_pRefCnt ? m_pRefCnt->GetSharedRefCnt() : 0;
  }

  /// Unique method
  /// @return true if just one shared pointer
  [[nodiscard]] bool Unique() const noexcept
  {
    return (m_pRefCnt && (m_pRefCnt->GetSharedRefCnt() == 1));
  }

protected:
  /// TODO: some operater overloading
};

template<typename T>
class WeakPtr
{
public:
  typedef WeakPtr<T> ThisType;
  typedef T          ObjectType;

public:
  WeakPtr() noexcept
    : m_pObject{ nullptr }
    , m_pRefCnt{ nullptr }
  {}

  /// Constructor with self type
  WeakPtr(const ThisType& weakPtr) noexcept
    : m_pObject{ weakPtr.m_pObject }
    , m_pRefCnt{ weakPtr.m_pRefCnt }
  {
    if (m_pRefCnt)
      m_pRefCnt->AddWeakReference();
  }

  /// Move constructor with self type
  WeakPtr(const ThisType&& weakPtr) noexcept
    : m_pObject{ weakPtr.m_pObject }
    , m_pRefCnt{ weakPtr.m_pRefCnt }
  {
    weakPtr.m_pObject = nullptr;
    weakPtr.m_pRefCnt = nullptr;
  }

  /// Constructor
  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  WeakPtr(const WeakPtr<U>& weakPtr) noexcept
    : m_pObject{ weakPtr.m_pObject }
    , m_pRefCnt{ weakPtr.m_pRefCnt }
  {
    if (m_pRefCnt)
      m_pRefCnt->AddWeakReference();
  }

  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  WeakPtr(const WeakPtr<U>&& weakPtr) noexcept
    : m_pObject{ weakPtr.m_pObject }
    , m_pRefCnt{ weakPtr.m_pRefCnt }
  {
    weakPtr.m_pObject = nullptr;
    weakPtr.m_pRefCnt = nullptr;
  }

  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  WeakPtr(const SharedPtr<U>& sharedPtr)
    : m_pObject{ sharedPtr.m_pObject }
    , m_pRefCnt{ sharedPtr.m_pRefCnt }
  {
    if (m_pRefCnt)
      m_pRefCnt->AddWeakReference();
  }

  ~WeakPtr()
  {
    if (m_pRefCnt)
      m_pRefCnt->ReleaseWeakReference();
  }

  ThisType& operator=(const ThisType& weakPtr) noexcept
  {
    Assign(weakPtr);
    return *this;
  }

  ThisType& operator=(const ThisType&& weakPtr) noexcept
  {
    WeakPtr(std::move(weakPtr)).Swap(*this);
    return *this;
  }

  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  ThisType operator=(const WeakPtr<U>& weakPtr) noexcept
  {
    Assign(weakPtr);
    return *this;
  }

  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  ThisType operator=(const WeakPtr<U>&& weakPtr) noexcept
  {
    WeakPtr(std::move(weakPtr)).Swap(*this);
    return *this;
  }

  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  ThisType operator=(const SharedPtr<U>& sharedPtr) noexcept
  {
    if (m_pRefCnt != sharedPtr.m_pRefCnt) {
      if (m_pRefCnt)
        m_pRefCnt->ReleaseWeakReference();

      m_pObject = sharedPtr.m_pObject;
      m_pRefCnt = sharedPtr.m_pRefCnt;
      if (m_pRefCnt)
        m_pRefCnt->ReleaseWeakReference();
    }
    return *this;
  }

  SharedPtr<T> lock() const noexcept
  {
    SharedPtr<T> temp;
    temp.m_pRefCnt =
      m_pRefCnt ? m_pRefCnt->ConditionallyAddShareRefCnt() : m_pRefCnt;
    if (temp.m_pRefCnt)
      temp.m_pObject = m_pObject;
    return temp;
  }

  template<typename U,
           typename = typename std::enable_if<
             std::is_convertible<U*, ObjectType*>::value>::type>
  void Assign(const WeakPtr<U>& weakPtr)
  {
    if (m_pRefCnt != weakPtr.m_pRefCnt) {
      if (m_pRefCnt)
        m_pRefCnt->ReleaseWeakReference();

      m_pObject = weakPtr.m_pObject;
      m_pRefCnt = weakPtr.m_pRefCnt;
      if (m_pRefCnt)
        m_pRefCnt->ReleaseWeakReference();
    }
  }

  /// Swap the weakPtr
  void Swap(ThisType& weakPtr)
  {
    ObjectType* const pObject = weakPtr.m_pObject;
    weakPtr.m_pObject         = m_pObject;
    m_pObject                 = pObject;

    RefCntUtilityBase* const pCntUtilityBase = weakPtr.m_pRefCnt;
    weakPtr.m_pRefCnt                        = m_pRefCnt;
    m_pRefCnt                                = pCntUtilityBase;
  }

protected:
  /// Weak owned pointer
  ObjectType* m_pObject;
  /// Reference count for ownd pointer
  RefCntUtilityBase* m_pRefCnt;

  /// Set other type of WeakPtr and SharedPtr
  /// as friend class
  template<typename U>
  friend class WeakPtr;

  template<typename U>
  friend class SharedPtr;
};
}
