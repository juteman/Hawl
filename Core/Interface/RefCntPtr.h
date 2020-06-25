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

private:
  T* m_pObject = nullptr;
};
}

#endif