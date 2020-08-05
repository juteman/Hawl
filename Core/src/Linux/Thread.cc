#include "Thread.h"
#include <pthread.h>
namespace Hawl
{

bool SetThreadPriority(Thread &thread, ThreadPriority priority)
{
    sched_param temp{};
    temp.sched_priority = int(priority);
    if (pthread_setschedparam(thread.native_handle(), SCHED_OTHER, &temp))
        return false;
}
} // namespace Hawl