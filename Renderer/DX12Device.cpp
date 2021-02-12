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
    else
    {
        for (UINT adapterIndex = 0;
             DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex
        )
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

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
