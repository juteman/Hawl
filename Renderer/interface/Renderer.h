/*
 *  Copyright 2020 juteman
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
#include "RenderInfo.h"
#include "TextureFormat.h"
//#include "tbb/spin_mutex.h"
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Hawl
{
#ifdef __cplusplus
#ifndef MAKE_ENUM_FLAG
#define MAKE_ENUM_FLAG(TYPE, ENUM_TYPE)                                                            \
    static inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b)                                    \
    {                                                                                              \
        return (ENUM_TYPE)((TYPE)(a) | (TYPE)(b));                                                 \
    }                                                                                              \
    static inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b)                                    \
    {                                                                                              \
        return (ENUM_TYPE)((TYPE)(a) & (TYPE)(b));                                                 \
    }                                                                                              \
    static inline ENUM_TYPE operator|=(ENUM_TYPE &a, ENUM_TYPE b)                                  \
    {                                                                                              \
        return a = (a | b);                                                                        \
    }                                                                                              \
    static inline ENUM_TYPE operator&=(ENUM_TYPE &a, ENUM_TYPE b)                                  \
    {                                                                                              \
        return a = (a & b);                                                                        \
    }

#endif
#endif

#ifndef RENDERER_CUSTOM_MAX
enum
{
    MAX_INSTANCE_EXTENSIONS = 64,
    MAX_DEVICE_EXTENSIONS = 64,
    /// Max number of GPUs in SLI or Cross-Fire
    MAX_LINKED_GPUS = 4,
    MAX_RENDER_TARGET_ATTACHMENTS = 8,
    MAX_VERTEX_BINDINGS = 15,
    MAX_VERTEX_ATTRIBS = 15,
    MAX_SEMANTIC_NAME_LENGTH = 128,
    MAX_DEBUG_NAME_LENGTH = 128,
    MAX_MIP_LEVELS = 0xFFFFFFFF,
    MAX_SWAPCHAIN_IMAGES = 3,
    MAX_ROOT_CONSTANTS_PER_ROOTSIGNATURE = 4,
    MAX_GPU_VENDOR_STRING_LENGTH = 64, // max size for GPUVendorPreset strings
};
#endif

struct DescriptorHeap;
struct NullDescriptors;

typedef enum GPUPresetLevel
{
    GPU_PRESET_NONE = 0,
    GPU_PRESET_OFFICE, // This means unsupported
    GPU_PRESET_LOW,
    GPU_PRESET_MEDIUM,
    GPU_PRESET_HIGH,
    GPU_PRESET_ULTRA,
    GPU_PRESET_COUNT
} GPUPresetLevel;

typedef struct Fence
{
    ID3D12Fence *pDxFence;
    HANDLE       pDxWaitIdleFenceEvent;
    uint64       mFenceValue;
    uint64       mPadA;
} Fence;

typedef enum QueueType
{
    QUEUE_TYPE_GRAPHICS = 0,
    QUEUE_TYPE_TRANSFER,
    QUEUE_TYPE_COMPUTE,
    MAX_QUEUE_TYPE
} QueueType;

typedef enum QueueFlag
{
    QUEUE_FLAG_NONE = 0x0,
    QUEUE_FLAG_DISABLE_GPU_TIMEOUT = 0x1,
    QUEUE_FLAG_INIT_MICROPROFILE = 0x2,
    MAX_QUEUE_FLAG = 0xFFFFFFFF
} QueueFlag;
MAKE_ENUM_FLAG(uint32, QueueFlag)

typedef enum QueuePriority
{
    QUEUE_PRIORITY_NORMAL,
    QUEUE_PRIORITY_HIGH,
    QUEUE_PRIORITY_GLOBAL_REALTIME,
    MAX_QUEUE_PRIORITY
} QueuePriority;

typedef struct Queue
{
    ID3D12CommandQueue *pDxQueue;
    uint32              mType : 3;
    uint32              mNodeIndex : 4;
    Fence *             pFence;
} Queue;

typedef struct QueueDesc
{
    QueueType     mType;
    QueueFlag     mFlag;
    QueuePriority mPriority;
    uint32_t      mNodeIndex;
} QueueDesc;

typedef struct GPUVendorPreset
{
    char mVendorId[MAX_GPU_VENDOR_STRING_LENGTH];
    char mModelId[MAX_GPU_VENDOR_STRING_LENGTH];
    char mRevisionId[MAX_GPU_VENDOR_STRING_LENGTH]; // OPtional as not all gpu's have that. Default
                                                    // is : 0x00
    GPUPresetLevel mPresetLevel;

    char mGpuName[MAX_GPU_VENDOR_STRING_LENGTH]; // If GPU Name is missing then value will be empty
                                                 // string
    char mGpuDriverVersion[MAX_GPU_VENDOR_STRING_LENGTH];
    char mGpuDriverDate[MAX_GPU_VENDOR_STRING_LENGTH];
} GPUVendorPreset;

typedef enum WaveOpsSupportFlags
{
    WAVE_OPS_SUPPORT_FLAG_NONE = 0x0,
    WAVE_OPS_SUPPORT_FLAG_BASIC_BIT = 0x00000001,
    WAVE_OPS_SUPPORT_FLAG_VOTE_BIT = 0x00000002,
    WAVE_OPS_SUPPORT_FLAG_ARITHMETIC_BIT = 0x00000004,
    WAVE_OPS_SUPPORT_FLAG_BALLOT_BIT = 0x00000008,
    WAVE_OPS_SUPPORT_FLAG_SHUFFLE_BIT = 0x00000010,
    WAVE_OPS_SUPPORT_FLAG_SHUFFLE_RELATIVE_BIT = 0x00000020,
    WAVE_OPS_SUPPORT_FLAG_CLUSTERED_BIT = 0x00000040,
    WAVE_OPS_SUPPORT_FLAG_QUAD_BIT = 0x00000080,
    WAVE_OPS_SUPPORT_FLAG_PARTITIONED_BIT_NV = 0x00000100,
    WAVE_OPS_SUPPORT_FLAG_ALL = 0x7FFFFFFF
} WaveOpsSupportFlags;
MAKE_ENUM_FLAG(uint32, WaveOpsSupportFlags);

/**
 * GPU Settings struct
 */
