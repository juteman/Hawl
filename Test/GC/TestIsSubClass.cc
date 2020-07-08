#include <GC/Triats.h>
#include <iostream>
using namespace Hawl;

template<typename T>
class Base1
{};
template<typename T1, typename T2>
class Basev2
{};
class DerivedSingle : public Base1<int>
{};
class DerivedTwo : public Basev2<int, int>
{};
int
main()
{
  DerivedSingle single;
  std::cout << IsSubclassOfTemplate<DerivedSingle, Base1>::value << std::endl;
  std::cout
    << IsSubclassOfTemplate<std::remove_cv<DerivedTwo>::type, Basev2>::value
    << std::endl;
}