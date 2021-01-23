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
#include <tbb/atomic.h>

#include "D3D12MemAlloc.h"
#include "math/Common.h"

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


#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN ((D3D12_GPU_VIRTUAL_ADDRESS)-1)


// clang-format off
D3D12_BLEND_OP gDx12BlendOpTranslator[BlendMode::MAX_BLEND_MODES] =
{
    D3D12_BLEND_OP_ADD,
    D3D12_BLEND_OP_SUBTRACT,
    D3D12_BLEND_OP_REV_SUBTRACT,
    D3D12_BLEND_OP_MIN,
    D3D12_BLEND_OP_MAX,
};

D3D12_BLEND gDx12BlendConstantTranslator[BlendConstant::MAX_BLEND_CONSTANTS] =
{
    D3D12_BLEND_ZERO,
    D3D12_BLEND_ONE,
    D3D12_BLEND_SRC_COLOR,
    D3D12_BLEND_INV_SRC_COLOR,
    D3D12_BLEND_DEST_COLOR,
    D3D12_BLEND_INV_DEST_COLOR,
    D3D12_BLEND_SRC_ALPHA,
    D3D12_BLEND_INV_SRC_ALPHA,
    D3D12_BLEND_DEST_ALPHA,
    D3D12_BLEND_INV_DEST_ALPHA,
    D3D12_BLEND_SRC_ALPHA_SAT,
    D3D12_BLEND_BLEND_FACTOR,
    D3D12_BLEND_INV_BLEND_FACTOR,
};

D3D12_COMPARISON_FUNC gDx12ComparisonFuncTranslator[CompareMode::MAX_COMPARE_MODES] =
{
    D3D12_COMPARISON_FUNC_NEVER,
    D3D12_COMPARISON_FUNC_LESS,
    D3D12_COMPARISON_FUNC_EQUAL,
    D3D12_COMPARISON_FUNC_LESS_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER,
    D3D12_COMPARISON_FUNC_NOT_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER_EQUAL,
    D3D12_COMPARISON_FUNC_ALWAYS,
};

D3D12_STENCIL_OP gDx12StencilOpTranslator[StencilOp::MAX_STENCIL_OPS] =
{
    D3D12_STENCIL_OP_KEEP,
    D3D12_STENCIL_OP_ZERO,
    D3D12_STENCIL_OP_REPLACE,
    D3D12_STENCIL_OP_INVERT,
    D3D12_STENCIL_OP_INCR,
    D3D12_STENCIL_OP_DECR,
    D3D12_STENCIL_OP_INCR_SAT,
    D3D12_STENCIL_OP_DECR_SAT,
};

D3D12_CULL_MODE gDx12CullModeTranslator[MAX_CULL_MODES] =
{
    D3D12_CULL_MODE_NONE,
    D3D12_CULL_MODE_BACK,
    D3D12_CULL_MODE_FRONT,
};

D3D12_FILL_MODE gDx12FillModeTranslator[MAX_FILL_MODES] =
{
    D3D12_FILL_MODE_SOLID,
    D3D12_FILL_MODE_WIREFRAME,
};

const D3D12_COMMAND_LIST_TYPE gDx12CmdTypeTranslator[MAX_QUEUE_TYPE] =
{
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    D3D12_COMMAND_LIST_TYPE_COPY,
    D3D12_COMMAND_LIST_TYPE_COMPUTE
};

const D3D12_COMMAND_QUEUE_PRIORITY gDx12QueuePriorityTranslator[QueuePriority::MAX_QUEUE_PRIORITY]
{
    D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
    D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
#if !defined(XBOX)
    D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME,
#endif
};
// clang-format on
//
// internal functions start with a capital letter / API starts with a small letter

// internal functions are capital first letter and capital letter of the next word

// Functions points for functions that need to be loaded
PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER fnD3D12CreateRootSignatureDeserializer = nullptr;
PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE fnD3D12SerializeVersionedRootSignature = nullptr;
PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER
fnD3D12CreateVersionedRootSignatureDeserializer = nullptr;


/************************************************************************/
// Descriptor Heap Defines
/************************************************************************/
typedef struct DescriptorHeapProperties
{
    uint32_t mMaxDescriptors;
    D3D12_DESCRIPTOR_HEAP_FLAGS mFlags;
} DescriptorHeapProperties;

typedef struct NullDescriptors
{
    // Default nullptr Descriptors for binding at empty descriptor slots to make sure all descriptors are bound at submit
    D3D12_CPU_DESCRIPTOR_HANDLE mNullTextureSRV[TEXTURE_DIM_COUNT];
    D3D12_CPU_DESCRIPTOR_HANDLE mNullTextureUAV[TEXTURE_DIM_COUNT];
    D3D12_CPU_DESCRIPTOR_HANDLE mNullBufferSRV;
    D3D12_CPU_DESCRIPTOR_HANDLE mNullBufferUAV;
    D3D12_CPU_DESCRIPTOR_HANDLE mNullBufferCBV;
    D3D12_CPU_DESCRIPTOR_HANDLE mNullSampler;
} NullDescriptors;

