#pragma once
#ifndef HAWL_LINUX_CRITICALSECTION_H
#  define HAWL_LINUX_CRITICALSECTION_H
#  include "BaseType.h"
#  include <pthread.h>
namespace Hawl {
namespace Linux {
/// Linux下pthread的临界区封装
class CriticalSection
{
public:
  /// 禁用拷贝构造
  CriticalSection(const CriticalSection&) = delete;
  CriticalSection& operator=(const CriticalSection&) = delete;

  /// 设置互斥锁的类型
  inline CriticalSection()
  {
    pthread_mutexattr_t mutexAttributes;
    pthread_mutexattr_init(&mutexAttributes);
    // 设置互斥锁为可重复互斥锁
    pthread_mutexattr_settype(&mutexAttributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &mutexAttributes);
    pthread_mutexattr_destroy(&mutexAttributes);
  }

  inline ~CriticalSection() { pthread_mutex_destroy(&m_mutex); }

  /// 锁住临界区
  inline void Lock() { pthread_mutex_lock(&m_mutex); }

  /// 询问加锁
  inline INT TryLock() { pthread_mutex_trylock(&m_mutex); }

  ///释放临界区
  inline void Unlock() { pthread_mutex_unlock(&m_mutex); }

private:
  pthread_mutex_t m_mutex;
};

} // !Linux
} // !Hawl
#endif