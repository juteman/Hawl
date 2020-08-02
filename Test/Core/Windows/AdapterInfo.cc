#include "DX12Device.h"

int main(int argc, char *argv[])
{
    Hawl::DX12Device testAdapter;
    testAdapter.CreateDXGIFactory6();
    testAdapter.GetHardwareAdapter();
    testAdapter.LogAdapter();
}
