/**
 *  Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#pragma once

#include "BaseType.h"
#include "Common.h"
#include "EASTL/vector.h"
#include "TextureFormat.h"
#include "D3D12MemAlloc.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Hawl
{


class alignas(64) RendererImpl
{
public:
    Renderer(const RendererDesc &desc);
    HAWL_DISABLE_COPY(Renderer);
    ~Renderer() = default;

#if D3D12_SUPPORTED
    [[nodiscard]] FORCEINLINE ID3D12Debug *&GetDxDebug()
    {
        return pDXDebug;
    }


    /**
     * \brief get local variable mEnableGpuBasedValidation by this function
     * \return reference to mEnableGpuBasedValidation
     * \note return reference value can be changed outside, it is not const!
     */
    [[nodiscard]] FORCEINLINE uint32 &IsEnableGpuBasedValidation()
    {
        return mEnableGpuBasedValidation;
    }

    [[nodiscard]] FORCEINLINE IDXGIFactory6 * &GetFactory6()
    {
        return pDXGIFactory;
    }
#endif


private:
#if D3D12_SUPPORTED
    DescriptorHeap **pCPUDescriptorHeaps = nullptr;
    DescriptorHeap **pCbvSrvUavHeaps = nullptr;
    DescriptorHeap **pSamplerHeaps = nullptr;

    IDXGIFactory6 *           pDXGIFactory = nullptr;
    IDXGIAdapter4 *           pDxActiveGPU = nullptr;
    ID3D12Device *            pDxDevice = nullptr;
    class D3D12MA::Allocator *pResourceAllocator = nullptr;
    ID3D12Debug *             pDXDebug = nullptr;
    ID3D12InfoQueue *         pDxDebugValidation = nullptr;
#endif
#if DRED_ENABLE
    ID3D12DeviceRemovedExtendedDataSettings* pDredSettings;
#endif
    uint64           mPadA;
    NullDescriptors *pNullDescriptors = nullptr;
    GPUSettings *    pActiveGpuSettings = nullptr;
    ShaderMacro *    pBuiltinShaderDefines = nullptr;
    GPUCapBits *     pCapBits = nullptr;
    uint32 mLinkedNodeCount : 4{};
    uint32 mGpuMode : 3{};
    uint32 mShaderTarget : 4{};
    uint32 mApi : 5{};
    uint32 mEnableGpuBasedValidation : 1{};
    uint32 mBuiltinShaderDefinesCount = 0;
};

struct GpuDesc
{
    Renderer *                        pRenderer = NULL;
    IDXGIAdapter4 *                   pGpu = NULL;
    D3D_FEATURE_LEVEL                 mMaxSupportedFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
    D3D12_FEATURE_DATA_D3D12_OPTIONS  mFeatureDataOptions;
    D3D12_FEATURE_DATA_D3D12_OPTIONS1 mFeatureDataOptions1;
    SIZE_T                            mDedicatedVideoMemory = 0;
    char                              mVendorId[MAX_GPU_VENDOR_STRING_LENGTH];
    char                              mDeviceId[MAX_GPU_VENDOR_STRING_LENGTH];
    char                              mRevisionId[MAX_GPU_VENDOR_STRING_LENGTH];
    char                              mName[MAX_GPU_VENDOR_STRING_LENGTH];
    GPUPresetLevel                    mPreset;
};
} // namespace Hawl