typedef struct GPUSettings
{
    uint32              mUniformBufferAlignment;
    uint32              mUploadBufferTextureAlignment;
    uint32              mUploadBufferTextureRowAlignment;
    uint32              mMaxVertexInputBindings;
    uint32              mMaxRootSignatureDWORDS;
    uint32              mWaveLaneCount;
    WaveOpsSupportFlags mWaveOpsSupportFlags;
    GPUVendorPreset     mGpuVendorPreset;
    uint32              mMultiDrawIndirect : 1;
    uint32              mROVsSupported : 1;
    uint32              mTessellationSupported : 1;
    uint32              mGeometryShaderSupported : 1;
} GPUSettings;

typedef struct ShaderMacro
{
    const char *definition;
    const char *value;
} ShaderMacro;

typedef struct GPUCapBits
{
    bool canShaderReadFrom[TextureFormat::Count];
    bool canShaderWriteTo[TextureFormat::Count];
    bool canRenderTargetWriteTo[TextureFormat::Count];
} GPUCapBits;


typedef enum ShaderTarget
{
    shader_target_5_1,
    shader_target_6_0,
    shader_target_6_1,
    shader_target_6_2,
    shader_target_6_3, //required for Raytracing
} ShaderTarget;

typedef enum GpuMode
{
    GPU_MODE_SINGLE = 0,
    GPU_MODE_LINKED,
} GpuMode;

typedef struct RendererDesc
{
    ShaderTarget                 mShaderTarget;
    GpuMode                      mGpuMode;
    /// This results in new validation not possible during API calls on the CPU, by creating patched shaders that have validation added directly to the shader.
    /// However, it can slow things down a lot, especially for applications with numerous PSOs. Time to see the first render frame may take several minutes
    bool                         mEnableGPUBasedValidation;
} RendererDesc;

typedef struct alignas(64) Renderer
{
#if D3D12_SUPPORTED
    DescriptorHeap **pCPUDescriptorHeaps;
    DescriptorHeap **pCbvSrvUavHeaps;
    DescriptorHeap **pSamplerHeaps;

    IDXGIFactory6 *pDXGIFactory;
    IDXGIAdapter4 *pDxActiveGPU;
    ID3D12Device * pDxDevice;
    // class  D3D12MA::Allocator*      pResourceAllocator;
#endif
    NullDescriptors *pNullDescriptors;
    char *           pName;
    GPUSettings *    pActiveGpuSettings;
    ShaderMacro *    pBuiltinShaderDefines;
    GPUCapBits *     pCapBits;
    uint32           mLinkedNodeCount : 4;
    uint32           mGpuMode : 3;
    uint32           mShaderTarget : 4;
    uint32           mApi : 5;
    uint32           mEnableGpuBasedValidation : 1;
    uint32           mBuiltinShaderDefinesCount;
} Renderer;

} // namespace Hawl
