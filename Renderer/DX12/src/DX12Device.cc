#include "DX12Device.h"
#include "BaseType.h"
#include "DX12helper.h"
#include <Windows.h>
#include <d3d12.h>
#include <iostream>
#include <string>
#include <vector>
namespace Hawl
{
// Feature Level List of Direct3D
const static D3D_FEATURE_LEVEL D3DFeatureLevels[] = {
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
};

void DX12Device::CreateDXGIFactory6(bool isDebug)
{
    const UINT32 flags = isDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
    CHECK_DX12_RESULT(
        CreateDXGIFactory2(flags, IID_PPV_ARGS(m_dxgiFactory6.InitAndGetAddressOf())));
}

void DX12Device::CreateDevice(bool isDebug)
{

    auto CreateMaxFeatureLevel = [&](const D3D_FEATURE_LEVEL *pFeatureLevels,
                                     UINT32                   featureLevelCount) -> bool {
        for (uint32_t i = 0; i < featureLevelCount; i++)
        {
            if (SUCCEEDED(D3D12CreateDevice(m_dxgiAdapter4.Get(),
                                            pFeatureLevels[i],
                                            IID_PPV_ARGS(m_device4.InitAndGetAddressOf()))))
            {
                adapterDesc.maxFeatureSupported = pFeatureLevels[i];
                return true;
            }
        }

        return false;
    };
    // Create the factory and get the
    CreateDXGIFactory6(isDebug);
    GetHardwareAdapter();
    bool result = CreateMaxFeatureLevel(D3DFeatureLevels, ArraySize(D3DFeatureLevels));
    if (result && m_device4 != nullptr)
    {
        // TODO should be instead of logger
        std::cout << "success create device" << std::endl;
    }
}

void DX12Device::GetHardwareAdapter()
{
    // Enum the adapter by Gpu power
    for (UINT32 i = 0;
         m_dxgiFactory6->EnumAdapterByGpuPreference(
             i,
             DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
             IID_PPV_ARGS(m_dxgiAdapter4.InitAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND;
         i++)
    {
        DXGI_ADAPTER_DESC3 desc{};

        CHECK_DX12_RESULT(m_dxgiAdapter4->GetDesc3(&desc));

        // No soft render
        if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
        {
            // if support level Dx11 , the choose the adapter
            if (SUCCEEDED(D3D12CreateDevice(
                    m_dxgiAdapter4.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
            {
                adapterDesc.info = desc;
                break;
            }
        }
    }

    // Else no adapter match
    // TODO May choose soft render here
    if (!m_dxgiAdapter4)
    {
        // TODO log here
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
