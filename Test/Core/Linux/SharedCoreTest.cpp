#include "EASTL/utility.h"
#include "Logger.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"

void* operator new[](size_t size, const char* pName, int flags, unsigned     debugFlags, const char* file, int line)
{
    return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return malloc(size);
}

int main()
{
    using namespace eastl;
    vector<string> test;
    string b = "BDD";
    move(b);
    Hawl::Logger::info("{}", b.data());
    return 0;
}


