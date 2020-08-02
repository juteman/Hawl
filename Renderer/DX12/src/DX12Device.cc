#include "DX12Device.h"
#include "BaseType.h"
#include "DX12helper.h"
#include "RenderDesc.h"
#include <Windows.h>
#include <d3d12.h>
#include <iostream>
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
    CHECK_DX12_RESULT(
        CreateDXGIFactory2(flags, IID_PPV_ARGS(m_dxgiFactory6.InitAndGetAddressOf())));
}

void DX12Device::CreateDevice(bool isDebug)
{
    CreateDXGIFactory6(isDebug);
    GetHardwareAdapter();
    CHECK_DX12_RESULT(D3D12CreateDevice(m_dxgiAdapter4.Get(),
                                        D3D_FEATURE_LEVEL_11_0,
                                        IID_PPV_ARGS(m_device4.InitAndGetAddressOf())))
}

void DX12Device::GetHardwareAdapter()
{
    for (UINT32 i = 0;
         m_dxgiFactory6->EnumAdapterByGpuPreference(
             i,
             DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
             IID_PPV_ARGS(m_dxgiAdapter4.InitAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND;
         i++)
    {
        DXGI_ADAPTER_DESC3 desc{};

        CHECK_DX12_RESULT(m_dxgiAdapter4->GetDesc3(&desc));

        if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
        {

            if (SUCCEEDED(D3D12CreateDevice(
                    m_dxgiAdapter4.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
            {
                adapterDesc.info = desc;
                break;
            }
        }
    }

    if (!m_dxgiAdapter4)
    {
        MessageBox(nullptr, L"Can't not find Adapter support Dx12.", nullptr, 0);
        exit(0);
    }
}

void DX12Device::LogAdapter() const
{
    std::wstring text = L"***Adapter Description:";
    text += adapterDesc.info.Description;
    std::wcout << text << std::endl;
}

void DX12Device::LogAdapterOutPut()
{
    UINT         i = 0;
    IDXGIOutput *output = nullptr;
    while (m_dxgiAdapter4->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc{};
        CHECK_DX12_RESULT(output->GetDesc(&desc));
        adapterDesc.outputInfo.push_back(desc);
    }
}

} // namespace Hawl
