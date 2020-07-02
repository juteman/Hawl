#include <Timer.h>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace Hawl;
int
main()
{
  INT64  uSec = getUSec();
  UINT32 mSec = getMSec();
  cout << uSec << endl;
  cout << mSec << endl;
  LTimer lowTimer;
  usleep(1000000);
  cout << lowTimer.GetElapsedTime(true) << endl;
  HTimer hTimer;
  usleep(1000000);
  cout << hTimer.GetElapsedTime(false) << endl;
  cout << hTimer.GetElapsedTimeAverage() << endl;
  cout << hTimer.GetSeconds(false) << endl;
  return 0;
}