DescriptorHeapProperties gCpuDescriptorHeapProperties[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
{
    {1024 * 256, D3D12_DESCRIPTOR_HEAP_FLAG_NONE}, // CBV SRV UAV
    {2048, D3D12_DESCRIPTOR_HEAP_FLAG_NONE},       // Sampler
    {512, D3D12_DESCRIPTOR_HEAP_FLAG_NONE},        // RTV
    {512, D3D12_DESCRIPTOR_HEAP_FLAG_NONE},        // DSV
};


static void remove_default_resources(Renderer *pRenderer)
{
    SAFE_FREE(pRenderer->pNullDescriptors)
}

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


/************************************************************************/
// Descriptor Heap Structures
/************************************************************************/
/// CPU Visible Heap to store all the resources needing CPU read / write operations - Textures/Buffers/RTV
typedef struct DescriptorHeap
{
    typedef struct DescriptorHandle
    {
        D3D12_CPU_DESCRIPTOR_HANDLE mCpu;
        D3D12_GPU_DESCRIPTOR_HANDLE mGpu;
    } DescriptorHandle;

    /// DX Heap
    ID3D12DescriptorHeap *pCurrentHeap;
    /// Lock for multi-threaded descriptor allocations
    std::mutex *pMutex;
    ID3D12Device *pDevice;
    D3D12_CPU_DESCRIPTOR_HANDLE *pHandles;
    /// Start position in the heap
    DescriptorHandle mStartHandle;
    /// Free List used for CPU only descriptor heaps
    eastl::vector<DescriptorHandle> mFreeList;
    /// Description
    D3D12_DESCRIPTOR_HEAP_DESC mDesc;
    /// DescriptorInfo Increment Size
    uint32_t mDescriptorSize;
    /// Used
    tbb::atomic<uint32> mUsedDescriptors;
} DescriptorHeap;

/************************************************************************/
// Static Descriptor Heap Implementation
/************************************************************************/
static void add_descriptor_heap(ID3D12Device *pDevice,
                                const D3D12_DESCRIPTOR_HEAP_DESC *pDesc,
                                DescriptorHeap **ppDescHeap)
{
    uint32_t numDescriptors = pDesc->NumDescriptors;
    hook_modify_descriptor_heap_size(pDesc->Type, &numDescriptors);

    auto *pHeap = static_cast<DescriptorHeap *>(mi_calloc(1, sizeof(*pHeap)));

    pHeap->pMutex = static_cast<std::mutex *>(mi_calloc(1, sizeof(std::mutex)));
    // Descriptor Heap pDevice point to Renderer.pDxDevice
    pHeap->pDevice = pDevice;

    // Keep 32 aligned for easy remove
    numDescriptors = RoundUp(numDescriptors, 32);

    D3D12_DESCRIPTOR_HEAP_DESC Desc = *pDesc;
    Desc.NumDescriptors = numDescriptors;

    pHeap->mDesc = Desc;

    CHECK_DX12RESULT(pDevice->CreateDescriptorHeap(&Desc, IID_ARGS(&pHeap->pCurrentHeap)));

    pHeap->mStartHandle.mCpu = pHeap->pCurrentHeap->GetCPUDescriptorHandleForHeapStart();
    pHeap->mStartHandle.mGpu = pHeap->pCurrentHeap->GetGPUDescriptorHandleForHeapStart();
    pHeap->mDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(pHeap->mDesc.Type);
    if (Desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
        pHeap->pHandles = static_cast<D3D12_CPU_DESCRIPTOR_HANDLE *>(mi_calloc(
            Desc.NumDescriptors,
            sizeof(D3D12_CPU_DESCRIPTOR_HANDLE)));

    *ppDescHeap = pHeap;
}

static void remove_descriptor_heap(DescriptorHeap *pHeap)
{
    SAFE_RELEASE(pHeap->pCurrentHeap)

    // TODO make new and delete c++ friendly
    // Deconstruct should not be called by hand
    // pHeap->pMutex.~mutex();
    mi_free(pHeap->pMutex);

    pHeap->mFreeList.~vector();

    SAFE_FREE(pHeap->pHandles)

    // Here pHeap->Device is released?
    SAFE_FREE(pHeap)
}


/************************************************************************/
// Multi GPU Helper Functions
/************************************************************************/
uint32_t util_calculate_shared_node_mask(Renderer *pRenderer)
{
    if (pRenderer->mGpuMode == GPU_MODE_LINKED)
        return (1 << pRenderer->mLinkedNodeCount) - 1;
    else
        return 0;
}

/************************************************************************/
/************************************************************************/
constexpr D3D12_DEPTH_STENCIL_DESC util_to_depth_desc(const DepthStateDesc *pDesc)
{
    EA_ASSERT(pDesc->mDepthFunc < CompareMode::MAX_COMPARE_MODES);
    EA_ASSERT(pDesc->mStencilFrontFunc < CompareMode::MAX_COMPARE_MODES);
    EA_ASSERT(pDesc->mStencilFrontFail < StencilOp::MAX_STENCIL_OPS);
    EA_ASSERT(pDesc->mDepthFrontFail < StencilOp::MAX_STENCIL_OPS);
    EA_ASSERT(pDesc->mStencilFrontPass < StencilOp::MAX_STENCIL_OPS);
    EA_ASSERT(pDesc->mStencilBackFunc < CompareMode::MAX_COMPARE_MODES);
    EA_ASSERT(pDesc->mStencilBackFail < StencilOp::MAX_STENCIL_OPS);
    EA_ASSERT(pDesc->mDepthBackFail < StencilOp::MAX_STENCIL_OPS);
    EA_ASSERT(pDesc->mStencilBackPass < StencilOp::MAX_STENCIL_OPS);

    D3D12_DEPTH_STENCIL_DESC ret = {};
    ret.DepthEnable = static_cast<BOOL>(pDesc->mDepthTest);
    ret.DepthWriteMask = pDesc->mDepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    ret.DepthFunc = gDx12ComparisonFuncTranslator[pDesc->mDepthFunc];
    ret.StencilEnable = static_cast<BOOL>(pDesc->mStencilTest);
    ret.StencilReadMask = pDesc->mStencilReadMask;
    ret.StencilWriteMask = pDesc->mStencilWriteMask;
    ret.BackFace.StencilFunc = gDx12ComparisonFuncTranslator[pDesc->mStencilBackFunc];
    ret.FrontFace.StencilFunc = gDx12ComparisonFuncTranslator[pDesc->mStencilFrontFunc];
    ret.BackFace.StencilDepthFailOp = gDx12StencilOpTranslator[pDesc->mDepthBackFail];
    ret.FrontFace.StencilDepthFailOp = gDx12StencilOpTranslator[pDesc->mDepthFrontFail];
    ret.BackFace.StencilFailOp = gDx12StencilOpTranslator[pDesc->mStencilBackFail];
    ret.FrontFace.StencilFailOp = gDx12StencilOpTranslator[pDesc->mStencilFrontFail];
    ret.BackFace.StencilPassOp = gDx12StencilOpTranslator[pDesc->mStencilBackPass];
    ret.FrontFace.StencilPassOp = gDx12StencilOpTranslator[pDesc->mStencilFrontPass];

    return ret;
}


constexpr D3D12_BLEND_DESC util_to_blend_desc(const BlendStateDesc *pDesc)
{
    int blendDescIndex = 0;
#if defined(GRAPHICS_DEBUG)

    for (int i = 0; i < MAX_RENDER_TARGET_ATTACHMENTS; ++i)
    {
        if (pDesc->mRenderTargetMask & (1 << i))
        {
            EA_ASSERT(pDesc->mSrcFactors[blendDescIndex] < BlendConstant::MAX_BLEND_CONSTANTS);
            EA_ASSERT(pDesc->mDstFactors[blendDescIndex] < BlendConstant::MAX_BLEND_CONSTANTS);
            EA_ASSERT(pDesc->mSrcAlphaFactors[blendDescIndex] < BlendConstant::MAX_BLEND_CONSTANTS);
            EA_ASSERT(pDesc->mDstAlphaFactors[blendDescIndex] < BlendConstant::MAX_BLEND_CONSTANTS);
            EA_ASSERT(pDesc->mBlendModes[blendDescIndex] < BlendMode::MAX_BLEND_MODES);
            EA_ASSERT(pDesc->mBlendAlphaModes[blendDescIndex] < BlendMode::MAX_BLEND_MODES);
        }

        if (pDesc->mIndependentBlend)
            ++blendDescIndex;
    }

    blendDescIndex = 0;
#endif

    D3D12_BLEND_DESC ret = {};
    ret.AlphaToCoverageEnable = static_cast<BOOL>(pDesc->mAlphaToCoverage);
    ret.IndependentBlendEnable = TRUE;
    for (int i = 0; i < MAX_RENDER_TARGET_ATTACHMENTS; i++)
    {
        if (pDesc->mRenderTargetMask & (1 << i))
        {
            BOOL blendEnable =
            (gDx12BlendConstantTranslator[pDesc->mSrcFactors[blendDescIndex]] != D3D12_BLEND_ONE ||
             gDx12BlendConstantTranslator[pDesc->mDstFactors[blendDescIndex]] != D3D12_BLEND_ZERO ||
             gDx12BlendConstantTranslator[pDesc->mSrcAlphaFactors[blendDescIndex]] != D3D12_BLEND_ONE ||
             gDx12BlendConstantTranslator[pDesc->mDstAlphaFactors[blendDescIndex]] != D3D12_BLEND_ZERO);

            ret.RenderTarget[i].BlendEnable = blendEnable;
            ret.RenderTarget[i].RenderTargetWriteMask = (UINT8)pDesc->mMasks[blendDescIndex];
            ret.RenderTarget[i].BlendOp = gDx12BlendOpTranslator[pDesc->mBlendModes[blendDescIndex]];
            ret.RenderTarget[i].SrcBlend = gDx12BlendConstantTranslator[pDesc->mSrcFactors[blendDescIndex]];
            ret.RenderTarget[i].DestBlend = gDx12BlendConstantTranslator[pDesc->mDstFactors[blendDescIndex]];
            ret.RenderTarget[i].BlendOpAlpha = gDx12BlendOpTranslator[pDesc->mBlendAlphaModes[blendDescIndex]];
            ret.RenderTarget[i].SrcBlendAlpha = gDx12BlendConstantTranslator[pDesc->mSrcAlphaFactors[blendDescIndex]];
            ret.RenderTarget[i].DestBlendAlpha =
                gDx12BlendConstantTranslator[pDesc->mDstAlphaFactors[blendDescIndex]];
        }

        if (pDesc->mIndependentBlend)
            ++blendDescIndex;
    }

    return ret;
}


constexpr D3D12_RASTERIZER_DESC util_to_rasterizer_desc(const RasterizerStateDesc *pDesc)
{
    EA_ASSERT(pDesc->mFillMode < FillMode::MAX_FILL_MODES);
    EA_ASSERT(pDesc->mCullMode < CullMode::MAX_CULL_MODES);
    EA_ASSERT(pDesc->mFrontFace == FRONT_FACE_CCW || pDesc->mFrontFace == FRONT_FACE_CW);

    D3D12_RASTERIZER_DESC ret = {};
    ret.FillMode = gDx12FillModeTranslator[pDesc->mFillMode];
    ret.CullMode = gDx12CullModeTranslator[pDesc->mCullMode];
    ret.FrontCounterClockwise = pDesc->mFrontFace == FRONT_FACE_CCW;
    ret.DepthBias = pDesc->mDepthBias;
    ret.DepthBiasClamp = 0.0f;
    ret.SlopeScaledDepthBias = pDesc->mSlopeScaledDepthBias;
    ret.DepthClipEnable = !pDesc->mDepthClampEnable;
    ret.MultisampleEnable = pDesc->mMultiSample ? TRUE : FALSE;
    ret.AntialiasedLineEnable = FALSE;
    ret.ForcedSampleCount = 0;
    ret.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return ret;
}

/************************************************************************/
/************************************************************************/

static DescriptorHeap::DescriptorHandle consume_descriptor_handles(DescriptorHeap *pHeap, uint32_t descriptorCount)
{
    if (pHeap->mUsedDescriptors + descriptorCount > pHeap->mDesc.NumDescriptors)
    {
        std::scoped_lock<std::mutex> lock(*pHeap->pMutex);

        if ((pHeap->mDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
        {
            uint32_t currentOffset = pHeap->mUsedDescriptors;
            D3D12_DESCRIPTOR_HEAP_DESC desc = pHeap->mDesc;
            while (pHeap->mUsedDescriptors + descriptorCount > desc.NumDescriptors)
                desc.NumDescriptors <<= 1;
            ID3D12Device *pDevice = pHeap->pDevice;
            SAFE_RELEASE(pHeap->pCurrentHeap)
            pDevice->CreateDescriptorHeap(&desc, IID_ARGS(&pHeap->pCurrentHeap));
            pHeap->mDesc = desc;
            pHeap->mStartHandle.mCpu = pHeap->pCurrentHeap->GetCPUDescriptorHandleForHeapStart();
            pHeap->mStartHandle.mGpu = pHeap->pCurrentHeap->GetGPUDescriptorHandleForHeapStart();

            auto *const rangeSizes = static_cast<uint32_t *>(alloca(pHeap->mUsedDescriptors * sizeof(uint32_t)));
            uint32_t usedDescriptors = pHeap->mUsedDescriptors.load();
            for (uint32_t i = 0; i < pHeap->mUsedDescriptors; ++i)
                rangeSizes[i] = 1;
            pDevice->CopyDescriptors(1, &pHeap->mStartHandle.mCpu, &usedDescriptors,
                                     pHeap->mUsedDescriptors, pHeap->pHandles, rangeSizes,
                                     pHeap->mDesc.Type);
            auto *pNewHandles = static_cast<D3D12_CPU_DESCRIPTOR_HANDLE *>(mi_calloc(pHeap->mDesc.NumDescriptors, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE)));
            memcpy(pNewHandles, pHeap->pHandles, pHeap->mUsedDescriptors * sizeof(D3D12_CPU_DESCRIPTOR_HANDLE));
            SAFE_FREE(pHeap->pHandles)
            pHeap->pHandles = pNewHandles;
        }
        else if (pHeap->mFreeList.size() >= descriptorCount)
        {
            if (descriptorCount == 1)
            {
                DescriptorHeap::DescriptorHandle ret = pHeap->mFreeList.back();
                pHeap->mFreeList.pop_back();
                return ret;
            }

            // search for continuous free items in the list
            uint32_t freeCount = 1;
            for (size_t i = pHeap->mFreeList.size() - 1; i > 0; --i)
            {
                const size_t index = i - 1;
                const DescriptorHeap::DescriptorHandle descHandle = pHeap->mFreeList[index];
                if (descHandle.mCpu.ptr + pHeap->mDescriptorSize == pHeap->mFreeList[i].mCpu.ptr)
                    ++freeCount;
                else
                    freeCount = 1;

                if (freeCount == descriptorCount)
                {
                    pHeap->mFreeList.erase(pHeap->mFreeList.begin() + index, pHeap->mFreeList.begin() + index + descriptorCount);
                    return descHandle;
                }
            }
        }
    }

    uint32 usedDescriptors = pHeap->mUsedDescriptors.fetch_and_add<tbb::memory_semantics::relaxed>(descriptorCount);
    EA_ASSERT(usedDescriptors + descriptorCount <= pHeap->mDesc.NumDescriptors);
    DescriptorHeap::DescriptorHandle ret =
    {
        {pHeap->mStartHandle.mCpu.ptr + usedDescriptors * pHeap->mDescriptorSize},
        {pHeap->mStartHandle.mGpu.ptr + usedDescriptors * pHeap->mDescriptorSize},
    };

    return ret;
}

void return_cpu_descriptor_handles(DescriptorHeap *pHeap, D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32_t count)
{
    EA_ASSERT((pHeap->mDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) == 0);
    std::scoped_lock<std::mutex> lock(*pHeap->pMutex);
    for (uint32_t i = 0; i < count; ++i)
        pHeap->mFreeList.push_back({
            {handle.ptr + pHeap->mDescriptorSize * i},
            D3D12_GPU_VIRTUAL_ADDRESS_NULL});
}

static void copy_descriptor_handle(DescriptorHeap *pHeap, const D3D12_CPU_DESCRIPTOR_HANDLE &srcHandle, const uint64_t &dstHandle, uint32_t index)
{
    pHeap->pHandles[(dstHandle / pHeap->mDescriptorSize) + index] = srcHandle;
    pHeap->pDevice->CopyDescriptorsSimple(
        1,
        {pHeap->mStartHandle.mCpu.ptr + dstHandle + (index * pHeap->mDescriptorSize)},
        srcHandle,
        pHeap->mDesc.Type);
}

static void add_sampler(Renderer *pRenderer, const D3D12_SAMPLER_DESC *pSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE *pHandle)
{
    *pHandle =
        consume_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER], 1).mCpu;
    pRenderer->pDxDevice->CreateSampler(pSamplerDesc, *pHandle);
}

D3D12_DEPTH_STENCIL_DESC gDefaultDepthDesc = {};
D3D12_BLEND_DESC gDefaultBlendDesc = {};
D3D12_RASTERIZER_DESC gDefaultRasterizerDesc = {};

static void add_srv(
    Renderer *pRenderer, ID3D12Resource *pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC *pSrvDesc, D3D12_CPU_DESCRIPTOR_HANDLE *pHandle)
{
    if (D3D12_GPU_VIRTUAL_ADDRESS_NULL == pHandle->ptr)
        *pHandle = consume_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], 1).mCpu;
    pRenderer->pDxDevice->CreateShaderResourceView(pResource, pSrvDesc, *pHandle);
}

