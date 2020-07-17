#include "Thread.h"
#include <pthread.h>

namespace Hawl {

bool
HawlThread::Create(size_t         stackSize,
                   ThreadHandle*  threadHandle,
                   ThreadPriority priority)
{
  pthread_attr_t attributes;
  pthread_attr_init(&attributes);

  // pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);

  if (stackSize == 0) {
#if !defined(THREAD_SANITIZER)
    return 0;
#else
    return 2 * (1 << 23); // 2 times the default stack size on Linux
#endif
  }

  // set the stack size
  if (stackSize > 0)
    pthread_attr_setstacksize(&attributes, stackSize);

  /// TODO Create thread with func event
  return 0; // need to return
}

ThreadHandle
HawlThread::GetCurrentHandle()
{
  return ThreadHandle(pthread_self());
}

ThreadRef
HawlThread::GetThreadRef()
{
  return ThreadRef(pthread_self());
}

}