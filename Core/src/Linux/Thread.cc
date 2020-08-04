#include "Thread.h"
#include <pthread.h>

namespace Hawl {

bool HawlThread::Create(ThreadHandle *threadHandle, size_t stackSize, ThreadPriority threadPriority)
{
    // initialize pthread attribute
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    // if set the stack size is not 0
    // set the stack size
    // else the thread stack size is the default stack size (linux 8192K)
    if(stackSize > 0)
        pthread_attr_setstacksize(&attributes, stackSize);

    pthread_t handle;
    // TODO Create thread model here
}
}