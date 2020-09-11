#include "Thread.h"
#include "Logger.h"
namespace Hawl
{
bool ThreadPool::Create(UINT32 numOfThreads, Priority threadPriority)
{
    if(numOfThreads > MaxThreadCount)
    {
        Logger::warn("Create to many thread in thread pool {}.", __FILE__);
    }

    for(int i; i < numOfThreads; i++)
        m_threads.push_back(std::thread( [this]{ this->TaskRunner(); } ));
}

} // namespace Hawl