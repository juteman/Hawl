#include <Timer.h>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace Hawl;
int
main()
{
  FLOAT32 fixedDeltaTime = 0.01f;
  HTimer  hTimer;
  while (true) {
    FLOAT32 deltaTime = hTimer.GetSeconds(false);
    if (deltaTime >= fixedDeltaTime) {
      cout << deltaTime << endl;
      hTimer.Reset();
    }
  }

  return 0;
}