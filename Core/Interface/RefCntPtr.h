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
#ifndef HAWL_REFCNTPTR_H
#  define HAWL_REFCNTPTR_H
#  include "HIID.h"
#  include "coreafx.h"
#  include <type_traits>
namespace Hawl {
class IObject
{
public:
  /// 查询该类型是否支持指定类型接口
  /// @param IID Hawl 接口标识符ID
  /// @param ppInterface 返回值
  /// 输入指定Interface ID，如果类型支持指定，返回接口的指针内存地址
  /// 否则返回 一个 空指针
  virtual void HAWL_CALLCONV QueryInterface(const HIID& IID,
                                            IObject**   ppInterface) = 0;
  /// 使引用计数加1
  /// 返回增加后的引用计数值
  /// @note 多线程环境下的引用计数不可靠，需要加锁
  virtual RefCount HAWL_CALLCONV AddRef() = 0;

  /// 使引用计数减1，若引用计数的值为0，则释放资源
  /// 返回减少后的引用计数值
  /// @note 多线程环境下的引用计数不可靠，需要加锁
  virtual RefCount HAWL_CALLCONV Release() = 0;
};

/// 引用计数智能指针的实现
/// @warning 此引用计数接口还未经过测试，使用有风险
/// @note RefCntPtr 参数必须包含IObject
template<typename T>
class RefCntPtr
{
public:
  RefCntPtr() noexcept {}

  explicit RefCntPtr(T* pObject) noexcept
    : m_pObject(pObject)
  {
    if (m_pObject)
      m_pObject->AddRef();
  }

  RefCntPtr(T* pObject, const HIID& IID) noexcept
    : m_pObject(pObject)
  {
    if (pObject)
      pObject->QueryInterface(IID, reinterpret_cast<IObject**>(&m_pObject));
  }

  RefCntPtr(const RefCntPtr& AutoPtr) noexcept
    : m_pObject{ AutoPtr.m_pObject }
  {
    if (m_pObject)
      m_pObject->AddRef();
  }

  /// 继承类型指针不会增加引用计数
  template<typename DerivedType,
           typename = typename std::enable_if<
             std::is_base_of<T, DerivedType>::value>::type>
  RefCntPtr(RefCntPtr<DerivedType>&& AutoPtr) noexcept
    : m_pObject{ std::move(AutoPtr.m_pObject) }
  {
    AutoPtr.m_pObject = nullptr;
  }

  ~RefCntPtr() { Release(); }

  void swap(RefCntPtr& AutoPtr) noexcept
  {
    std::swap(m_pObject, AutoPtr.m_pObject);
  }

  void Attach(T* pObject) noexcept
  {
    Release();
    m_pObject = pObject;
  }

  T* Detach() noexcept
  {
    T* pObject = m_pObject;
    m_pObject  = nullptr;
    return pObject;
  }

  void Release() noexcept
  {
    if (m_pObject) {
      m_pObject->Release();
      m_pObject = nullptr;
    }
  }

  RefCntPtr& operator=(T* pObject) noexcept
  {
    if (m_pObject != pObject) {
      if (m_pObject)
        m_pObject->Release();
      m_pObject = pObject;
      if (m_pObject)
        m_pObject->AddRef();
    }
    return *this;
  }

  RefCntPtr& operator=(const RefCntPtr& AutoPtr) noexcept
  {
    return *this = AutoPtr.m_pObject;
  }

  template<typename DerivedType,
           typename = typename std::enable_if<
             std::is_base_of<T, DerivedType>::value>::type>
  RefCntPtr& operator=(const RefCntPtr<DerivedType>& AutoPtr) noexcept
  {
    return *this = static_cast<T*>(AutoPtr.m_pObject);
  }

  RefCntPtr& operator=(RefCntPtr&& AutoPtr) noexcept
  {
    if (m_pObject != AutoPtr.m_pObject)
      Attach(AutoPtr.Detach());

    return *this;
  }

  template<typename DerivedType,
           typename = typename std::enable_if<
             std::is_base_of<T, DerivedType>::value>::type>
  RefCntPtr& operator=(RefCntPtr<DerivedType>&& AutoPtr) noexcept
  {
    if (m_pObject != AutoPtr.m_pObject)
      Attach(AutoPtr.Detach());

    return *this;
  }

  bool operator!() const noexcept { return m_pObject == nullptr; }

  operator bool() const noexcept { return m_pObject != nullptr; }

  bool operator==(const RefCntPtr& Ptr) const noexcept
  {
    return m_pObject == Ptr.m_pObject;
  }

  bool operator!=(const RefCntPtr& Ptr) const noexcept
  {
    return m_pObject != Ptr.m_pObject;
  }

  bool operator<(const RefCntPtr& Ptr) const noexcept
  {
    return static_cast<const T*>(*this) < static_cast<const T*>(Ptr);
  }

  T& operator*() noexcept { return *m_pObject; }

  const T& operator*() const noexcept { return *m_pObject; }

  T*       RawPtr() noexcept { return m_pObject; }
  const T* RawPtr() const noexcept { return m_pObject; }

  operator T*() noexcept { return RawPtr(); }
  operator const T*() const noexcept { return RawPtr(); }

  T*       operator->() noexcept { return m_pObject; }
  const T* operator->() const noexcept { return m_pObject; }

  template<typename InterfaceType>
  RefCntPtr<InterfaceType> Cast(const HIID& IID)
  {
    return RefCntAutoPtr<InterfaceType>{ m_pObject, IID };
  }

private:
  template<typename OtherType>
  friend class RefCntPtr;

  T* m_pObject = nullptr;
};
}

#endif