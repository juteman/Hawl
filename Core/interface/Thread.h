/**
 *  Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#pragma once
#include "Algorithm/LockfreeQueue.h"
#include "BaseType.h"
#include "Common.h"
#include <string>
#include <thread>
#include <vector>

namespace Hawl
{
enum class Priority : int
{
    Lowest = 0,
    Low,
    Normal,
    High,
    Highest,
};


struct Task
{
    HAWL_DISABLE_COPY(Task)
    Task() = default;
    virtual ~Task() = default;
    virtual void run() = 0;
    Priority taskPriority = Priority::Normal;
};


class ThreadPool
{
private:
    using Queue = Algorithm::LockFreeQueue<Task *>;

protected:
    const uint MaxThreadCount = std::thread::hardware_concurrency() / 2;
    Queue TaskQueue;
    std::vector<std::thread> m_threads;

    void TaskRunner()
    {
        while (!TaskQueue.IsEmpty())
        {
            Task *realTask;
            // If DeQueue return error here, the task queue is empty
            if (!TaskQueue.DeQueue(realTask))
                break;
            realTask->run();
            //TODO notify on thread here
        }
    }

public:
    virtual ~ThreadPool() = default;

    /**
     * \brief Specify the thread number size and priority of thread the create thread pool
     * \param numOfThreads the number of thread to use in the pool
     * \param threadPriority priority in the pool thread
     * \return true for success create thread pool
     */
    virtual bool Create(uint32 numOfThreads, Priority threadPriority);

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
};
} // namespace Hawl
