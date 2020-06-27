#include <iostream>

class A
{
public:
  virtual void Init();
  virtual void print();
};

void
A::Init()
{
  print();
}

void
A::print()
{
  std::cout << "A print func" << std::endl;
}

class B : A
{
public:
  void Init() override;
};

void
B::Init()
{
  B::print();
  A::Init();
}

int
main()
{
  B b;
  b.Init();
  return 0;
}