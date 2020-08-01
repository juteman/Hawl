#include "DX12Device.h"
#include "BaseType.h"
#include "RenderDesc.h"
#include "DX12helper.h"
#include <d3d12.h>
#include <vector>
namespace Hawl
{
typedef struct GpuDesc
{
    IDXGIAdapter4 *                   pGPU = nullptr;
    D3D_FEATURE_LEVEL                 MaxSupportFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
    D3D12_FEATURE_DATA_D3D12_OPTIONS  featureDataOptions;
    D3D12_FEATURE_DATA_D3D12_OPTIONS1 FeatureDataOptions1;
    char                              venderID[MAX_GPU_VENDOR_STRING_LENGTH];
    char                              deviceID[MAX_GPU_VENDOR_STRING_LENGTH];
    char                              revisionId[MAX_GPU_VENDOR_STRING_LENGTH];
    char                              name[MAX_GPU_VENDOR_STRING_LENGTH];
    GPUPresetLevel                    preset;
} GpuDesc;

void DX12Device::CreateDXGIFactory6(bool isDebug)
{
    const UINT32 flags = isDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
    CHECK_DX12_RESULT(CreateDXGIFactory2(flags, IID_PPV_ARGS(m_dxgiFactory6.InitAndGetAddressOf())));
}

void DX12Device::CreateDevice(bool isDebug)
{
    CreateDXGIFactory6(isDebug);
}

void DX12Device::GetHardwareAdapter()
{

    std::vector<GpuDesc> gpuDesc;
    IDXGIAdapter4 *      adapter4 = nullptr;
    for (UINT32 i = 0;
         m_dxgiFactory6->EnumAdapterByGpuPreference(
             i,
             DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
             IID_PPV_ARGS(m_dxgiAdapter.InitAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND;
         i++)
    {
        DXGI_ADAPTER_DESC3 desc{};
        adapter4->GetDesc3(&desc);

        if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
        {

            if (SUCCEEDED(D3D12CreateDevice(
                    m_dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_1, __uuidof(ID3D12Device), nullptr)))
            {
            }
        }
    }
}
} // namespace Hawl
} // namespace Hawl