static void add_uav(
    Renderer *pRenderer, ID3D12Resource *pResource, ID3D12Resource *pCounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC *pUavDesc,
    D3D12_CPU_DESCRIPTOR_HANDLE *pHandle)
{
    if (D3D12_GPU_VIRTUAL_ADDRESS_NULL == pHandle->ptr)
        *pHandle = consume_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], 1).mCpu;
    pRenderer->pDxDevice->CreateUnorderedAccessView(pResource, pCounterResource, pUavDesc, *pHandle);
}

static void add_cbv(Renderer *pRenderer, const D3D12_CONSTANT_BUFFER_VIEW_DESC *pCbvDesc, D3D12_CPU_DESCRIPTOR_HANDLE *pHandle)
{
    if (D3D12_GPU_VIRTUAL_ADDRESS_NULL == pHandle->ptr)
        *pHandle = consume_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], 1).mCpu;
    pRenderer->pDxDevice->CreateConstantBufferView(pCbvDesc, *pHandle);
}


static void add_rtv(
	Renderer* pRenderer, ID3D12Resource* pResource, DXGI_FORMAT format, uint32_t mipSlice, uint32_t arraySlice,
	D3D12_CPU_DESCRIPTOR_HANDLE* pHandle)
{
	if (D3D12_GPU_VIRTUAL_ADDRESS_NULL == pHandle->ptr)
		*pHandle = consume_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV], 1).mCpu;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	D3D12_RESOURCE_DESC           desc = pResource->GetDesc();
	D3D12_RESOURCE_DIMENSION      type = desc.Dimension;

	rtvDesc.Format = format;

	switch (type)
	{
		case D3D12_RESOURCE_DIMENSION_BUFFER: break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if (desc.DepthOrArraySize > 1)
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
				rtvDesc.Texture1DArray.MipSlice = mipSlice;
				if (arraySlice != -1)
				{
					rtvDesc.Texture1DArray.ArraySize = 1;
					rtvDesc.Texture1DArray.FirstArraySlice = arraySlice;
				}
				else
				{
					rtvDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
				}
			}
			else
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
				rtvDesc.Texture1D.MipSlice = mipSlice;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (desc.SampleDesc.Count > 1)
			{
				if (desc.DepthOrArraySize > 1)
				{
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
					if (arraySlice != -1)
					{
						rtvDesc.Texture2DMSArray.ArraySize = 1;
						rtvDesc.Texture2DMSArray.FirstArraySlice = arraySlice;
					}
					else
					{
						rtvDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
					}
				}
				else
				{
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
				}
			}
			else
			{
				if (desc.DepthOrArraySize > 1)
				{
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
					rtvDesc.Texture2DArray.MipSlice = mipSlice;
					if (arraySlice != -1)
					{
						rtvDesc.Texture2DArray.ArraySize = 1;
						rtvDesc.Texture2DArray.FirstArraySlice = arraySlice;
					}
					else
					{
						rtvDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
					}
				}
				else
				{
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
					rtvDesc.Texture2D.MipSlice = mipSlice;
				}
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
			rtvDesc.Texture3D.MipSlice = mipSlice;
			if (arraySlice != -1)
			{
				rtvDesc.Texture3D.WSize = 1;
				rtvDesc.Texture3D.FirstWSlice = arraySlice;
			}
			else
			{
				rtvDesc.Texture3D.WSize = desc.DepthOrArraySize;
			}
			break;
		default: break;
	}

	pRenderer->pDxDevice->CreateRenderTargetView(pResource, &rtvDesc, *pHandle);
}

static void add_dsv(
	Renderer* pRenderer, ID3D12Resource* pResource, DXGI_FORMAT format, uint32_t mipSlice, uint32_t arraySlice,
	D3D12_CPU_DESCRIPTOR_HANDLE* pHandle)
{
	if (D3D12_GPU_VIRTUAL_ADDRESS_NULL == pHandle->ptr)
		*pHandle = consume_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV], 1).mCpu;
		
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	D3D12_RESOURCE_DESC           desc = pResource->GetDesc();
	D3D12_RESOURCE_DIMENSION      type = desc.Dimension;

	dsvDesc.Format = format;

	switch (type)
	{
		case D3D12_RESOURCE_DIMENSION_BUFFER: break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if (desc.DepthOrArraySize > 1)
			{
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
				dsvDesc.Texture1DArray.MipSlice = mipSlice;
				if (arraySlice != -1)
				{
					dsvDesc.Texture1DArray.ArraySize = 1;
					dsvDesc.Texture1DArray.FirstArraySlice = arraySlice;
				}
				else
				{
					dsvDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
				}
			}
			else
			{
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
				dsvDesc.Texture1D.MipSlice = mipSlice;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (desc.SampleDesc.Count > 1)
			{
				if (desc.DepthOrArraySize > 1)
				{
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
					if (arraySlice != -1)
					{
						dsvDesc.Texture2DMSArray.ArraySize = 1;
						dsvDesc.Texture2DMSArray.FirstArraySlice = arraySlice;
					}
					else
					{
						dsvDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
					}
				}
				else
				{
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
				}
			}
			else
			{
				if (desc.DepthOrArraySize > 1)
				{
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
					dsvDesc.Texture2DArray.MipSlice = mipSlice;
					if (arraySlice != -1)
					{
						dsvDesc.Texture2DArray.ArraySize = 1;
						dsvDesc.Texture2DArray.FirstArraySlice = arraySlice;
					}
					else
					{
						dsvDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
					}
				}
				else
				{
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
					dsvDesc.Texture2D.MipSlice = mipSlice;
				}
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D: EA_ASSERT(false && "Cannot create 3D Depth Stencil"); break;
		default: break;
	}

	pRenderer->pDxDevice->CreateDepthStencilView(pResource, &dsvDesc, *pHandle);
}

static void add_default_resources(Renderer *pRenderer)
{
    pRenderer->pNullDescriptors = static_cast<NullDescriptors *>(mi_calloc(1, sizeof(NullDescriptors)));
    // Create nullptr descriptors in case user does not specify some descriptors we can bind null descriptor handles at those points
    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    add_sampler(pRenderer, &samplerDesc, &pRenderer->pNullDescriptors->mNullSampler);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8_UINT;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_R8_UINT;

    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_1D]);
    add_uav(pRenderer, nullptr, nullptr, &uavDesc, &pRenderer->pNullDescriptors->mNullTextureUAV[TEXTURE_DIM_1D]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_2D]);
    add_uav(pRenderer, nullptr, nullptr, &uavDesc, &pRenderer->pNullDescriptors->mNullTextureUAV[TEXTURE_DIM_2D]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_2DMS]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_3D]);
    add_uav(pRenderer, nullptr, nullptr, &uavDesc, &pRenderer->pNullDescriptors->mNullTextureUAV[TEXTURE_DIM_3D]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_1D_ARRAY]);
    add_uav(pRenderer, nullptr, nullptr, &uavDesc, &pRenderer->pNullDescriptors->mNullTextureUAV[TEXTURE_DIM_1D_ARRAY]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_2D_ARRAY]);
    add_uav(pRenderer, nullptr, nullptr, &uavDesc, &pRenderer->pNullDescriptors->mNullTextureUAV[TEXTURE_DIM_2D_ARRAY]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_2DMS_ARRAY]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_CUBE]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullTextureSRV[TEXTURE_DIM_CUBE_ARRAY]);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    add_srv(pRenderer, nullptr, &srvDesc, &pRenderer->pNullDescriptors->mNullBufferSRV);
    add_uav(pRenderer, nullptr, nullptr, &uavDesc, &pRenderer->pNullDescriptors->mNullBufferUAV);
    add_cbv(pRenderer, nullptr, &pRenderer->pNullDescriptors->mNullBufferCBV);

    BlendStateDesc blendStateDesc = {};
    blendStateDesc.mDstAlphaFactors[0] = BC_ZERO;
    blendStateDesc.mDstFactors[0] = BC_ZERO;
    blendStateDesc.mSrcAlphaFactors[0] = BC_ONE;
    blendStateDesc.mSrcFactors[0] = BC_ONE;
    blendStateDesc.mMasks[0] = ALL;
    blendStateDesc.mRenderTargetMask = BLEND_STATE_TARGET_ALL;
    blendStateDesc.mIndependentBlend = false;
    gDefaultBlendDesc = util_to_blend_desc(&blendStateDesc);

    DepthStateDesc depthStateDesc = {};
    depthStateDesc.mDepthFunc = CMP_LEQUAL;
    depthStateDesc.mDepthTest = false;
    depthStateDesc.mDepthWrite = false;
    depthStateDesc.mStencilBackFunc = CMP_ALWAYS;
    depthStateDesc.mStencilFrontFunc = CMP_ALWAYS;
    depthStateDesc.mStencilReadMask = 0xFF;
    depthStateDesc.mStencilWriteMask = 0xFF;
    gDefaultDepthDesc = util_to_depth_desc(&depthStateDesc);

    RasterizerStateDesc rasterizerStateDesc = {};
    rasterizerStateDesc.mCullMode = CULL_MODE_BACK;
    gDefaultRasterizerDesc = util_to_rasterizer_desc(&rasterizerStateDesc);
}

