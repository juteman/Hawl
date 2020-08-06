#include "tbb/tbb.h"
#include <iostream>
using namespace tbb;

void Foo(int &i)
{
    std::cout << "now counter" << i << std::endl;
}

class ApplyFoo
{
  public:
    void operator()(const blocked_range<int> &r) const
    {
        for (int i = r.begin(); i != r.end(); ++i)
            Foo(i);
    }
};

int main()
{
    parallel_for(blocked_range<int>(0, 10, 4), ApplyFoo(), simple_partitioner());
}