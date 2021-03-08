#include "DX12Device.h"

#include "BaseType.h"
#include "D3D12Desc.h"
#include "DX12Helper.h"
#include "Logger.h"
#include <cassert>


void GetHardwareAdapter(IDXGIFactory1 *pFactory,
                        IDXGIAdapter1 **ppAdapter,
                        bool needHighPerformance)
{
    *ppAdapter = nullptr;

    IDXGIAdapter1 *adapter;

    IDXGIFactory6 *factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                needHighPerformance == true
                    ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
                    : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

    uint flags = 0;
#if GRAPHICS_DEBUG
    flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CHECK_DX12RESULT(CreateDXGIFactory2(flags, IID_PPV_ARGS(&pDXGIFactory6)))

    bool           foundSuitAbleGpu = false;
    IDXGIAdapter4 *adapter4 = nullptr;
    bool           foundSoftwareAdapter = false;
    GpuDesc        gpuDesc;
    // Find number of usable GPUs
    // Use DXGI6 interface which lets us specify gpu preference so we dont need to use NVOptimus or AMDPowerExpress exports
    for (uint i = 0; DXGI_ERROR_NOT_FOUND != pDXGIFactory6->EnumAdapterByGpuPreference(i,
                         DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                         IID_PPV_ARGS(&adapter4)); ++i)
    {
        DXGI_ADAPTER_DESC3 desc{};
        adapter4->GetDesc3(&desc);

        // Ignore Microsoft Driver
        if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
        {
            for (uint32 level = 0; level < sizeof(featureLevels) / sizeof(featureLevels[0]); ++
                 level)
            {
                // Make sure the adapter can support a D3D12 device
                if (SUCCEEDED(
                    D3D12CreateDevice(adapter4, featureLevels[level], __uuidof(ID3D12Device),
                        nullptr)))
                {
                    // find adapter support D3D feature and adapter4 
                    HRESULT hres = adapter4->QueryInterface(IID_PPV_ARGS(&gpuDesc.pGpu));
                    if (SUCCEEDED(hres))
                    {
                        D3D12CreateDevice(adapter4, featureLevels[level], IID_PPV_ARGS(&pDxDevice));
                        FillGpuDesc(pDxDevice, featureLevels[level], gpuDesc);
                        foundSuitAbleGpu = true;
                        break;
                    }
                }
            }
        }
        else
        {
            foundSoftwareAdapter = true;
        }

        adapter4->Release();
    }

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(
                D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device),
                    nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter;
}