uint32_t util_calculate_node_mask(Renderer *pRenderer, uint32_t i)
{
    if (pRenderer->mGpuMode == GPU_MODE_LINKED)
        return (1 << i);
    else
        return 0;
}


DXGI_FORMAT util_to_dx_uav_format(DXGI_FORMAT defaultFormat)
{
	switch (defaultFormat)
	{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM;

		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM;

		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8X8_UNORM;

		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;

#if defined(GRAPHICS_DEBUG)
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_D16_UNORM: Logger::error("Requested a UAV format for a depth stencil format");
#endif

		default: return defaultFormat;
	}
}

DXGI_FORMAT util_to_dx_dsv_format(DXGI_FORMAT defaultFormat)
{
	switch (defaultFormat)
	{
			// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

			// No Stencil
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;

			// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;

			// 16-bit Z w/o Stencil
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM: return DXGI_FORMAT_D16_UNORM;

		default: return defaultFormat;
	}
}

DXGI_FORMAT util_to_dx_srv_format(DXGI_FORMAT defaultFormat)
{
	switch (defaultFormat)
	{
			// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

			// No Stencil
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

			// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

			// 16-bit Z w/o Stencil
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM: return DXGI_FORMAT_R16_UNORM;

		case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;

		default: return defaultFormat;
	}
}

DXGI_FORMAT util_to_dx_stencil_format(DXGI_FORMAT defaultFormat)
{
	switch (defaultFormat)
	{
			// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

			// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

		default: return DXGI_FORMAT_UNKNOWN;
	}
}

DXGI_FORMAT util_to_dx_swapchain_format(TextureFormat const format)
{
	DXGI_FORMAT result = DXGI_FORMAT_UNKNOWN;

	// FLIP_DISCARD and FLIP_SEQEUNTIAL swapchain buffers only support these formats
	switch (format)
	{
		case R16G16B16A16_SFLOAT: result = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
		case B8G8R8A8_UNORM: result = DXGI_FORMAT_B8G8R8A8_UNORM; break;
		case R8G8B8A8_UNORM: result = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		case B8G8R8A8_SRGB: result = DXGI_FORMAT_B8G8R8A8_UNORM; break;
		case R8G8B8A8_SRGB: result = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		case R10G10B10A2_UNORM: result = DXGI_FORMAT_R10G10B10A2_UNORM; break;
		default: break;
	}

	if (result == DXGI_FORMAT_UNKNOWN)
	{
		Logger::error("Image Format (%u) not supported for creating swapchain buffer", (uint32_t)format);
	}

	return result;
}


D3D12_SHADER_VISIBILITY util_to_dx_shader_visibility(ShaderStage stages)
{
	D3D12_SHADER_VISIBILITY res = D3D12_SHADER_VISIBILITY_ALL;
	uint32_t                stageCount = 0;

	if (stages == SHADER_STAGE_COMP)
	{
		return D3D12_SHADER_VISIBILITY_ALL;
	}
	if (stages & SHADER_STAGE_VERT)
	{
		res = D3D12_SHADER_VISIBILITY_VERTEX;
		++stageCount;
	}
	if (stages & SHADER_STAGE_GEOM)
	{
		res = D3D12_SHADER_VISIBILITY_GEOMETRY;
		++stageCount;
	}
	if (stages & SHADER_STAGE_HULL)
	{
		res = D3D12_SHADER_VISIBILITY_HULL;
		++stageCount;
	}
	if (stages & SHADER_STAGE_DOMN)
	{
		res = D3D12_SHADER_VISIBILITY_DOMAIN;
		++stageCount;
	}
	if (stages & SHADER_STAGE_FRAG)
	{
		res = D3D12_SHADER_VISIBILITY_PIXEL;
		++stageCount;
	}
#ifdef ENABLE_RAYTRACING
	if (stages == SHADER_STAGE_RAYTRACING)
	{
		return D3D12_SHADER_VISIBILITY_ALL;
	}
#endif
	EA_ASSERT(stageCount > 0);
	return stageCount > 1 ? D3D12_SHADER_VISIBILITY_ALL : res;
}

