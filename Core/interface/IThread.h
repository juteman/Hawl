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
#ifndef HAWL_ITHREAD_H
#  define HAWL_ITHREAD_H
#  include "BaseType.h"
namespace Hawl {

enum class ThreadPriority
{
  Lowest = 0,
  Low,
  Normal,
  High,
  Highest,
};

class IThread
{
public:
  IThread();

  virtual ~IThread();
  /// 创建线程
  virtual void Create() = 0;

  /// 设置线程的优先级
  /// @param threadPriority 线程的优先级
  virtual void SetPriority(ThreadPriority threadPriority) = 0;

  /// 终止线程
  /// @param waitToExit 等待线程终止后退出
  virtual void Exit(bool waitToExit) = 0;

  inline UINT32 GetThreadID() { return m_threadID; }

protected:
  UINT32 m_threadID;

  ThreadPriority m_threadPriority;
};
}
#endif