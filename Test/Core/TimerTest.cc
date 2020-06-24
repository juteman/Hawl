#include <Timer.h>
#include <Windows.h>
#include <iostream>

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
  Sleep(1000);
  cout << lowTimer.GetElapsedTime(true) << endl;
  HTimer hTimer;
  Sleep(1000);
  cout << hTimer.GetElapsedTime(false) << endl;
  cout << hTimer.GetElapsedTimeAverage() << endl;
  cout << hTimer.GetSeconds(false) << endl;
  return 0;
}