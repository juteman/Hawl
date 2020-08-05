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
#include <thread>
#include "Algorithm/LockfreeQueue.h"
namespace Hawl
{
typedef std::jthread Thread;

class Task
{
    HAWL_DISABLE_COPY(Task)
  public:
    Task() = default;
    virtual ~Task() = default;

    /// Real work thread to run
    virtual void run() = 0;
};

enum class ThreadPriority : int
{
    Lowest = 0,
    Low,
    Normal,
    High,
    Highest,
};

/// Set the priority of thread
static bool SetThreadPriority(Thread &thread, ThreadPriority priority);

class ThreadPool
{
  private:
    using Queue = Algorithm::LockFreeQueue<Task*>;

  public:
    virtual ~ThreadPool() = default;

    /**
     * \brief Specify the thread number size and priority of thread the create thread pool
     * \param numOfThreads the number of thread to use in the pool
     * \param stackSize the size of stack of thread pool (default as 32k)
     * \param threadPriority priority in the pool thread
     * \return true for success create thread pool
     */
    virtual bool Create(UINT32         numOfThreads,
                        UINT32         stackSize = (32 * 1024),
                        ThreadPriority threadPriority = ThreadPriority::Normal) = 0;

    /**
     * \brief Clean all the thread in pool and destroy the pool
     */
    virtual void Destroy() = 0;

    /**
     * \brief If have free thread, dispatch task to thread
     * \param task work need to be done
     */
    virtual void AddTask(Task *task) = 0;

    /**
     * \brief Retract the previously task
     * \param task try to retract
     */
    virtual void RetractTask(Task *task) = 0;

    /**
     * \brief Allocate a thread pool
     * @return a new thread pool
     */
    static ThreadPool* Allocate();

  protected:


};


} // namespace Hawl