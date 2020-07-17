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
#ifndef HAWL_THREAD_H
#  define HAWL_THREAD_H
#  include "BaseType.h"
#  include <string>
#  if PLATFORM_LINUX
#    include <pthread.h>
typedef pid_t ThreadID;
#  endif

namespace Hawl {

enum class ThreadPriority
{
  Lowest = 0,
  Low,
  Normal,
  High,
  Highest,
};

class ThreadRef
{
public:
#  if PLATFORM_LINUX
  typedef pthread_t ThreadType;
#  endif

  /// construct func
  constexpr ThreadRef()
    : m_id(0)
  {}

  explicit constexpr ThreadRef(ThreadType id)
    : m_id(id)
  {}

  bool operator==(ThreadRef& rhs) const { return m_id == rhs.m_id; }

  bool operator!=(ThreadRef& rhs) const { return m_id != rhs.m_id; }

  inline bool is_null() const { return m_id == 0; }

private:
  ThreadType m_id;
};

class ThreadHandle
{
public:
#  if PLATFORM_WIN32
  typedef void* Handle;
#  elif PLATFORM_LINUX
  typedef pthread_t Handle;
#  endif
  constexpr ThreadHandle()
    : m_handle(0)
  {}

  explicit constexpr ThreadHandle(Handle handle)
    : m_handle(handle)
  {}

  bool operator==(const ThreadHandle& rhs) const
  {
    return m_handle == rhs.m_handle;
  }

  bool is_null() const { return !m_handle; }

  Handle GetHandle() const { return m_handle; }

private:
  Handle m_handle;
};

class HawlThread
{
public:
  /// Create Thread
  static bool Create(size_t         stackSize,
                     ThreadHandle*  threadHandle,
                     ThreadPriority priority = ThreadPriority::Normal);

  /// set the thread priority
  /// @param threadPriority enum class about thread priority
  static void SetPriority(ThreadPriority threadPriority);

  /// set the current thread priority
  static ThreadPriority GetPriority();

  /// kill the thread
  /// @param waitToExit wait the facity to exit
  static void Exit(bool waitToExit);

  /// Get the thread id
  /// can use to log
  static ThreadID GetThreadID();

  /// Get the thread reference
  /// To check whether on the right thread
  static ThreadRef GetThreadRef();

  /// GetCurrentHandle can get the current thread
  static ThreadHandle GetCurrentHandle();

  /// Set the thread name
  /// @param name the name set to thread
  static void SetName(const std::string& name);

  /// Joins with a thread created
  static void Join(ThreadHandle threadHandle);

  /// Detaches and releases the thread handle
  static void Detach(ThreadHandle threadHandle);

protected:
};
}
#endif