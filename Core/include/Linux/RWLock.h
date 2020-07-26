/*
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
#ifndef HAWL_LINUX_RWLOCK_H
#  define HAWL_LINUX_RELOCK_H
#  include <assert.h>
#  include <cstdio>
#  include <errno.h>
#  include <pthread.h>
#  include <string.h>

namespace Hawl::Linux
{
/// Linux pthread read and write mutex class
class RWLock
{
public:
  RWLock()
  {
    int err = pthread_rwlock_init(&m_RWmutex, nullptr);
    assert(err == 0);
  }

  ~RWLock()
  {
    int err = pthread_rwlock_destroy(&m_RWmutex);
    assert(err == 0);
  };

  /// get the read lock
  void ReadLock()
  {
    int err = pthread_rwlock_rdlock(&m_RWmutex);
    assert(err == 0);
  }

  /// get the write lock
  void WriteLock()
  {
    int err = pthread_rwlock_wrlock(&m_RWmutex);
    assert(err == 0);
  }

  // unlock
  void Unlock()
  {
    int err = pthread_rwlock_unlock(&m_RWmutex);
    assert(err == 0);
  }

private:
  pthread_rwlock_t m_RWmutex;
};
}
#endif