D3D12_RESOURCE_STATES util_to_dx_resource_state(ResourceState state)
{
	D3D12_RESOURCE_STATES ret = D3D12_RESOURCE_STATE_COMMON;

	// These states cannot be combined with other states so we just do an == check
	if (state == RESOURCE_STATE_GENERIC_READ)
		return D3D12_RESOURCE_STATE_GENERIC_READ;
	if (state == RESOURCE_STATE_COMMON)
		return D3D12_RESOURCE_STATE_COMMON;
	if (state == RESOURCE_STATE_PRESENT)
		return D3D12_RESOURCE_STATE_PRESENT;

	if (state & RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
		ret |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	if (state & RESOURCE_STATE_INDEX_BUFFER)
		ret |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
	if (state & RESOURCE_STATE_RENDER_TARGET)
		ret |= D3D12_RESOURCE_STATE_RENDER_TARGET;
	if (state & RESOURCE_STATE_UNORDERED_ACCESS)
		ret |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	if (state & RESOURCE_STATE_DEPTH_WRITE)
		ret |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
	if (state & RESOURCE_STATE_DEPTH_READ)
		ret |= D3D12_RESOURCE_STATE_DEPTH_READ;
	if (state & RESOURCE_STATE_STREAM_OUT)
		ret |= D3D12_RESOURCE_STATE_STREAM_OUT;
	if (state & RESOURCE_STATE_INDIRECT_ARGUMENT)
		ret |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
	if (state & RESOURCE_STATE_COPY_DEST)
		ret |= D3D12_RESOURCE_STATE_COPY_DEST;
	if (state & RESOURCE_STATE_COPY_SOURCE)
		ret |= D3D12_RESOURCE_STATE_COPY_SOURCE;
	if (state & RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		ret |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	if (state & RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		ret |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
#ifdef ENABLE_RAYTRACING
	if (state & RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
		ret |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
#endif

	return ret;
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
    CHECK_HRESULT(hook_create_device(nullptr, feature_levels[0], &pRenderer->pDxDevice));

#if defined(GRAPHICS_DEBUG)
    //Sets the callback functions to invoke when the GPU hangs
    //pRenderer->pDxDevice->SetHangCallbacksX(HANGBEGINCALLBACK, HANGPRINTCALLBACK, nullptr);
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

    uint32_t gpuCount = 0;
    IDXGIAdapter4 *adapter = nullptr;
    bool foundSoftwareAdapter = false;

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
                            nullptr))
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
    for (uint i = 0; DXGI_ERROR_NOT_FOUND != pRenderer->pDXGIFactory->EnumAdapterByGpuPreference(i,
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

    auto isDeviceBetter = [](GpuDesc *gpuDesc,
                             const uint32_t testIndex,
                             const uint32_t refIndex)
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
    auto *gpuSettings = static_cast<GPUSettings *>(alloca(gpuCount * sizeof(GPUSettings)));

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
        SAFE_RELEASE(gpuDesc[i].pGpu)
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


static void RemoveDevice(Renderer *pRenderer)
{
    SAFE_RELEASE(pRenderer->pDXGIFactory);
    SAFE_RELEASE(pRenderer->pDxActiveGPU);
#if EA_PLATFORM_DESKTOP
    if (pRenderer->pDxDebugValidation)
    {
        pRenderer->pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
        pRenderer->pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
        pRenderer->pDxDebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
        SAFE_RELEASE(pRenderer->pDxDebugValidation);
    }
#endif

#if defined(XBOX)
    SAFE_RELEASE(pRenderer->pDxDevice);
#elif GRAPHICS_DEBUG
    ID3D12DebugDevice *pDebugDevice = nullptr;
    pRenderer->pDxDevice->QueryInterface(&pDebugDevice);

    SAFE_RELEASE(pRenderer->pDXDebug)
    SAFE_RELEASE(pRenderer->pDxDevice)

    if (pDebugDevice)
    {
        // Debug device is released first so report live objects don't show its ref as a warning.
        pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
        pDebugDevice->Release();
    }
#else
    SAFE_RELEASE(pRenderer->pDxDevice);
#endif

#if defined(USE_NSIGHT_AFTERMATH)
    DestroyAftermathTracker(&pRenderer->mAftermathTracker);
#endif

#if defined(USE_DRED)
    SAFE_RELEASE(pRenderer->pDredSettings);
#endif
}

inline void utils_caps_builder(Renderer *pRenderer)
{
    pRenderer->pCapBits = static_cast<GPUCapBits *>(mi_calloc(1, sizeof(GPUCapBits)));

    for (uint32_t i = 0; i < TextureFormat::Count; ++i)
    {
        const DXGI_FORMAT fmt = static_cast<DXGI_FORMAT>(TextureFormatToDxgiFormat(
            static_cast<TextureFormat>(i)));
        if (fmt == DXGI_FORMAT_UNKNOWN)
            continue;

        D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {fmt};

        pRenderer->pDxDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT,
                                                  &formatSupport,
                                                  sizeof(formatSupport));
        pRenderer->pCapBits->canShaderReadFrom[i] =
            (formatSupport.Support1 & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE) != 0;
        pRenderer->pCapBits->canShaderWriteTo[i] =
            (formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE) != 0;
        pRenderer->pCapBits->canRenderTargetWriteTo[i] =
            (formatSupport.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET) != 0;
    }
}

/************************************************************************/
// Renderer Init Remove
/************************************************************************/
void initRenderer(const char *appName, const RendererDesc *pDesc, Renderer **ppRenderer)
{
    EA_ASSERT(appName);
    EA_ASSERT(pDesc);
    EA_ASSERT(ppRenderer);

    Renderer *pRenderer = static_cast<Renderer *>(mi_calloc_aligned(1, alignof(Renderer), sizeof(Renderer)));
    EA_ASSERT(pRenderer);

    pRenderer->mGpuMode = pDesc->mGpuMode;
    pRenderer->mShaderTarget = pDesc->mShaderTarget;
    pRenderer->mEnableGpuBasedValidation = pDesc->mEnableGPUBasedValidation;
#if defined(XBOX)
    pRenderer->mApi = RENDERER_API_XBOX_D3D12;
#else
    pRenderer->mApi = RENDERER_API_D3D12;
#endif

    pRenderer->mName = appName;

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
            *ppRenderer = nullptr;
            return;
        }

#if !defined(XBOX)
        //anything below LOW preset is not supported and we will exit
        if (pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel < GPU_PRESET_LOW)
        {
            //have the condition in the EA_ASSERT as well so its cleared when the EA_ASSERT message box appears

            EA_ASSERT(
                pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel >= GPU_PRESET_LOW);

            //remove device and any memory we allocated in just above as this is the first function called
            //when initializing the forge
            RemoveDevice(pRenderer);
            SAFE_FREE(pRenderer)
            Logger::error("Selected GPU has an Office Preset in gpu.cfg.");
            Logger::error("Office preset is not supported by The Forge.");

            //return nullptr pRenderer so that client can gracefully handle exit
            //This is better than exiting from here in case client has allocated memory or has fallbacks
            *ppRenderer = nullptr;
            return;
        }

        utils_caps_builder(pRenderer);

        if (pRenderer->mShaderTarget >= shader_target_6_0)
        {
            // Query the level of support of Shader Model.
            D3D12_FEATURE_DATA_SHADER_MODEL shaderModelSupport = {D3D_SHADER_MODEL_6_0};
            D3D12_FEATURE_DATA_D3D12_OPTIONS1 waveIntrinsicsSupport = {};
            if (!SUCCEEDED(pRenderer->pDxDevice->CheckFeatureSupport(
                static_cast<D3D12_FEATURE>(D3D12_FEATURE_SHADER_MODEL), &shaderModelSupport, sizeof(
                    shaderModelSupport))))
            {
                return;
            }
            // Query the level of support of Wave Intrinsics.
            if (!SUCCEEDED(pRenderer->pDxDevice->CheckFeatureSupport(
                static_cast<D3D12_FEATURE>(D3D12_FEATURE_D3D12_OPTIONS1), &waveIntrinsicsSupport,
                sizeof(
                    waveIntrinsicsSupport))))
            {
                return;
            }

            // // If the device doesn't support SM6 or Wave Intrinsics, try enabling the experimental feature for Shader Model 6 and creating the device again.
            // if (shaderModelSupport.HighestShaderModel != D3D_SHADER_MODEL_6_0 ||
            //     waveIntrinsicsSupport.WaveOps != TRUE)
            // {
            //     RENDERDOC_API_1_1_2 *rdoc_api = nullptr;
            //     // At init, on windows
            //     if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
            //     {
            //         pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(
            //             mod,
            //             "RENDERDOC_GetAPI");
            //         RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
            //     }
            //
            //     // If RenderDoc is connected shader model 6 is not detected but it still works
            //     if (!rdoc_api || !rdoc_api->IsTargetControlConnected())
            //     {
            //         // If the device still doesn't support SM6 or Wave Intrinsics after enabling the experimental feature, you could set up your application to use the highest supported shader model.
            //         // For simplicity we just exit the application here.
            //         if (shaderModelSupport.HighestShaderModel < D3D_SHADER_MODEL_6_0 ||
            //             (waveIntrinsicsSupport.WaveOps != TRUE && !SUCCEEDED(
            //                  EnableExperimentalShaderModels())))
            //         {
            //             RemoveDevice(pRenderer);
            //             LOGF(LogLevel::eERROR, "Hardware does not support Shader Model 6.0");
            //             return;
            //         }
            //     }
            //     else
            //     {
            //         LOGF(LogLevel::eWARNING,
            //              "\nRenderDoc does not support SM 6.0 or higher. Application might work but you won't be able to debug the SM 6.0+ "
            //              "shaders or view their bytecode.");
            //     }
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
    pRenderer->pCPUDescriptorHeaps = static_cast<DescriptorHeap **>(mi_malloc(
        sizeof(DescriptorHeap *) * D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES));
    pRenderer->pCbvSrvUavHeaps = static_cast<DescriptorHeap **>(mi_malloc(
        sizeof(DescriptorHeap *) * pRenderer->mLinkedNodeCount));
    pRenderer->pSamplerHeaps = static_cast<DescriptorHeap **>(mi_malloc(
        sizeof(DescriptorHeap *) * pRenderer->mLinkedNodeCount));

    for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Flags = gCpuDescriptorHeapProperties[i].mFlags;
        desc.NodeMask = 0; // CPU Descriptor Heap - Node mask is irrelevant
        desc.NumDescriptors = gCpuDescriptorHeapProperties[i].mMaxDescriptors;
        desc.Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i);
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

    D3D12MA::ALLOCATION_CALLBACKS allocationCallbacks = {};
    allocationCallbacks.pAllocate = [](size_t size, size_t alignment, void *)
    {
        return mi_memalign(alignment, size);
    };
    allocationCallbacks.pFree = [](void *ptr, void *)
    {
        mi_free(ptr);
    };
    desc.pAllocationCallbacks = &allocationCallbacks;
    CHECK_DX12RESULT(D3D12MA::CreateAllocator(&desc, &pRenderer->pResourceAllocator))
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

    pRenderer->mBuiltinShaderDefinesCount = sizeof(rendererShaderDefines) / sizeof(rendererShaderDefines[0]);
    pRenderer->pBuiltinShaderDefines = static_cast<ShaderMacro *>(mi_calloc(pRenderer->mBuiltinShaderDefinesCount, sizeof(ShaderMacro)));
    for (uint32_t i = 0; i < pRenderer->mBuiltinShaderDefinesCount; ++i)
    {
        pRenderer->pBuiltinShaderDefines[i] = rendererShaderDefines[i];
    }

    // Renderer is good!
    *ppRenderer = pRenderer;
}


void removeRenderer(Renderer *pRenderer)
{
    EA_ASSERT(pRenderer);

    for (uint32_t i = 0; i < pRenderer->mBuiltinShaderDefinesCount; ++i)
        pRenderer->pBuiltinShaderDefines[i].~ShaderMacro();
    SAFE_FREE(pRenderer->pBuiltinShaderDefines);

    remove_default_resources(pRenderer);

    // Destroy the Direct3D12 bits
    for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        remove_descriptor_heap(pRenderer->pCPUDescriptorHeaps[i]);
    }

    for (uint32_t i = 0; i < pRenderer->mLinkedNodeCount; ++i)
    {
        remove_descriptor_heap(pRenderer->pCbvSrvUavHeaps[i]);
        remove_descriptor_heap(pRenderer->pSamplerHeaps[i]);
    }

    SAFE_RELEASE(pRenderer->pResourceAllocator);

    RemoveDevice(pRenderer);

    hook_post_remove_renderer(pRenderer);

    nvapiExit();
    agsExit();

    // Free all the renderer components
    SAFE_FREE(pRenderer->pCPUDescriptorHeaps)
    SAFE_FREE(pRenderer->pCbvSrvUavHeaps)
    SAFE_FREE(pRenderer->pSamplerHeaps)
    SAFE_FREE(pRenderer->pCapBits)
    SAFE_FREE(pRenderer->pActiveGpuSettings)
    SAFE_FREE(pRenderer)
}


/************************************************************************/
// Resource Creation Functions
/************************************************************************/
void addFence(Renderer *pRenderer, Fence **ppFence)
{
    //EA_ASSERT that renderer is valid
    EA_ASSERT(pRenderer);
    EA_ASSERT(ppFence);

    //create a Fence and EA_ASSERT that it is valid
    Fence *pFence = static_cast<Fence *>(mi_calloc(1, sizeof(Fence)));
    EA_ASSERT(pFence);

    CHECK_DX12RESULT(pRenderer->pDxDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_ARGS(&pFence->pDxFence)));
    pFence->mFenceValue = 1;

    pFence->pDxWaitIdleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, NULL);

    *ppFence = pFence;
}

void removeFence(Renderer *pRenderer, Fence *pFence)
{
    //EA_ASSERT that renderer is valid
    EA_ASSERT(pRenderer);
    //EA_ASSERT that given fence to remove is valid
    EA_ASSERT(pFence);

    SAFE_RELEASE(pFence->pDxFence)
    CloseHandle(pFence->pDxWaitIdleFenceEvent);

    SAFE_FREE(pFence);
}


void addQueue(Renderer *pRenderer, QueueDesc *pDesc, Queue **ppQueue)
{
    EA_ASSERT(pRenderer);
    EA_ASSERT(pDesc);
    EA_ASSERT(ppQueue);

    auto pQueue = static_cast<Queue *>(mi_calloc(1, sizeof(Queue)));
    EA_ASSERT(pQueue);

    if (pDesc->mNodeIndex)
    {
        EA_ASSERT(pRenderer->mGpuMode == GPU_MODE_LINKED && "Node Masking can only be used with Linked Multi GPU");
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    if (pDesc->mFlag & QUEUE_FLAG_DISABLE_GPU_TIMEOUT)
        queueDesc.Flags |= D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
    queueDesc.Type = gDx12CmdTypeTranslator[pDesc->mType];
    queueDesc.Priority = gDx12QueuePriorityTranslator[pDesc->mPriority];
    queueDesc.NodeMask = util_calculate_node_mask(pRenderer, pDesc->mNodeIndex);

    CHECK_DX12RESULT(hook_create_command_queue(pRenderer->pDxDevice, &queueDesc, &pQueue->pDxQueue));

    wchar_t queueTypeBuffer[32] = {};
    wchar_t *queueType = nullptr;
    switch (queueDesc.Type)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        queueType = L"GRAPHICS QUEUE";
        break;
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        queueType = L"COMPUTE QUEUE";
        break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        queueType = L"COPY QUEUE";
        break;
    default:
        break;
    }

    swprintf_s(queueTypeBuffer, L"%ls %u", queueType, pDesc->mNodeIndex);
    pQueue->pDxQueue->SetName(queueTypeBuffer);

    pQueue->mType = pDesc->mType;
    pQueue->mNodeIndex = pDesc->mNodeIndex;

    // Add queue fence. This fence will make sure we finish all GPU works before releasing the queue
    addFence(pRenderer, &pQueue->pFence);

    *ppQueue = pQueue;
}

