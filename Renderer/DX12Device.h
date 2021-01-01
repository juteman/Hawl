#pragma once
#include "BaseType.h"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace Hawl
{
class DeviceImpl
{
public:
    DeviceImpl(uint32 bEnableGpuBasedValidation);
    ~DeviceImpl();

private:
    ID3D12Debug *    pDXDebug;
    IDXGIFactory6 *  pDXGIFactory6;
    ID3D12Device *   pDxDevice;
    ID3D12InfoQueue *pDxDebugValidation;
};
}
