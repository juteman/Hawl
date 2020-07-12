#include "RefCntPtr.h"
#include <dxgi.h>
#include <iostream>

using namespace Hawl;

int
main()
{
  RefCountPtr<IDXGIAdapter> TempAdapter;
  TempAdapter.GetInitReference();

  return 0;
}