void removeQueue(Renderer *pRenderer, Queue *pQueue)
{
    EA_ASSERT(pQueue);

    // Make sure we finished all GPU works before we remove the queue
    waitQueueIdle(pQueue);

    removeFence(pRenderer, pQueue->pFence);

    SAFE_RELEASE(pQueue->pDxQueue);

    SAFE_FREE(pQueue);
}

void addSwapChain(Renderer *pRenderer, const SwapChainDesc *pDesc, SwapChain **ppSwapChain)
{
    EA_ASSERT(pRenderer);
    EA_ASSERT(pDesc);
    EA_ASSERT(ppSwapChain);
    EA_ASSERT(pDesc->mImageCount <= MAX_SWAPCHAIN_IMAGES);

    auto *pSwapChain = static_cast<SwapChain *>(mi_calloc(1, sizeof(SwapChain) + pDesc->mImageCount * sizeof(RenderTarget *)));
    EA_ASSERT(pSwapChain);
    pSwapChain->ppRenderTargets = reinterpret_cast<RenderTarget **>(pSwapChain + 1);
    EA_ASSERT(pSwapChain->ppRenderTargets);

#if !defined(XBOX)
    pSwapChain->mDxSyncInterval = pDesc->mEnableVsync ? 1 : 0;

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = pDesc->mWidth;
    desc.Height = pDesc->mHeight;
    desc.Format = util_to_dx_swapchain_format(pDesc->mColorFormat);
    desc.Stereo = false;
    desc.SampleDesc.Count = 1; // If multisampling is needed, we'll resolve it later
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = pDesc->mImageCount;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc.Flags = 0;

    BOOL allowTearing = FALSE;
    pRenderer->pDXGIFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
    desc.Flags |= allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    pSwapChain->mFlags |= (!pDesc->mEnableVsync && allowTearing) ? DXGI_PRESENT_ALLOW_TEARING : 0;

    IDXGISwapChain1 *swapchain;

    HWND hwnd = static_cast<HWND>(pDesc->mWindowHandle.window);

    CHECK_DX12RESULT(pRenderer->pDXGIFactory->CreateSwapChainForHwnd(pDesc->ppPresentQueues[0]->pDxQueue, hwnd, &desc, NULL, NULL, &swapchain));

    CHECK_DX12RESULT(pRenderer->pDXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

    CHECK_DX12RESULT(swapchain->QueryInterface(IID_ARGS(&pSwapChain->pDxSwapChain)));
    swapchain->Release();

    // Allowing multiple command queues to present for applications like Alternate Frame Rendering
    if (pRenderer->mGpuMode == GPU_MODE_LINKED && pDesc->mPresentQueueCount > 1)
    {
        auto **ppQueues = static_cast<IUnknown **>(alloca(pDesc->mPresentQueueCount * sizeof(IUnknown *)));
        UINT *pCreationMasks = static_cast<UINT *>(alloca(pDesc->mPresentQueueCount * sizeof(UINT)));
        for (uint32_t i = 0; i < pDesc->mPresentQueueCount; ++i)
        {
            ppQueues[i] = pDesc->ppPresentQueues[i]->pDxQueue;
            pCreationMasks[i] = (1 << pDesc->ppPresentQueues[i]->mNodeIndex);
        }

        pSwapChain->pDxSwapChain->ResizeBuffers1(
            desc.BufferCount, desc.Width, desc.Height, desc.Format, desc.Flags, pCreationMasks, ppQueues);
    }

    auto **buffers = static_cast<ID3D12Resource **>(alloca(pDesc->mImageCount * sizeof(ID3D12Resource *)));

    // Create rendertargets from swapchain
    for (uint32_t i = 0; i < pDesc->mImageCount; ++i)
    {
        CHECK_DX12RESULT(pSwapChain->pDxSwapChain->GetBuffer(i, IID_ARGS(&buffers[i])));
    }

#endif

    RenderTargetDesc descColor = {};
    descColor.mWidth = pDesc->mWidth;
    descColor.mHeight = pDesc->mHeight;
    descColor.mDepth = 1;
    descColor.mArraySize = 1;
    descColor.mFormat = pDesc->mColorFormat;
    descColor.mClearValue = pDesc->mColorClearValue;
    descColor.mSampleCount = SAMPLE_COUNT_1;
    descColor.mSampleQuality = 0;
    descColor.pNativeHandle = nullptr;
    descColor.mFlags = TEXTURE_CREATION_FLAG_ALLOW_DISPLAY_TARGET;
#if defined(XBOX)
	descColor.mFlags |= TEXTURE_CREATION_FLAG_OWN_MEMORY_BIT;
	pSwapChain->pPresentQueue = pDesc->mPresentQueueCount ? pDesc->ppPresentQueues[0] : NULL;
#endif
    for (uint32_t i = 0; i < pDesc->mImageCount; ++i)
    {
#if !defined(XBOX)
        descColor.pNativeHandle = static_cast<void *>(buffers[i]);
#endif
        ::addRenderTarget(pRenderer, &descColor, &pSwapChain->ppRenderTargets[i]);
    }

    pSwapChain->mImageCount = pDesc->mImageCount;
    pSwapChain->mEnableVsync = pDesc->mEnableVsync;

    *ppSwapChain = pSwapChain;
}

void addCmdPool(Renderer *pRenderer, const CmdPoolDesc *pDesc, CmdPool **ppCmdPool)
{
    //EA_ASSERT that renderer is valid
    EA_ASSERT(pRenderer);
    EA_ASSERT(pDesc);
    EA_ASSERT(ppCmdPool);

    //create one new CmdPool and add to renderer
    CmdPool *pCmdPool = static_cast<CmdPool *>(mi_calloc(1, sizeof(CmdPool)));
    EA_ASSERT(pCmdPool);

    CHECK_DX12RESULT(pRenderer->pDxDevice->CreateCommandAllocator(
        gDx12CmdTypeTranslator[pDesc->pQueue->mType], IID_ARGS(&pCmdPool->pDxCmdAlloc)));

    pCmdPool->pQueue = pDesc->pQueue;

    *ppCmdPool = pCmdPool;
}

void removeCmdPool(Renderer *pRenderer, CmdPool *pCmdPool)
{
    //check validity of given renderer and command pool
    EA_ASSERT(pRenderer);
    EA_ASSERT(pCmdPool);

    SAFE_RELEASE(pCmdPool->pDxCmdAlloc)
    SAFE_FREE(pCmdPool)
}


void addCmd(Renderer* pRenderer, const CmdDesc* pDesc, Cmd** ppCmd)
{
	//verify that given pool is valid
    EA_ASSERT(pRenderer);
	EA_ASSERT(pDesc);
	EA_ASSERT(ppCmd);

	// initialize to zero
    auto* pCmd = static_cast<Cmd *>(mi_calloc_aligned(1, sizeof(Cmd), alignof(Cmd)));
	EA_ASSERT(pCmd);

	//set command pool of new command
	pCmd->mNodeIndex = pDesc->pPool->pQueue->mNodeIndex;
	pCmd->mType = pDesc->pPool->pQueue->mType;
	pCmd->pQueue = pDesc->pPool->pQueue;
	pCmd->pRenderer = pRenderer;

	pCmd->pBoundHeaps[0] = pRenderer->pCbvSrvUavHeaps[pCmd->mNodeIndex];
	pCmd->pBoundHeaps[1] = pRenderer->pSamplerHeaps[pCmd->mNodeIndex];

	pCmd->pCmdPool = pDesc->pPool;

    const uint32_t nodeMask = util_calculate_node_mask(pRenderer, pCmd->mNodeIndex);

	if (QUEUE_TYPE_TRANSFER == pDesc->pPool->pQueue->mType)
	{
		CHECK_DX12RESULT(hook_create_copy_cmd(pRenderer->pDxDevice, nodeMask, pDesc->pPool->pDxCmdAlloc, pCmd))
	}
	else
	{
		ID3D12PipelineState* initialState = nullptr;
		CHECK_DX12RESULT(pRenderer->pDxDevice->CreateCommandList(
			nodeMask, gDx12CmdTypeTranslator[pCmd->mType], pDesc->pPool->pDxCmdAlloc,
			initialState, __uuidof(pCmd->pDxCmdList), reinterpret_cast<void **>(&(pCmd->pDxCmdList))))
	}

	// Command lists are addd in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	CHECK_DX12RESULT(pCmd->pDxCmdList->Close())

	*ppCmd = pCmd;
}

void removeCmd(Renderer* pRenderer, Cmd* pCmd)
{
	//verify that given command and pool are valid
	EA_ASSERT(pRenderer);
	EA_ASSERT(pCmd);

	if (QUEUE_TYPE_TRANSFER == pCmd->mType)
	{
		hook_remove_copy_cmd(pCmd);
	}
	else
	{
		SAFE_RELEASE(pCmd->pDxCmdList)
	}

	SAFE_FREE(pCmd)
}


void addCmd_n(Renderer* pRenderer, const CmdDesc* pDesc, uint32_t cmdCount, Cmd*** pppCmd)
{
	//verify that ***cmd is valid
	EA_ASSERT(pRenderer);
	EA_ASSERT(pDesc);
	EA_ASSERT(cmdCount);
	EA_ASSERT(pppCmd);

	Cmd** ppCmds = static_cast<Cmd **>(mi_calloc(cmdCount, sizeof(Cmd *)));
	EA_ASSERT(ppCmds);

	//add n new cmds to given pool
	for (uint32_t i = 0; i < cmdCount; ++i)
	{
		::addCmd(pRenderer, pDesc, &ppCmds[i]);
	}

	*pppCmd = ppCmds;
}

void removeCmd_n(Renderer* pRenderer, uint32_t cmdCount, Cmd** ppCmds)
{
	//verify that given command list is valid
	EA_ASSERT(ppCmds);

	//remove every given cmd in array
	for (uint32_t i = 0; i < cmdCount; ++i)
	{
		removeCmd(pRenderer, ppCmds[i]);
	}

	SAFE_FREE(ppCmds)
}


void addTexture(Renderer* pRenderer, const TextureDesc* pDesc, Texture** ppTexture)
{
	EA_ASSERT(pRenderer);
	EA_ASSERT(pDesc && pDesc->mWidth && pDesc->mHeight && (pDesc->mDepth || pDesc->mArraySize));
	if (pDesc->mSampleCount > SAMPLE_COUNT_1 && pDesc->mMipLevels > 1)
	{
		Logger::error("Multi-Sampled textures cannot have mip maps");
		EA_ASSERT(false);
		return;
	}

	//allocate new texture
	auto* pTexture = static_cast<Texture *>(mi_calloc_aligned(1, sizeof(Texture), alignof(Texture)));
	EA_ASSERT(pTexture);

	if (pDesc->pNativeHandle)
	{
		pTexture->mOwnsImage = false;
		pTexture->pDxResource = (ID3D12Resource*)pDesc->pNativeHandle;
	}
	else
	{
		pTexture->mOwnsImage = true;
	}


	//add to gpu
	D3D12_RESOURCE_DESC desc = {};

	DXGI_FORMAT dxFormat = static_cast<DXGI_FORMAT>(TextureFormatToDxgiFormat(pDesc->mFormat));

	DescriptorType      descriptors = pDesc->mDescriptors;

#if defined(GRAPHICS_DEBUG)
	wchar_t debugName[MAX_DEBUG_NAME_LENGTH] = {};
	if (pDesc->pName)
	{
		mbstowcs(debugName, pDesc->pName, MAX_DEBUG_NAME_LENGTH);
	}
#endif

	EA_ASSERT(DXGI_FORMAT_UNKNOWN != dxFormat);

	if (nullptr == pTexture->pDxResource)
	{
		D3D12_RESOURCE_DIMENSION res_dim = D3D12_RESOURCE_DIMENSION_UNKNOWN;
		if (pDesc->mFlags & TEXTURE_CREATION_FLAG_FORCE_2D)
		{
			EA_ASSERT(pDesc->mDepth == 1);
			res_dim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		}
		else if (pDesc->mFlags & TEXTURE_CREATION_FLAG_FORCE_3D)
		{
			res_dim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		}
		else
		{
			if (pDesc->mDepth > 1)
				res_dim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			else if (pDesc->mHeight > 1)
				res_dim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			else
				res_dim = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		}

		desc.Dimension = res_dim;
		//On PC, If Alignment is set to 0, the runtime will use 4MB for MSAA textures and 64KB for everything else.
		//On XBox, We have to explicitlly assign D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT if MSAA is used
		desc.Alignment = static_cast<UINT>(pDesc->mSampleCount) > 1 ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : 0;
		desc.Width = pDesc->mWidth;
		desc.Height = pDesc->mHeight;
		desc.DepthOrArraySize = static_cast<UINT16>(pDesc->mArraySize != 1 ? pDesc->mArraySize : pDesc->mDepth);
		desc.MipLevels = static_cast<UINT16>(pDesc->mMipLevels);
		desc.Format = DxgiFormatToTypeless(dxFormat);
		desc.SampleDesc.Count = static_cast<UINT>(pDesc->mSampleCount);
		desc.SampleDesc.Quality = static_cast<UINT>(pDesc->mSampleQuality);
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS data;
		data.Format = desc.Format;
		data.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		data.SampleCount = desc.SampleDesc.Count;
		pRenderer->pDxDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &data, sizeof(data));
		while (data.NumQualityLevels == 0 && data.SampleCount > 0)
		{
			Logger::warn("Sample Count {} not supported. Trying a lower sample count {}", data.SampleCount, data.SampleCount / 2);
			data.SampleCount = desc.SampleDesc.Count / 2;
			pRenderer->pDxDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &data, sizeof(data));
		}
		desc.SampleDesc.Count = data.SampleCount;

		ResourceState actualStartState = pDesc->mStartState;

		// Decide UAV flags
		if (descriptors & DESCRIPTOR_TYPE_RW_TEXTURE)
		{
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		// Decide render target flags
		if (pDesc->mStartState & RESOURCE_STATE_RENDER_TARGET)
		{
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			actualStartState = (pDesc->mStartState > RESOURCE_STATE_RENDER_TARGET) ?
				(pDesc->mStartState & (ResourceState)~RESOURCE_STATE_RENDER_TARGET) :
				RESOURCE_STATE_RENDER_TARGET;

		}
		else if (pDesc->mStartState & RESOURCE_STATE_DEPTH_WRITE)
		{
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			actualStartState = (pDesc->mStartState > RESOURCE_STATE_DEPTH_WRITE) ?
				(pDesc->mStartState & (ResourceState)~RESOURCE_STATE_DEPTH_WRITE) :
				RESOURCE_STATE_DEPTH_WRITE;
		}

		// Decide sharing flags
		if (pDesc->mFlags & TEXTURE_CREATION_FLAG_EXPORT_ADAPTER_BIT)
		{
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		}

		hook_modify_texture_resource_flags(pDesc->mFlags, &desc.Flags);

		DECLARE_ZERO(D3D12_CLEAR_VALUE, clearValue);
		clearValue.Format = dxFormat;
		if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
		{
			clearValue.DepthStencil.Depth = pDesc->mClearValue.depth;
			clearValue.DepthStencil.Stencil = (UINT8)pDesc->mClearValue.stencil;
		}
		else
		{
			clearValue.Color[0] = pDesc->mClearValue.r;
			clearValue.Color[1] = pDesc->mClearValue.g;
			clearValue.Color[2] = pDesc->mClearValue.b;
			clearValue.Color[3] = pDesc->mClearValue.a;
		}

		D3D12_CLEAR_VALUE*    pClearValue = nullptr;
		D3D12_RESOURCE_STATES res_states = util_to_dx_resource_state(actualStartState);

		if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) || (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
		{
			pClearValue = &clearValue;
		}

		D3D12MA::ALLOCATION_DESC alloc_desc = {};
		alloc_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		if (pDesc->mFlags & TEXTURE_CREATION_FLAG_OWN_MEMORY_BIT)
			alloc_desc.Flags |= D3D12MA::ALLOCATION_FLAG_COMMITTED;

#if defined(XBOX)
		if (pDesc->mFlags & TEXTURE_CREATION_FLAG_ALLOW_DISPLAY_TARGET)
		{
			alloc_desc.ExtraHeapFlags |= D3D12_HEAP_FLAG_ALLOW_DISPLAY;
			desc.Format = dxFormat;
		}
#endif

		// Multi GPU
		alloc_desc.CreationNodeMask = (1 << pDesc->mNodeIndex);
		alloc_desc.VisibleNodeMask = alloc_desc.CreationNodeMask;
		for (uint32_t i = 0; i < pDesc->mSharedNodeIndexCount; ++i)
			alloc_desc.VisibleNodeMask |= (1 << pDesc->pSharedNodeIndices[i]);

		// // Create resource
		// if (SUCCEEDED(hook_create_special_resource(pRenderer, &desc, pClearValue, res_states, pDesc->mFlags, &pTexture->pDxResource)))
		// {
		// 	Logger::info("Allocated memory in special platform-specific RAM");
		// }
		// else
		// {
			CHECK_DX12RESULT(pRenderer->pResourceAllocator->CreateResource(&alloc_desc, &desc, res_states, pClearValue,
				&pTexture->pDxAllocation, IID_ARGS(&pTexture->pDxResource)))
#if defined(GRAPHICS_DEBUG)
			// Set name
			//pTexture->pDxAllocation->SetName(debugName);
#endif
		// }
	}
	else
	{
		desc = pTexture->pDxResource->GetDesc();
		dxFormat = desc.Format;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC  srvDesc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

	switch (desc.Dimension)
	{
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		{
			if (desc.DepthOrArraySize > 1)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
				// SRV
				srvDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
				srvDesc.Texture1DArray.FirstArraySlice = 0;
				srvDesc.Texture1DArray.MipLevels = desc.MipLevels;
				srvDesc.Texture1DArray.MostDetailedMip = 0;
				// UAV
				uavDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
				uavDesc.Texture1DArray.FirstArraySlice = 0;
				uavDesc.Texture1DArray.MipSlice = 0;
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
				// SRV
				srvDesc.Texture1D.MipLevels = desc.MipLevels;
				srvDesc.Texture1D.MostDetailedMip = 0;
				// UAV
				uavDesc.Texture1D.MipSlice = 0;
			}
			break;
		}
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		{
			if (DESCRIPTOR_TYPE_TEXTURE_CUBE == (descriptors & DESCRIPTOR_TYPE_TEXTURE_CUBE))
			{
				EA_ASSERT(desc.DepthOrArraySize % 6 == 0);

				if (desc.DepthOrArraySize > 6)
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					// SRV
					srvDesc.TextureCubeArray.First2DArrayFace = 0;
					srvDesc.TextureCubeArray.MipLevels = desc.MipLevels;
					srvDesc.TextureCubeArray.MostDetailedMip = 0;
					srvDesc.TextureCubeArray.NumCubes = desc.DepthOrArraySize / 6;
				}
				else
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					// SRV
					srvDesc.TextureCube.MipLevels = desc.MipLevels;
					srvDesc.TextureCube.MostDetailedMip = 0;
				}

				// UAV
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uavDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
				uavDesc.Texture2DArray.FirstArraySlice = 0;
				uavDesc.Texture2DArray.MipSlice = 0;
				uavDesc.Texture2DArray.PlaneSlice = 0;
			}
			else
			{
				if (desc.DepthOrArraySize > 1)
				{
					if (desc.SampleDesc.Count > SAMPLE_COUNT_1)
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
						// Cannot create a multisampled uav
						// SRV
						srvDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
						srvDesc.Texture2DMSArray.FirstArraySlice = 0;
						// No UAV
					}
					else
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
						uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
						// SRV
						srvDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
						srvDesc.Texture2DArray.FirstArraySlice = 0;
						srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
						srvDesc.Texture2DArray.MostDetailedMip = 0;
						srvDesc.Texture2DArray.PlaneSlice = 0;
						// UAV
						uavDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
						uavDesc.Texture2DArray.FirstArraySlice = 0;
						uavDesc.Texture2DArray.MipSlice = 0;
						uavDesc.Texture2DArray.PlaneSlice = 0;
					}
				}
				else
				{
					if (desc.SampleDesc.Count > SAMPLE_COUNT_1)
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
						// Cannot create a multisampled uav
					}
					else
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
						uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
						// SRV
						srvDesc.Texture2D.MipLevels = desc.MipLevels;
						srvDesc.Texture2D.MostDetailedMip = 0;
						srvDesc.Texture2D.PlaneSlice = 0;
						// UAV
						uavDesc.Texture2D.MipSlice = 0;
						uavDesc.Texture2D.PlaneSlice = 0;
					}
				}
			}
			break;
		}
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			// SRV
			srvDesc.Texture3D.MipLevels = desc.MipLevels;
			srvDesc.Texture3D.MostDetailedMip = 0;
			// UAV
			uavDesc.Texture3D.MipSlice = 0;
			uavDesc.Texture3D.FirstWSlice = 0;
			uavDesc.Texture3D.WSize = desc.DepthOrArraySize;
			break;
		}
		default: break;
	}

	DescriptorHeap* pHeap = pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
	uint32_t handleCount = (descriptors & DESCRIPTOR_TYPE_TEXTURE) ? 1 : 0;
	handleCount += (descriptors & DESCRIPTOR_TYPE_RW_TEXTURE) ? pDesc->mMipLevels : 0;
	pTexture->mDxDescriptorHandles = consume_descriptor_handles(pHeap, handleCount).mCpu;

	if (descriptors & DESCRIPTOR_TYPE_TEXTURE)
	{
		EA_ASSERT(srvDesc.ViewDimension != D3D12_SRV_DIMENSION_UNKNOWN);

		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = util_to_dx_srv_format(dxFormat);
		add_srv(pRenderer, pTexture->pDxResource, &srvDesc, &pTexture->mDxDescriptorHandles);
		++pTexture->mUavStartIndex;
	}

	if (descriptors & DESCRIPTOR_TYPE_RW_TEXTURE)
	{
		pTexture->mDescriptorSize = pHeap->mDescriptorSize;
		uavDesc.Format = util_to_dx_uav_format(dxFormat);
		for (uint32_t i = 0; i < pDesc->mMipLevels; ++i)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = { pTexture->mDxDescriptorHandles.ptr + (i + pTexture->mUavStartIndex) * pTexture->mDescriptorSize };

			uavDesc.Texture1DArray.MipSlice = i;
			if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
				uavDesc.Texture3D.WSize = desc.DepthOrArraySize / (UINT)pow(2, i);
			add_uav(pRenderer, pTexture->pDxResource, NULL, &uavDesc, &handle);
		}
	}

