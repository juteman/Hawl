/**
 *  Copyright 2020 juteman
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
#include "BaseType.h"
#include "Common.h"
#include <pthread.h>
namespace Hawl
{
namespace Linux
{
/// Linux pthread critical section Class
class CriticalSection
{
    HAWL_DISABLE_COPY(CriticalSection)
  public:
    /// set the Mutex attributes
    inline CriticalSection()
    {
        pthread_mutexattr_t mutexAttributes;
        pthread_mutexattr_init(&mutexAttributes);
        // set the mutex as recursive
        pthread_mutexattr_settype(&mutexAttributes, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_mutex, &mutexAttributes);
        pthread_mutexattr_destroy(&mutexAttributes);
    }

    inline ~CriticalSection()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    /// lock the critical section
    inline void Lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    /**
     * \brief try to take a lock
     * \return True if success take a lock
     */
    inline bool TryLock()
    {
        return 0 == pthread_mutex_trylock(&m_mutex);
    }

    /// release the critical section
    inline void Unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

  private:
    pthread_mutex_t m_mutex;
};

} // namespace Linux
} // namespace Hawl
