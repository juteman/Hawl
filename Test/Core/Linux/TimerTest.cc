#include <Timer.h>
#include <atomic>
#include <iostream>
#include "Logger.h"
using namespace std;
using namespace Hawl;
int
main()
{
  atomic<int> num            = 1;
  FLOAT32     fixedDeltaTime = 0.01f;
  HTimer      hTimer;

  while (true) {
    FLOAT32 deltaTime = hTimer.GetSeconds(false);
    if (deltaTime >= fixedDeltaTime) {
      Logger::info("{}", deltaTime);
      hTimer.Reset();
    }
  }

  if (--num == 0)
    cout << "atomic build true!!" << endl;
  return 0;
}