#if defined(ENABLE_GRAPHICS_DEBUG)
	if (pDesc->pName)
	{
		// Set name
		pTexture->pDxResource->SetName(debugName);
	}
#endif

	pTexture->mNodeIndex = pDesc->mNodeIndex;
	pTexture->mHandleCount = handleCount;
	pTexture->mMipLevels = pDesc->mMipLevels;
	pTexture->mWidth = pDesc->mWidth;
	pTexture->mHeight = pDesc->mHeight;
	pTexture->mDepth = pDesc->mDepth;
	pTexture->mUav = pDesc->mDescriptors & DESCRIPTOR_TYPE_RW_TEXTURE;
	pTexture->mFormat = pDesc->mFormat;
	pTexture->mArraySizeMinusOne = pDesc->mArraySize - 1;

	*ppTexture = pTexture;
}


void removeTexture(Renderer* pRenderer, Texture* pTexture)
{
	EA_ASSERT(pRenderer);
	EA_ASSERT(pTexture);

	//delete texture descriptors
	if (pTexture->mDxDescriptorHandles.ptr != D3D12_GPU_VIRTUAL_ADDRESS_NULL)
		return_cpu_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV],
			pTexture->mDxDescriptorHandles, pTexture->mHandleCount);

	if (pTexture->mOwnsImage)
	{
		SAFE_RELEASE(pTexture->pDxAllocation)
		SAFE_RELEASE(pTexture->pDxResource)
	}

	if (pTexture->pSvt)
	{
		void removeVirtualTexture(Renderer* pRenderer, VirtualTexture* pSvt);
		removeVirtualTexture(pRenderer, pTexture->pSvt);
	}

	SAFE_FREE(pTexture)
}


