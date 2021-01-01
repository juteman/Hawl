#include "DX12Device.h"

#include "BaseType.h"
#include "D3D12Desc.h"
#include "DX12Helper.h"
#include "Logger.h"
#include <cassert>

namespace Hawl
{
FORCEINLINE void EnableDebugLayer(ID3D12Debug *pDXDebug, const uint32 bEnableGpuBasedValidation)
{
    pDXDebug->EnableDebugLayer();
    ID3D12Debug1 *pDebug1 = nullptr;
    if (SUCCEEDED(pDXDebug->QueryInterface(IID_PPV_ARGS(&pDebug1))))
    {
        pDebug1->SetEnableGPUBasedValidation(bEnableGpuBasedValidation);
        pDebug1->Release();
    }
}

void FillGpuDesc(ID3D12Device *&pDxDevice, const D3D_FEATURE_LEVEL &featureLevel, GpuDesc &gpuDesc)
{
    // Query the level of support of Shader Model.
    D3D12_FEATURE_DATA_D3D12_OPTIONS  featureData = {};
    D3D12_FEATURE_DATA_D3D12_OPTIONS1 featureData1 = {};
    // Query the level of support of Wave Intrinsics.
    pDxDevice->CheckFeatureSupport(
        static_cast<D3D12_FEATURE>(D3D12_FEATURE_D3D12_OPTIONS),
        &featureData,
        sizeof(featureData));
    pDxDevice->CheckFeatureSupport(
        static_cast<D3D12_FEATURE>(D3D12_FEATURE_D3D12_OPTIONS1),
        &featureData1,
        sizeof(featureData1));

    DXGI_ADAPTER_DESC3 desc = {};
    gpuDesc.pGpu->GetDesc3(&desc);
    gpuDesc.mMaxSupportedFeatureLevel = featureLevel;
    gpuDesc.mDedicatedVideoMemory = desc.DedicatedVideoMemory;
    gpuDesc.mFeatureDataOptions = featureData;
    gpuDesc.mFeatureDataOptions1 = featureData1;

    gpuDesc.mDeviceId = desc.DeviceId;
    gpuDesc.mVendorId = desc.VendorId;
    gpuDesc.mRevisionId = desc.Revision;

    size_t numConverted = 0;
    wcstombs_s(&numConverted, gpuDesc.mName, desc.Description, MAX_GPU_VENDOR_STRING_LENGTH);
}

DeviceImpl::DeviceImpl(uint32 bEnableGpuBasedValidation)
{
#if GRAPHICS_DEBUG
    if (SUCCEEDED(
        D3D12GetDebugInterface(__uuidof(pDXDebug), reinterpret_cast<void **>(&(pDXDebug)))))
    {
        EnableDebugLayer(pDXDebug, bEnableGpuBasedValidation);
    }
#endif

    // TODO DRED_ENABLE used for debug, needed add and deconstructed function both
#if DRED_ENABLE
#endif

    D3D_FEATURE_LEVEL featureLevels[4] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    uint flags = 0;
#if GRAPHICS_DEBUG
    flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CHECK_DX12_RESULT(CreateDXGIFactory2(flags, IID_PPV_ARGS(&pDXGIFactory6)))

    bool           foundSuitAbleGpu = false;
    IDXGIAdapter4 *adapter4 = nullptr;
    bool           foundSoftwareAdapter = false;
    GpuDesc        gpuDesc;
    // Find number of usable GPUs
    // Use DXGI6 interface which lets us specify gpu preference so we dont need to use NVOptimus or AMDPowerExpress exports
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != pDXGIFactory6->EnumAdapterByGpuPreference(i,
                         DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                         IID_PPV_ARGS(&adapter4)); ++i)
    {
        DXGI_ADAPTER_DESC3 desc{};
        adapter4->GetDesc3(&desc);

        // Ignore Microsoft Driver
        if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
        {
            for (uint32_t level = 0; level < sizeof(featureLevels) / sizeof(featureLevels[0]); ++
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

    // If the only adapter we found is a software adapter, log error message for QA
    if (!foundSuitAbleGpu && foundSoftwareAdapter)
    {
        Logger::error("The only available GPU has DXGI_ADAPTER_FLAG_SOFTWARE. Early exiting");
        assert(false);
    }

    Logger::info("Name of selected gpu: {}", gpuDesc.mName);
    Logger::info("Vendor id of selected gpu: {}", gpuDesc.mVendorId);
    Logger::info("Revision id of selected gpu: {}", gpuDesc.mRevisionId);
    Logger::info("Dedicated video memory of selected gpu: {}", gpuDesc.mDedicatedVideoMemory);
#if DEBUG
    HRESULT hr = pDxDevice->QueryInterface(IID_PPV_ARGS(&pDxDebugValidation));
    if (SUCCEEDED(hr))
    {
        pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // D3D12_MESSAGE_ID_LOADPIPELINE_NAMENOTFOUND breaks even when it is disabled
        pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
        pDxDebugValidation->SetBreakOnID(D3D12_MESSAGE_ID_LOADPIPELINE_NAMENOTFOUND, false);
    }
#endif
}

DeviceImpl::~DeviceImpl()
{
    SAFE_RELEASE(pDXGIFactory6);
    if (pDxDebugValidation)
    {
        pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
        pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
        pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
        SAFE_RELEASE(pDxDebugValidation);
    }
#if DEBUG
    ID3D12DebugDevice *pDebugDevice = nullptr;
    pDxDevice->QueryInterface(&pDebugDevice);

    SAFE_RELEASE(pDXDebug);
    SAFE_RELEASE(pDxDevice);

    if (pDebugDevice)
    {
        // Debug device is released first so report live objects don't show its ref as a warning.
        pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
        pDebugDevice->Release();
    }
#else
    SAFE_RELEASE(pDxDevice);
#endif

#if DRED_ENABLE
#endif
}
} // namespace Hawl
