/*
 * Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 * Copyright (c) 2018-2021 The Forge Interactive Inc.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *  and base of the Direct3D12.cpp
 *	in The-Forge (see https://github.com/ConfettiFX/The-Forge)
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/


#include "D3D12Hooks.h"
#include "DX12Helper.h"
#include "GPUHelper.h"

#include <d3d12.h>
#include "EASTL/vector.h"
#include "EAAssert/eaassert.h"
#include "mimalloc.h"

#include <amd_ags.h>


#if defined(XBOX)
// 
#else
#define IID_ARGS IID_PPV_ARGS
#endif

#define SAFE_FREE(p_var)                \
	if ((p_var))                        \
	{                                   \
		mi_free((void*)(p_var));        \
		p_var = nullptr;                \
	}

#if defined(__cplusplus)
#define DECLARE_ZERO(type, var) type var = {};
#else
#define DECLARE_ZERO(type, var) type var = { 0 };
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p_var)     \
	if (p_var)                  \
	{                           \
		(p_var)->Release();     \
		p_var = nullptr;        \
	}
#endif


//
// internal functions start with a capital letter / API starts with a small letter

// internal functions are capital first letter and capital letter of the next word

// Functions points for functions that need to be loaded
PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER fnD3D12CreateRootSignatureDeserializer = nullptr;
PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE fnD3D12SerializeVersionedRootSignature = nullptr;
PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER
fnD3D12CreateVersionedRootSignatureDeserializer = nullptr;

typedef enum GpuVendor
{
    GPU_VENDOR_NVIDIA,
    GPU_VENDOR_AMD,
    GPU_VENDOR_INTEL,
    GPU_VENDOR_UNKNOWN,
    GPU_VENDOR_COUNT,
} GpuVendor;

static uint32_t gRootSignatureDWORDS[GpuVendor::GPU_VENDOR_COUNT] =
{
    64U,
    13U,
    64U,
    64U,
};

#define VENDOR_ID_NVIDIA 0x10DE
#define VENDOR_ID_AMD 0x1002
#define VENDOR_ID_AMD_1 0x1022
#define VENDOR_ID_INTEL 0x163C
#define VENDOR_ID_INTEL_1 0x8086
#define VENDOR_ID_INTEL_2 0x8087

static GpuVendor util_to_internal_gpu_vendor(UINT vendorId)
{
    if (vendorId == VENDOR_ID_NVIDIA)
        return GPU_VENDOR_NVIDIA;
    else if (vendorId == VENDOR_ID_AMD || vendorId == VENDOR_ID_AMD_1)
        return GPU_VENDOR_AMD;
    else if (vendorId == VENDOR_ID_INTEL || vendorId == VENDOR_ID_INTEL_1 || vendorId ==
             VENDOR_ID_INTEL_2)
        return GPU_VENDOR_INTEL;
    else
        return GPU_VENDOR_UNKNOWN;
}

static bool AddDevice(Renderer *pRenderer)
{
    // The D3D debug layer (as well as Microsoft PIX and other graphics debugger
    // tools using an injection library) is not compatible with Nsight Aftermath.
    // If Aftermath detects that any of these tools are present it will fail initialization.
#if defined(GRAPHICS_DEBUG) && !defined(USE_NSIGHT_AFTERMATH)
    //add debug layer if in debug mode
    if (SUCCEEDED(
        D3D12GetDebugInterface(__uuidof(pRenderer->pDXDebug), reinterpret_cast<void **>(&(pRenderer
            ->pDXDebug)))))
    {
        hook_enable_debug_layer(pRenderer);
    }
#endif

#if defined(USE_NSIGHT_AFTERMATH)
	// Enable Nsight Aftermath GPU crash dump creation.
	// This needs to be done before the Vulkan device is created.
	CreateAftermathTracker(pRenderer->pName, &pRenderer->mAftermathTracker);
#endif

#if defined(USE_DRED)
	SUCCEEDED(D3D12GetDebugInterface(__uuidof(pRenderer->pDredSettings), (void**)&(pRenderer->pDredSettings)));

	// Turn on AutoBreadcrumbs and Page Fault reporting
	pRenderer->pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	pRenderer->pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
#endif

    D3D_FEATURE_LEVEL feature_levels[4] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

#if defined(XBOX)
	// Create the DX12 API device object.
	CHECK_HRESULT(hook_create_device(NULL, feature_levels[0], &pRenderer->pDxDevice));

#if defined(GRAPHICS_DEBUG)
    //Sets the callback functions to invoke when the GPU hangs
    //pRenderer->pDxDevice->SetHangCallbacksX(HANGBEGINCALLBACK, HANGPRINTCALLBACK, NULL);
#endif

	// First, retrieve the underlying DXGI device from the D3D device.
	IDXGIDevice1* dxgiDevice;
	CHECK_HRESULT(pRenderer->pDxDevice->QueryInterface(IID_ARGS(&dxgiDevice)));

	// Identify the physical adapter (GPU or card) this device is running on.
	IDXGIAdapter* dxgiAdapter;
	CHECK_HRESULT(dxgiDevice->GetAdapter(&dxgiAdapter));

	// And obtain the factory object that created it.
	CHECK_HRESULT(dxgiAdapter->GetParent(IID_ARGS(&pRenderer->pDXGIFactory)));

	uint32_t gpuCount = 1;
	GpuDesc gpuDesc[1] = {};
	dxgiAdapter->QueryInterface(IID_ARGS(&gpuDesc[0].pGpu));

	dxgiAdapter->Release();
	typedef bool(*DeviceBetterFn)(GpuDesc* gpuDesc, uint32_t testIndex, uint32_t refIndex);
	DeviceBetterFn isDeviceBetter = [](GpuDesc* gpuDesc, uint32_t testIndex, uint32_t refIndex) -> bool
	{
		return false;
	};

	hook_fill_gpu_desc(pRenderer, feature_levels[0], &gpuDesc[0]);

#else
    // PC Device create start
    UINT flags = 0;
#if defined(GRAPHICS_DEBUG)
    flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    CHECK_DX12RESULT(CreateDXGIFactory2(flags, IID_ARGS(&pRenderer->pDXGIFactory)))

    uint32_t       gpuCount = 0;
    IDXGIAdapter4 *adapter = nullptr;
    bool           foundSoftwareAdapter = false;

    // Find number of usable GPUs
    // Use DXGI6 interface which lets us specify gpu preference so we don't need to use NVOptimus or AMDPowerExpress exports
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != pRenderer->pDXGIFactory->EnumAdapterByGpuPreference(i,
                         DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                         IID_ARGS(&adapter)); ++i)
    {
        DECLARE_ZERO(DXGI_ADAPTER_DESC3, desc)
        adapter->GetDesc3(&desc);

        // Ignore Microsoft Soft Renderer Driver
        if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
        {
            for (uint32_t level = 0; level < sizeof(feature_levels) / sizeof(feature_levels[0]); ++
                 level)
            {
                // Make sure the adapter can support a D3D12 device
                if (SUCCEEDED(
                        D3D12CreateDevice(adapter, feature_levels[level], __uuidof(ID3D12Device),
                            NULL))
                )
                {
                    GpuDesc gpuDesc = {};
                    HRESULT hres = adapter->QueryInterface(IID_ARGS(&gpuDesc.pGpu));
                    if (SUCCEEDED(hres))
                    {
                        SAFE_RELEASE(gpuDesc.pGpu);
                        ++gpuCount;
                        break;
                    }
                }
            }
        }
        else
        {
            foundSoftwareAdapter = true;
        }

        adapter->Release();
    }

    // If the only adapter we found is a software adapter, log error message for QA
    if (!gpuCount && foundSoftwareAdapter)
    {
        Logger::error("The only available GPU has DXGI_ADAPTER_FLAG_SOFTWARE. Early exiting");
        EA_ASSERT(false);
        return false;
    }

    EA_ASSERT(gpuCount);
    auto *gpuDesc = static_cast<GpuDesc *>(alloca(gpuCount * sizeof(GpuDesc)));
    memset(gpuDesc, 0, gpuCount * sizeof(GpuDesc));
    gpuCount = 0;

    // Use DXGI6 interface which lets us specify gpu preference so we don't need to use NVOptimus or AMDPowerExpress exports
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != pRenderer->pDXGIFactory->EnumAdapterByGpuPreference(i,
                         DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                         IID_ARGS(&adapter)); ++i)
    {
        DECLARE_ZERO(DXGI_ADAPTER_DESC3, desc)
        adapter->GetDesc3(&desc);

        // Ignore Microsoft Driver
        if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
        {
            for (uint32_t level = 0; level < sizeof(feature_levels) / sizeof(feature_levels[0]); ++
                 level)
            {
                // Make sure the adapter can support a D3D12 device
                if (SUCCEEDED(
                        D3D12CreateDevice(adapter, feature_levels[level], __uuidof(ID3D12Device),
                            nullptr))
                )
                {
                    HRESULT hres = adapter->QueryInterface(IID_ARGS(&gpuDesc[gpuCount].pGpu));
                    if (SUCCEEDED(hres))
                    {
                        D3D12CreateDevice(adapter,
                                          feature_levels[level],
                                          IID_PPV_ARGS(&pRenderer->pDxDevice));
                        hook_fill_gpu_desc(pRenderer, feature_levels[level], &gpuDesc[gpuCount]);
                        //get preset for current gpu description
                        gpuDesc[gpuCount].mPreset = GPU_PRESET_ULTRA;
                        // TODO Write a function to get the GPU PRESET level
                        /* getGPUPresetLevel(
                            gpuDesc[gpuCount].mVendorId,
                            gpuDesc[gpuCount].mDeviceId,
                            gpuDesc[gpuCount].mRevisionId); */

                        ++gpuCount;
                        SAFE_RELEASE(pRenderer->pDxDevice);
                        break;
                    }
                }
            }
        }

        adapter->Release();
    }

    EA_ASSERT(gpuCount > 0);

    typedef bool (*DeviceBetterFn)(GpuDesc *gpuDesc, uint32_t testIndex, uint32_t refIndex);
    DeviceBetterFn isDeviceBetter = [](GpuDesc *gpuDesc,
                                       uint32_t testIndex,
                                       uint32_t refIndex) -> bool
    {
        const GpuDesc &gpu1 = gpuDesc[testIndex];
        const GpuDesc &gpu2 = gpuDesc[refIndex];

        // force to an Intel, useful sometimes for debugging
        //		if(stricmp(gpu1.mVendorId, "0x8086") == 0 )
        //			return true;

        // If shader model 6.0 or higher is requested, prefer the GPU which supports it
        if (gpu1.pRenderer->mShaderTarget >= shader_target_6_0)
        {
            if (gpu1.mFeatureDataOptions1.WaveOps != gpu2.mFeatureDataOptions1.WaveOps)
                return gpu1.mFeatureDataOptions1.WaveOps;
        }

        // Next check for higher preset
        if (static_cast<int>(gpu1.mPreset) != static_cast<int>(gpu2.mPreset))
        {
            return gpu1.mPreset > gpu2.mPreset;
        }

        // Check feature level first, sort the greatest feature level gpu to the front
        if (static_cast<int>(gpu1.mMaxSupportedFeatureLevel) != static_cast<int>(gpu2.
                mMaxSupportedFeatureLevel))
        {
            return gpu1.mMaxSupportedFeatureLevel > gpu2.mMaxSupportedFeatureLevel;
        }

        return gpu1.mDedicatedVideoMemory > gpu2.mDedicatedVideoMemory;
    };