void addRenderTarget(Renderer* pRenderer, const RenderTargetDesc* pDesc, RenderTarget** ppRenderTarget)
{
	EA_ASSERT(pRenderer);
	EA_ASSERT(pDesc);
	EA_ASSERT(ppRenderTarget);

	const bool isDepth = TextureFormatHasDepth(pDesc->mFormat);
	EA_ASSERT(!((isDepth) && (pDesc->mDescriptors & DESCRIPTOR_TYPE_RW_TEXTURE)) && "Cannot use depth stencil as UAV");

	pDesc->mMipLevels = eastl::max(1U, pDesc->mMipLevels);

	auto* pRenderTarget = static_cast<RenderTarget *>(mi_calloc_aligned(1, sizeof(RenderTarget), alignof(RenderTarget)));
	EA_ASSERT(pRenderTarget);

	//add to gpu
    auto dxFormat = static_cast<DXGI_FORMAT>(TextureFormatToDxgiFormat(pDesc->mFormat));
	EA_ASSERT(DXGI_FORMAT_UNKNOWN != dxFormat);

	TextureDesc textureDesc = {};
	textureDesc.mArraySize = pDesc->mArraySize;
	textureDesc.mClearValue = pDesc->mClearValue;
	textureDesc.mDepth = pDesc->mDepth;
	textureDesc.mFlags = pDesc->mFlags;
	textureDesc.mFormat = pDesc->mFormat;
	textureDesc.mHeight = pDesc->mHeight;
	textureDesc.mMipLevels = pDesc->mMipLevels;
	textureDesc.mSampleCount = pDesc->mSampleCount;
	textureDesc.mSampleQuality = pDesc->mSampleQuality;
	textureDesc.mStartState = pDesc->mStartState;

	if (!isDepth)
		textureDesc.mStartState |= RESOURCE_STATE_RENDER_TARGET;
	else
		textureDesc.mStartState |= RESOURCE_STATE_DEPTH_WRITE;

	textureDesc.mWidth = pDesc->mWidth;
	textureDesc.pNativeHandle = pDesc->pNativeHandle;
	textureDesc.pName = pDesc->pName;
	textureDesc.mNodeIndex = pDesc->mNodeIndex;
	textureDesc.pSharedNodeIndices = pDesc->pSharedNodeIndices;
	textureDesc.mSharedNodeIndexCount = pDesc->mSharedNodeIndexCount;
	textureDesc.mDescriptors = pDesc->mDescriptors;
	if (!(pDesc->mFlags & TEXTURE_CREATION_FLAG_ALLOW_DISPLAY_TARGET))
	{
		// Create SRV by default for a render target
		textureDesc.mDescriptors |= DESCRIPTOR_TYPE_TEXTURE;
	}

	addTexture(pRenderer, &textureDesc, &pRenderTarget->pTexture);

    const D3D12_RESOURCE_DESC desc = pRenderTarget->pTexture->pDxResource->GetDesc();

	uint32_t handleCount = desc.MipLevels;
	if ((pDesc->mDescriptors & DESCRIPTOR_TYPE_RENDER_TARGET_ARRAY_SLICES) ||
		(pDesc->mDescriptors & DESCRIPTOR_TYPE_RENDER_TARGET_DEPTH_SLICES))
		handleCount *= desc.DepthOrArraySize;
	handleCount += 1;

	DescriptorHeap* pHeap = isDepth ?
		pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] :
		pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
	pRenderTarget->mDxDescriptors = consume_descriptor_handles(pHeap, handleCount).mCpu;
	pRenderTarget->mDxDescriptorSize = pHeap->mDescriptorSize;

	if(isDepth)
		add_dsv(pRenderer, pRenderTarget->pTexture->pDxResource, dxFormat, 0, -1, &pRenderTarget->mDxDescriptors);
	else
		add_rtv(pRenderer, pRenderTarget->pTexture->pDxResource, dxFormat, 0, -1, &pRenderTarget->mDxDescriptors);

	for (uint32_t i = 0; i < desc.MipLevels; ++i)
	{
		if ((pDesc->mDescriptors & DESCRIPTOR_TYPE_RENDER_TARGET_ARRAY_SLICES) ||
			(pDesc->mDescriptors & DESCRIPTOR_TYPE_RENDER_TARGET_DEPTH_SLICES))
		{
			for (uint32_t j = 0; j < desc.DepthOrArraySize; ++j)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = {
					pRenderTarget->mDxDescriptors.ptr +
					(1 + i * desc.DepthOrArraySize + j) * pRenderTarget->mDxDescriptorSize };

				if(isDepth)
					add_dsv(pRenderer, pRenderTarget->pTexture->pDxResource, dxFormat, i, j, &handle);
				else
					add_rtv(pRenderer, pRenderTarget->pTexture->pDxResource, dxFormat, i, j, &handle);
			}
		}
		else
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = { pRenderTarget->mDxDescriptors.ptr + (1 + i) * pRenderTarget->mDxDescriptorSize };

			if(isDepth)
				add_dsv(pRenderer, pRenderTarget->pTexture->pDxResource, dxFormat, i, -1, &handle);
			else
				add_rtv(pRenderer, pRenderTarget->pTexture->pDxResource, dxFormat, i, -1, &handle);
		}
	}

	pRenderTarget->mWidth = pDesc->mWidth;
	pRenderTarget->mHeight = pDesc->mHeight;
	pRenderTarget->mArraySize = pDesc->mArraySize;
	pRenderTarget->mDepth = pDesc->mDepth;
	pRenderTarget->mMipLevels = pDesc->mMipLevels;
	pRenderTarget->mSampleCount = pDesc->mSampleCount;
	pRenderTarget->mSampleQuality = pDesc->mSampleQuality;
	pRenderTarget->mFormat = pDesc->mFormat;
	pRenderTarget->mClearValue = pDesc->mClearValue;

	*ppRenderTarget = pRenderTarget;
}


void removeRenderTarget(Renderer* pRenderer, RenderTarget* pRenderTarget)
{
	bool const isDepth = TextureFormatHasDepth(pRenderTarget->mFormat);

	removeTexture(pRenderer, pRenderTarget->pTexture);

	const uint32_t depthOrArraySize = static_cast<uint32_t>(pRenderTarget->mArraySize * pRenderTarget->mDepth);
	uint32_t handleCount = pRenderTarget->mMipLevels;
	if ((pRenderTarget->mDescriptors & DESCRIPTOR_TYPE_RENDER_TARGET_ARRAY_SLICES) ||
		(pRenderTarget->mDescriptors & DESCRIPTOR_TYPE_RENDER_TARGET_DEPTH_SLICES))
		handleCount *= depthOrArraySize;
	handleCount += 1;

	!isDepth ?
		return_cpu_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV], pRenderTarget->mDxDescriptors, handleCount) :
		return_cpu_descriptor_handles(pRenderer->pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV], pRenderTarget->mDxDescriptors, handleCount);

	SAFE_FREE(pRenderTarget);
}