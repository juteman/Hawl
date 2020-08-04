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
#include <string>

namespace Hawl
{
// Set the Thread Id on different platform
#if PLATFORM_WIN32
typedef DWORD ThreadId;
typedef void *ThreadHandle;
#elif PLATFORM_LINUX
typedef pthread_t ThreadId;
typedef pthread_t ThreadHandle;
#endif

enum class ThreadPriority
{
    Lowest = 0,
    Low,
    Normal,
    High,
    Highest,
};

class HawlThread
{
    HAWL_DISABLE_COPY(HawlThread)
  public:
    HawlThread() = default;

    /**
     * \brief Get the current thread ID
     */
    static ThreadId GetThreadId();

    /**
     * \brief Get the current thread handle
     */
    static ThreadHandle GetHandle();

    /**
     * \brief Create a new thread
     * \param threadHandle handle used to get created thread handle
     * \param stackSize 0 to use default stack size
     * \param threadPriority the priority of the thread
     * \return True if success create thread, else for some error
     */
    static bool Create(ThreadHandle* threadHandle, size_t stackSize, ThreadPriority threadPriority);
};

} // namespace Hawl