#endif

    uint32 gpuIndex = UINT32_MAX;
    auto * gpuSettings = static_cast<GPUSettings *>(alloca(gpuCount * sizeof(GPUSettings)));

    for (uint32_t i = 0; i < gpuCount; ++i)
    {
        gpuSettings[i] = {};
        gpuSettings[i].mUniformBufferAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        gpuSettings[i].mUploadBufferTextureAlignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
        gpuSettings[i].mUploadBufferTextureRowAlignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
        gpuSettings[i].mMultiDrawIndirect = true;
        gpuSettings[i].mMaxVertexInputBindings = 32U;

        //assign device ID
        strncpy_s(gpuSettings[i].mGpuVendorPreset.mModelId,
                  gpuDesc[i].mDeviceId,
                  MAX_GPU_VENDOR_STRING_LENGTH);
        //assign vendor ID
        strncpy_s(gpuSettings[i].mGpuVendorPreset.mVendorId,
                  gpuDesc[i].mVendorId,
                  MAX_GPU_VENDOR_STRING_LENGTH);
        //assign Revision ID
        strncpy_s(gpuSettings[i].mGpuVendorPreset.mRevisionId,
                  gpuDesc[i].mRevisionId,
                  MAX_GPU_VENDOR_STRING_LENGTH);
        //get name from api
        strncpy_s(gpuSettings[i].mGpuVendorPreset.mGpuName,
                  gpuDesc[i].mName,
                  MAX_GPU_VENDOR_STRING_LENGTH);
        //get preset
        gpuSettings[i].mGpuVendorPreset.mPresetLevel = gpuDesc[i].mPreset;
        //get wave lane count
        gpuSettings[i].mWaveLaneCount = gpuDesc[i].mFeatureDataOptions1.WaveLaneCountMin;
        gpuSettings[i].mROVsSupported = gpuDesc[i].mFeatureDataOptions.ROVsSupported ? true : false;
        gpuSettings[i].mTessellationSupported = gpuSettings[i].mGeometryShaderSupported = true;
        gpuSettings[i].mWaveOpsSupportFlags = WAVE_OPS_SUPPORT_FLAG_ALL;

        // Determine root signature size for this gpu driver
        DXGI_ADAPTER_DESC adapterDesc;
        gpuDesc[i].pGpu->GetDesc(&adapterDesc);
        gpuSettings[i].mMaxRootSignatureDWORDS = gRootSignatureDWORDS[util_to_internal_gpu_vendor(
            adapterDesc.VendorId)];
        Logger::info(
            "GPU[{}] detected. Vendor ID: {}, Model ID: {}, Revision ID: {}, Preset: {}, GPU Name: {}",
            i,
            adapterDesc.VendorId,
            adapterDesc.DeviceId,
            adapterDesc.Revision,
            PresetLevelToString(gpuSettings[i].mGpuVendorPreset.mPresetLevel),
            adapterDesc.Description);

        // Check that gpu supports at least graphics
        if (gpuIndex == UINT32_MAX || isDeviceBetter(gpuDesc, i, gpuIndex))
        {
            gpuIndex = i;
        }
    }

    // Get the latest and greatest feature level gpu
    CHECK_DX12RESULT(gpuDesc[gpuIndex].pGpu->QueryInterface(IID_ARGS(&pRenderer->pDxActiveGPU)))
    EA_ASSERT(pRenderer->pDxActiveGPU != nullptr);
    pRenderer->pActiveGpuSettings = static_cast<GPUSettings *>(mi_malloc(sizeof(GPUSettings)));
    *pRenderer->pActiveGpuSettings = gpuSettings[gpuIndex];

    for (uint32_t i = 0; i < gpuCount; ++i)
    {
        SAFE_RELEASE(gpuDesc[i].pGpu);
    }

    // Print selected GPU information
    Logger::info("GPU[{}] is selected as default GPU", gpuIndex);
    Logger::info("Name of selected gpu: {}",
                 pRenderer->pActiveGpuSettings->mGpuVendorPreset.mGpuName);
    Logger::info("Vendor id of selected gpu: {}",
                 pRenderer->pActiveGpuSettings->mGpuVendorPreset.mVendorId);
    Logger::info("Model id of selected gpu: {}",
                 pRenderer->pActiveGpuSettings->mGpuVendorPreset.mModelId);
    Logger::info("Revision id of selected gpu: {}",
                 pRenderer->pActiveGpuSettings->mGpuVendorPreset.mRevisionId);
    Logger::info("Preset of selected gpu: {}",
                 PresetLevelToString(pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel));

    // Load functions
    {
        HMODULE module = hook_get_d3d12_module_handle();

        fnD3D12CreateRootSignatureDeserializer =
            reinterpret_cast<PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER>(GetProcAddress(
                module,
                "D3D12SerializeVersionedRootSignature"));

        fnD3D12SerializeVersionedRootSignature =
            reinterpret_cast<PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE>(GetProcAddress(
                module,
                "D3D12SerializeVersionedRootSignature"));

        fnD3D12CreateVersionedRootSignatureDeserializer =
            reinterpret_cast<PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER>(GetProcAddress(
                module,
                "D3D12CreateVersionedRootSignatureDeserializer"));
    }

