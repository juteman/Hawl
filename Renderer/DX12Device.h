#pragma once
#include "BaseType.h"

#include <d3d12.h>
#include <dxgi1_6.h>    

class DX12Device
{
public:
    DX12Device(uint32 bEnableGpuBasedValidation);
    ~DX12Device();

    ID3D12Device *   pDxDevice;
    ID3D12Debug *    pDXDebug;
    IDXGIFactory6 *  pDXGIFactory6;
    ID3D12InfoQueue *pDxDebugValidation;


};