#if !defined(XBOX)
    CHECK_DX12RESULT(D3D12CreateDevice(pRenderer->pDxActiveGPU,
        gpuDesc[gpuIndex].mMaxSupportedFeatureLevel,
        IID_ARGS(&pRenderer->pDxDevice)))
#endif

#if defined(USE_NSIGHT_AFTERMATH)
	SetAftermathDevice(pRenderer->pDxDevice);
#endif

#if EA_PLATFORM_DESKTOP && defined(_DEBUG)
    HRESULT hr = pRenderer->pDxDevice->QueryInterface(IID_ARGS(&pRenderer->pDxDebugValidation));
    if (SUCCEEDED(hr))
    {
        pRenderer->pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pRenderer->pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // D3D12_MESSAGE_ID_LOADPIPELINE_NAMENOTFOUND breaks even when it is disabled
        pRenderer->pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
        hr = pRenderer->pDxDebugValidation->SetBreakOnID(D3D12_MESSAGE_ID_LOADPIPELINE_NAMENOTFOUND,
                                                         false);
    }
#endif

    return true;
}


/************************************************************************/
// Renderer Init Remove
/************************************************************************/
void initRenderer(const char *appName, const RendererDesc *pDesc, Renderer **ppRenderer)
{
    EA_ASSERT(appName);
    EA_ASSERT(pDesc);
    EA_ASSERT(ppRenderer);

    Renderer *pRenderer = (Renderer *)mi_calloc_aligned(1, alignof(Renderer), sizeof(Renderer));
    EA_ASSERT(pRenderer);

    pRenderer->mGpuMode = pDesc->mGpuMode;
    pRenderer->mShaderTarget = pDesc->mShaderTarget;
    pRenderer->mEnableGpuBasedValidation = pDesc->mEnableGPUBasedValidation;
#if defined(XBOX)
	pRenderer->mApi = RENDERER_API_XBOX_D3D12;
#else
    pRenderer->mApi = RENDERER_API_D3D12;
#endif

    pRenderer->pName = (char *)mi_calloc(strlen(appName) + 1, sizeof(char));
    strcpy(pRenderer->pName, appName);

    // Initialize the D3D12 bits
    {
        AGSReturnCode agsRet = agsInit();
        if (AGSReturnCode::AGS_SUCCESS == agsRet)
        {
            agsPrintDriverInfo();
        }

        NvAPI_Status nvStatus = nvapiInit();
        if (NvAPI_Status::NVAPI_OK == nvStatus)
        {
            nvapiPrintDriverInfo();
        }

        if (!AddDevice(pRenderer))
        {
            *ppRenderer = NULL;
            return;
        }

#if !defined(XBOX)
        //anything below LOW preset is not supported and we will exit
        if (pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel < GPU_PRESET_LOW)
        {
            //have the condition in the assert as well so its cleared when the assert message box appears

            ASSERT(pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel >= GPU_PRESET_LOW);

            SAFE_FREE(pRenderer->pName);

            //remove device and any memory we allocated in just above as this is the first function called
            //when initializing the forge
            RemoveDevice(pRenderer);
            SAFE_FREE(pRenderer);
            LOGF(LogLevel::eERROR, "Selected GPU has an Office Preset in gpu.cfg.");
            LOGF(LogLevel::eERROR, "Office preset is not supported by The Forge.");

            //return NULL pRenderer so that client can gracefully handle exit
            //This is better than exiting from here in case client has allocated memory or has fallbacks
            *ppRenderer = NULL;
            return;
        }

        utils_caps_builder(pRenderer);

        if (pRenderer->mShaderTarget >= shader_target_6_0)
        {
            // Query the level of support of Shader Model.
            D3D12_FEATURE_DATA_SHADER_MODEL   shaderModelSupport = {D3D_SHADER_MODEL_6_0};
            D3D12_FEATURE_DATA_D3D12_OPTIONS1 waveIntrinsicsSupport = {};
            if (!SUCCEEDED(pRenderer->pDxDevice->CheckFeatureSupport(
                (D3D12_FEATURE)D3D12_FEATURE_SHADER_MODEL, &shaderModelSupport, sizeof(
                    shaderModelSupport))))
            {
                return;
            }
            // Query the level of support of Wave Intrinsics.
            if (!SUCCEEDED(pRenderer->pDxDevice->CheckFeatureSupport(
                (D3D12_FEATURE)D3D12_FEATURE_D3D12_OPTIONS1, &waveIntrinsicsSupport, sizeof(
                    waveIntrinsicsSupport))))
            {
                return;
            }

            // If the device doesn't support SM6 or Wave Intrinsics, try enabling the experimental feature for Shader Model 6 and creating the device again.
            if (shaderModelSupport.HighestShaderModel != D3D_SHADER_MODEL_6_0 ||
                waveIntrinsicsSupport.WaveOps != TRUE)
            {
                RENDERDOC_API_1_1_2 *rdoc_api = NULL;
                // At init, on windows
                if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
                {
                    pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(
                        mod,
                        "RENDERDOC_GetAPI");
                    RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
                }

                // If RenderDoc is connected shader model 6 is not detected but it still works
                if (!rdoc_api || !rdoc_api->IsTargetControlConnected())
                {
                    // If the device still doesn't support SM6 or Wave Intrinsics after enabling the experimental feature, you could set up your application to use the highest supported shader model.
                    // For simplicity we just exit the application here.
                    if (shaderModelSupport.HighestShaderModel < D3D_SHADER_MODEL_6_0 ||
                        (waveIntrinsicsSupport.WaveOps != TRUE && !SUCCEEDED(
                             EnableExperimentalShaderModels())))
                    {
                        RemoveDevice(pRenderer);
                        LOGF(LogLevel::eERROR, "Hardware does not support Shader Model 6.0");
                        return;
                    }
                }
                else
                {
                    LOGF(LogLevel::eWARNING,
                         "\nRenderDoc does not support SM 6.0 or higher. Application might work but you won't be able to debug the SM 6.0+ "
                         "shaders or view their bytecode.");
                }
            }
        }
#endif
        /************************************************************************/
        // Multi GPU - SLI Node Count
        /************************************************************************/
        uint32_t gpuCount = pRenderer->pDxDevice->GetNodeCount();
        pRenderer->mLinkedNodeCount = gpuCount;
        if (pRenderer->mLinkedNodeCount < 2)
            pRenderer->mGpuMode = GPU_MODE_SINGLE;
        /************************************************************************/
        // Descriptor heaps
        /************************************************************************/
        pRenderer->pCPUDescriptorHeaps = (DescriptorHeap **)tf_malloc(
            D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES * sizeof(DescriptorHeap *));
        pRenderer->pCbvSrvUavHeaps = (DescriptorHeap **)tf_malloc(
            pRenderer->mLinkedNodeCount * sizeof(DescriptorHeap *));
        pRenderer->pSamplerHeaps = (DescriptorHeap **)tf_malloc(
            pRenderer->mLinkedNodeCount * sizeof(DescriptorHeap *));

        for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Flags = gCpuDescriptorHeapProperties[i].mFlags;
            desc.NodeMask = 0; // CPU Descriptor Heap - Node mask is irrelevant
            desc.NumDescriptors = gCpuDescriptorHeapProperties[i].mMaxDescriptors;
            desc.Type = (D3D12_DESCRIPTOR_HEAP_TYPE)i;
            add_descriptor_heap(pRenderer->pDxDevice, &desc, &pRenderer->pCPUDescriptorHeaps[i]);
        }

        // One shader visible heap for each linked node
        for (uint32_t i = 0; i < pRenderer->mLinkedNodeCount; ++i)
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            desc.NodeMask = util_calculate_node_mask(pRenderer, i);

            desc.NumDescriptors = 1 << 16;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            add_descriptor_heap(pRenderer->pDxDevice, &desc, &pRenderer->pCbvSrvUavHeaps[i]);

            desc.NumDescriptors = 1 << 11;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            add_descriptor_heap(pRenderer->pDxDevice, &desc, &pRenderer->pSamplerHeaps[i]);
        }
        /************************************************************************/
        // Memory allocator
        /************************************************************************/
        D3D12MA::ALLOCATOR_DESC desc = {};
        desc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
        desc.pDevice = pRenderer->pDxDevice;
        desc.pAdapter = pRenderer->pDxActiveGPU;

        D3D12MA::ALLOCATION_CALLBACKS             allocationCallbacks = {};
        allocationCallbacks.pAllocate = [](size_t size, size_t alignment, void *)
        {
            return tf_memalign(alignment, size);
        };
        allocationCallbacks.pFree = [](void *ptr, void *)
        {
            tf_free(ptr);
        };
        desc.pAllocationCallbacks = &allocationCallbacks;
        CHECK_HRESULT(D3D12MA::CreateAllocator(&desc, &pRenderer->pResourceAllocator));
    }
    /************************************************************************/
    /************************************************************************/
    add_default_resources(pRenderer);

    hook_post_init_renderer(pRenderer);

    // Set shader macro based on runtime information
    ShaderMacro rendererShaderDefines[] =
    {
        // Descriptor set indices
        {"UPDATE_FREQ_NONE", "space0"},
        {"UPDATE_FREQ_PER_FRAME", "space1"},
        {"UPDATE_FREQ_PER_BATCH", "space2"},
        {"UPDATE_FREQ_PER_DRAW", "space3"},
    };
    pRenderer->mBuiltinShaderDefinesCount =
        sizeof(rendererShaderDefines) / sizeof(rendererShaderDefines[0]);
    pRenderer->pBuiltinShaderDefines = (ShaderMacro *)tf_calloc(
        pRenderer->mBuiltinShaderDefinesCount,
        sizeof(ShaderMacro));
    for (uint32_t i = 0; i < pRenderer->mBuiltinShaderDefinesCount; ++i)
    {
        pRenderer->pBuiltinShaderDefines[i] = rendererShaderDefines[i];
    }

    // Renderer is good!
    *ppRenderer = pRenderer;
}
