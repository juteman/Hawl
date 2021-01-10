/*
 * Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 * Copyright (c) 2018-2021 The Forge Interactive Inc.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *  and base of the IRenderer.h
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

#pragma once
#include "OS.h"
//
// default capability levels of the renderer
//
#if !defined(RENDERER_CUSTOM_MAX)
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
    MAX_GPU_VENDOR_STRING_LENGTH = 64,
    //max size for GPUVendorPreset strings
#if defined(VULKAN)
	MAX_PLANE_COUNT = 3,
#endif
};
#endif

#if defined(XBOX)
#include "../../Xbox/Common_3/Renderer/Direct3D12/Direct3D12X.h"
#elif D3D12_SUPPORTED
#include <d3d12.h>
// #include "../ThirdParty/OpenSource/DirectXShaderCompiler/inc/dxcapi.h"
#include <dxgi1_6.h>
#include <dxgidebug.h>

// Nsight Aftermath
#if defined(_WIN32) && defined(_DEBUG) && defined(NSIGHT_AFTERMATH)
#include "../ThirdParty/PrivateNvidia/NsightAftermath/include/AftermathTracker.h"
#endif
#endif


#if D3D12_SUPPORTED
// Raytracing
#ifdef D3D12_RAYTRACING_AABB_BYTE_ALIGNMENT
#define ENABLE_RAYTRACING
#endif

// Forward declare memory allocator classes
namespace D3D12MA
{
class Allocator;
class Allocation;
};
#endif


#if defined(VULKAN)
#if defined(_WINDOWS) || defined(XBOX)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__ANDROID__)
#ifndef VK_USE_PLATFORM_ANDROID_KHR
#define VK_USE_PLATFORM_ANDROID_KHR
#endif
#elif defined(__linux__) && !defined(VK_USE_PLATFORM_GGP)
#define VK_USE_PLATFORM_XLIB_KHR    //Use Xlib or Xcb as display server, defaults to Xlib
#endif


// Set this define to enable renderdoc layer
// NOTE: Setting this define will disable use of the khr dedicated allocation extension since it conflicts with the renderdoc capture layer
//#define USE_RENDER_DOC

// Raytracing
#ifdef VK_NV_RAY_TRACING_SPEC_VERSION
#define ENABLE_RAYTRACING

// Nsight Aftermath
#if (defined(_WIN32) || defined(__linux__)) && defined(_DEBUG) && defined(NSIGHT_AFTERMATH)
#include "../ThirdParty/PrivateNvidia/NsightAftermath/include/AftermathTracker.h"
#endif
#endif

#define CHECK_VKRESULT(exp)                                                    \
{                                                                              \
	VkResult vkres = (exp);                                                    \
	if (VK_SUCCESS != vkres)                                                   \
	{                                                                          \
		LOGF(eERROR, "%s: FAILED with VkResult: %u", #exp, (uint32_t)vkres);   \
		ASSERT(false);                                                         \
	}                                                                          \
}

#endif


#include "BaseType.h"
#include "TextureFormat.h"

#ifdef __cplusplus
#ifndef MAKE_ENUM_FLAG
#define MAKE_ENUM_FLAG(TYPE, ENUM_TYPE)                                                                        \
	static inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) { return (ENUM_TYPE)((TYPE)(a) | (TYPE)(b)); } \
	static inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) { return (ENUM_TYPE)((TYPE)(a) & (TYPE)(b)); } \
	static inline ENUM_TYPE operator|=(ENUM_TYPE& a, ENUM_TYPE b) { return a = (a | b); }                      \
	static inline ENUM_TYPE operator&=(ENUM_TYPE& a, ENUM_TYPE b) { return a = (a & b); }

#endif
#else
#define MAKE_ENUM_FLAG(TYPE, ENUM_TYPE)
#endif

// Enable graphics validation in debug builds by default.
#if defined(HAWL_DEBUG) && !defined(DISABLE_GRAPHICS_DEBUG)
#define ENABLE_GRAPHICS_DEBUG
#endif

typedef enum RendererApi
{
    RENDERER_API_D3D12 = 0,
    RENDERER_API_VULKAN,
    RENDERER_API_XBOX_D3D12,
} RendererApi;


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

MAKE_ENUM_FLAG(uint32_t, QueueFlag)

typedef enum QueuePriority
{
    QUEUE_PRIORITY_NORMAL,
    QUEUE_PRIORITY_HIGH,
    QUEUE_PRIORITY_GLOBAL_REALTIME,
    MAX_QUEUE_PRIORITY
} QueuePriority;

typedef enum LoadActionType
{
    LOAD_ACTION_DONTCARE,
    LOAD_ACTION_LOAD,
    LOAD_ACTION_CLEAR,
    MAX_LOAD_ACTION
} LoadActionType;


typedef enum ResourceState
{
    RESOURCE_STATE_UNDEFINED = 0,
    RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
    RESOURCE_STATE_INDEX_BUFFER = 0x2,
    RESOURCE_STATE_RENDER_TARGET = 0x4,
    RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
    RESOURCE_STATE_DEPTH_WRITE = 0x10,
    RESOURCE_STATE_DEPTH_READ = 0x20,
    RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
    RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
    RESOURCE_STATE_SHADER_RESOURCE = 0x40 | 0x80,
    RESOURCE_STATE_STREAM_OUT = 0x100,
    RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
    RESOURCE_STATE_COPY_DEST = 0x400,
    RESOURCE_STATE_COPY_SOURCE = 0x800,
    RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
    RESOURCE_STATE_PRESENT = 0x1000,
    RESOURCE_STATE_COMMON = 0x2000,
    RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x4000,
} ResourceState;

MAKE_ENUM_FLAG(uint32_t, ResourceState)

/// Choosing Memory Type
typedef enum ResourceMemoryUsage
{
    /// No intended memory usage specified.
    RESOURCE_MEMORY_USAGE_UNKNOWN = 0,
    /// Memory will be used on device only, no need to be mapped on host.
    RESOURCE_MEMORY_USAGE_GPU_ONLY = 1,
    /// Memory will be mapped on host. Could be used for transfer to device.
    RESOURCE_MEMORY_USAGE_CPU_ONLY = 2,
    /// Memory will be used for frequent (dynamic) updates from host and reads on device.
    RESOURCE_MEMORY_USAGE_CPU_TO_GPU = 3,
    /// Memory will be used for writing on device and readback on host.
    RESOURCE_MEMORY_USAGE_GPU_TO_CPU = 4,
    RESOURCE_MEMORY_USAGE_COUNT,
    RESOURCE_MEMORY_USAGE_MAX_ENUM = 0x7FFFFFFF
} ResourceMemoryUsage;

typedef enum PresentStatus
{
    PRESENT_STATUS_SUCCESS = 0,
    PRESENT_STATUS_DEVICE_RESET = 1,
    PRESENT_STATUS_FAILED = 2
} PresentStatus;

// Forward declarations
typedef struct Renderer           Renderer;
typedef struct Queue              Queue;
typedef struct Pipeline           Pipeline;
typedef struct Buffer             Buffer;
typedef struct Texture            Texture;
typedef struct RenderTarget       RenderTarget;
typedef struct Shader             Shader;
typedef struct DescriptorSet      DescriptorSet;
typedef struct DescriptorIndexMap DescriptorIndexMap;
typedef struct PipelineCache      PipelineCache;

// Raytracing
typedef struct Raytracing            Raytracing;
typedef struct RaytracingHitGroup    RaytracingHitGroup;
typedef struct AccelerationStructure AccelerationStructure;

typedef struct EsramManager EsramManager;

typedef struct IndirectDrawArguments
{
    uint32_t mVertexCount;
    uint32_t mInstanceCount;
    uint32_t mStartVertex;
    uint32_t mStartInstance;
}            IndirectDrawArguments;

typedef struct IndirectDrawIndexArguments
{
    uint32_t mIndexCount;
    uint32_t mInstanceCount;
    uint32_t mStartIndex;
    uint32_t mVertexOffset;
    uint32_t mStartInstance;
}            IndirectDrawIndexArguments;

typedef struct IndirectDispatchArguments
{
    uint32_t mGroupCountX;
    uint32_t mGroupCountY;
    uint32_t mGroupCountZ;
}            IndirectDispatchArguments;

typedef enum IndirectArgumentType
{
    INDIRECT_DRAW,
    INDIRECT_DRAW_INDEX,
    INDIRECT_DISPATCH,
    INDIRECT_VERTEX_BUFFER,
    INDIRECT_INDEX_BUFFER,
    INDIRECT_CONSTANT,
    INDIRECT_DESCRIPTOR_TABLE,
    // only for vulkan
    INDIRECT_PIPELINE,
    // only for vulkan now, probally will add to dx when it comes to xbox
    INDIRECT_CONSTANT_BUFFER_VIEW,
    // only for dx
    INDIRECT_SHADER_RESOURCE_VIEW,
    // only for dx
    INDIRECT_UNORDERED_ACCESS_VIEW,
    // only for dx
#if defined(METAL)
	INDIRECT_COMMAND_BUFFER,          // metal ICB
	INDIRECT_COMMAND_BUFFER_RESET,    // metal ICB reset
	INDIRECT_COMMAND_BUFFER_OPTIMIZE  // metal ICB optimization
#endif
} IndirectArgumentType;

/************************************************/

typedef enum DescriptorType
{
    DESCRIPTOR_TYPE_UNDEFINED = 0,
    DESCRIPTOR_TYPE_SAMPLER = 0x01,
    // SRV Read only texture
    DESCRIPTOR_TYPE_TEXTURE = (DESCRIPTOR_TYPE_SAMPLER << 1),
    /// UAV Texture
    DESCRIPTOR_TYPE_RW_TEXTURE = (DESCRIPTOR_TYPE_TEXTURE << 1),
    // SRV Read only buffer
    DESCRIPTOR_TYPE_BUFFER = (DESCRIPTOR_TYPE_RW_TEXTURE << 1),
    DESCRIPTOR_TYPE_BUFFER_RAW = (DESCRIPTOR_TYPE_BUFFER | (DESCRIPTOR_TYPE_BUFFER << 1)),
    /// UAV Buffer
    DESCRIPTOR_TYPE_RW_BUFFER = (DESCRIPTOR_TYPE_BUFFER << 2),
    DESCRIPTOR_TYPE_RW_BUFFER_RAW = (DESCRIPTOR_TYPE_RW_BUFFER | (DESCRIPTOR_TYPE_RW_BUFFER << 1)),
    /// Uniform buffer
    DESCRIPTOR_TYPE_UNIFORM_BUFFER = (DESCRIPTOR_TYPE_RW_BUFFER << 2),
    /// Push constant / Root constant
    DESCRIPTOR_TYPE_ROOT_CONSTANT = (DESCRIPTOR_TYPE_UNIFORM_BUFFER << 1),
    /// IA
    DESCRIPTOR_TYPE_VERTEX_BUFFER = (DESCRIPTOR_TYPE_ROOT_CONSTANT << 1),
    DESCRIPTOR_TYPE_INDEX_BUFFER = (DESCRIPTOR_TYPE_VERTEX_BUFFER << 1),
    DESCRIPTOR_TYPE_INDIRECT_BUFFER = (DESCRIPTOR_TYPE_INDEX_BUFFER << 1),
    /// Cubemap SRV
    DESCRIPTOR_TYPE_TEXTURE_CUBE = (DESCRIPTOR_TYPE_TEXTURE | (DESCRIPTOR_TYPE_INDIRECT_BUFFER << 1)
    ),
    /// RTV / DSV per mip slice
    DESCRIPTOR_TYPE_RENDER_TARGET_MIP_SLICES = (DESCRIPTOR_TYPE_INDIRECT_BUFFER << 2),
    /// RTV / DSV per array slice
    DESCRIPTOR_TYPE_RENDER_TARGET_ARRAY_SLICES = (DESCRIPTOR_TYPE_RENDER_TARGET_MIP_SLICES << 1),
    /// RTV / DSV per depth slice
    DESCRIPTOR_TYPE_RENDER_TARGET_DEPTH_SLICES = (DESCRIPTOR_TYPE_RENDER_TARGET_ARRAY_SLICES << 1),
    DESCRIPTOR_TYPE_RAY_TRACING = (DESCRIPTOR_TYPE_RENDER_TARGET_DEPTH_SLICES << 1),
#if defined(VULKAN)
	/// Subpass input (descriptor type only available in Vulkan)
	DESCRIPTOR_TYPE_INPUT_ATTACHMENT = (DESCRIPTOR_TYPE_RAY_TRACING << 1),
	DESCRIPTOR_TYPE_TEXEL_BUFFER = (DESCRIPTOR_TYPE_INPUT_ATTACHMENT << 1),
	DESCRIPTOR_TYPE_RW_TEXEL_BUFFER = (DESCRIPTOR_TYPE_TEXEL_BUFFER << 1),
	DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = (DESCRIPTOR_TYPE_RW_TEXEL_BUFFER << 1),
#endif
} DescriptorType;

MAKE_ENUM_FLAG(uint32_t, DescriptorType)

typedef enum SampleCount
{
    SAMPLE_COUNT_1 = 1,
    SAMPLE_COUNT_2 = 2,
    SAMPLE_COUNT_4 = 4,
    SAMPLE_COUNT_8 = 8,
    SAMPLE_COUNT_16 = 16,
} SampleCount;

typedef enum ShaderStage
{
    SHADER_STAGE_NONE = 0,
    SHADER_STAGE_VERT = 0X00000001,
    SHADER_STAGE_TESC = 0X00000002,
    SHADER_STAGE_TESE = 0X00000004,
    SHADER_STAGE_GEOM = 0X00000008,
    SHADER_STAGE_FRAG = 0X00000010,
    SHADER_STAGE_COMP = 0X00000020,
    SHADER_STAGE_RAYTRACING = 0X00000040,
    SHADER_STAGE_ALL_GRAPHICS = ((uint32_t)SHADER_STAGE_VERT | (uint32_t)SHADER_STAGE_TESC | (
                                     uint32_t)SHADER_STAGE_TESE | (uint32_t)SHADER_STAGE_GEOM | (
                                     uint32_t)SHADER_STAGE_FRAG),
    SHADER_STAGE_HULL = SHADER_STAGE_TESC,
    SHADER_STAGE_DOMN = SHADER_STAGE_TESE,
    SHADER_STAGE_COUNT = 7,
} ShaderStage;

MAKE_ENUM_FLAG(uint32_t, ShaderStage)

// This include is placed here because it uses data types defined previously in this file
// and forward enums are not allowed for some compilers (Xcode).
#include "IShaderReflection.h"

typedef enum PrimitiveTopology
{
    PRIMITIVE_TOPO_POINT_LIST = 0,
    PRIMITIVE_TOPO_LINE_LIST,
    PRIMITIVE_TOPO_LINE_STRIP,
    PRIMITIVE_TOPO_TRI_LIST,
    PRIMITIVE_TOPO_TRI_STRIP,
    PRIMITIVE_TOPO_PATCH_LIST,
    PRIMITIVE_TOPO_COUNT,
} PrimitiveTopology;

typedef enum IndexType
{
    INDEX_TYPE_UINT32 = 0,
    INDEX_TYPE_UINT16,
} IndexType;

typedef enum ShaderSemantic
{
    SEMANTIC_UNDEFINED = 0,
    SEMANTIC_POSITION,
    SEMANTIC_NORMAL,
    SEMANTIC_COLOR,
    SEMANTIC_TANGENT,
    SEMANTIC_BITANGENT,
    SEMANTIC_JOINTS,
    SEMANTIC_WEIGHTS,
    SEMANTIC_TEXCOORD0,
    SEMANTIC_TEXCOORD1,
    SEMANTIC_TEXCOORD2,
    SEMANTIC_TEXCOORD3,
    SEMANTIC_TEXCOORD4,
    SEMANTIC_TEXCOORD5,
    SEMANTIC_TEXCOORD6,
    SEMANTIC_TEXCOORD7,
    SEMANTIC_TEXCOORD8,
    SEMANTIC_TEXCOORD9,
} ShaderSemantic;

typedef enum BlendConstant
{
    BC_ZERO = 0,
    BC_ONE,
    BC_SRC_COLOR,
    BC_ONE_MINUS_SRC_COLOR,
    BC_DST_COLOR,
    BC_ONE_MINUS_DST_COLOR,
    BC_SRC_ALPHA,
    BC_ONE_MINUS_SRC_ALPHA,
    BC_DST_ALPHA,
    BC_ONE_MINUS_DST_ALPHA,
    BC_SRC_ALPHA_SATURATE,
    BC_BLEND_FACTOR,
    BC_ONE_MINUS_BLEND_FACTOR,
    MAX_BLEND_CONSTANTS
} BlendConstant;

typedef enum BlendMode
{
    BM_ADD,
    BM_SUBTRACT,
    BM_REVERSE_SUBTRACT,
    BM_MIN,
    BM_MAX,
    MAX_BLEND_MODES,
} BlendMode;

typedef enum CompareMode
{
    CMP_NEVER,
    CMP_LESS,
    CMP_EQUAL,
    CMP_LEQUAL,
    CMP_GREATER,
    CMP_NOTEQUAL,
    CMP_GEQUAL,
    CMP_ALWAYS,
    MAX_COMPARE_MODES,
} CompareMode;

typedef enum StencilOp
{
    STENCIL_OP_KEEP,
    STENCIL_OP_SET_ZERO,
    STENCIL_OP_REPLACE,
    STENCIL_OP_INVERT,
    STENCIL_OP_INCR,
    STENCIL_OP_DECR,
    STENCIL_OP_INCR_SAT,
    STENCIL_OP_DECR_SAT,
    MAX_STENCIL_OPS,
} StencilOp;

static const int RED = 0x1;
static const int GREEN = 0x2;
static const int BLUE = 0x4;
static const int ALPHA = 0x8;
static const int ALL = (RED | GREEN | BLUE | ALPHA);
static const int NONE = 0;

static const int BS_NONE = -1;
static const int DS_NONE = -1;
static const int RS_NONE = -1;

// Blend states are always attached to one of the eight or more render targets that
// are in a MRT
// Mask constants
typedef enum BlendStateTargets
{
    BLEND_STATE_TARGET_0 = 0x1,
    BLEND_STATE_TARGET_1 = 0x2,
    BLEND_STATE_TARGET_2 = 0x4,
    BLEND_STATE_TARGET_3 = 0x8,
    BLEND_STATE_TARGET_4 = 0x10,
    BLEND_STATE_TARGET_5 = 0x20,
    BLEND_STATE_TARGET_6 = 0x40,
    BLEND_STATE_TARGET_7 = 0x80,
    BLEND_STATE_TARGET_ALL = 0xFF,
} BlendStateTargets;

MAKE_ENUM_FLAG(uint32_t, BlendStateTargets)

typedef enum CullMode
{
    CULL_MODE_NONE = 0,
    CULL_MODE_BACK,
    CULL_MODE_FRONT,
    CULL_MODE_BOTH,
    MAX_CULL_MODES
} CullMode;

typedef enum FrontFace
{
    FRONT_FACE_CCW = 0,
    FRONT_FACE_CW
} FrontFace;

typedef enum FillMode
{
    FILL_MODE_SOLID,
    FILL_MODE_WIREFRAME,
    MAX_FILL_MODES
} FillMode;

typedef enum PipelineType
{
    PIPELINE_TYPE_UNDEFINED = 0,
    PIPELINE_TYPE_COMPUTE,
    PIPELINE_TYPE_GRAPHICS,
    PIPELINE_TYPE_RAYTRACING,
    PIPELINE_TYPE_COUNT,
} PipelineType;

typedef enum FilterType
{
    FILTER_NEAREST = 0,
    FILTER_LINEAR,
} FilterType;

typedef enum AddressMode
{
    ADDRESS_MODE_MIRROR,
    ADDRESS_MODE_REPEAT,
    ADDRESS_MODE_CLAMP_TO_EDGE,
    ADDRESS_MODE_CLAMP_TO_BORDER
} AddressMode;

typedef enum MipMapMode
{
    MIPMAP_MODE_NEAREST = 0,
    MIPMAP_MODE_LINEAR
} MipMapMode;

typedef union ClearValue
{
    struct
    {
        float r;
        float g;
        float b;
        float a;
    };

    struct
    {
        float    depth;
        uint32_t stencil;
    };
} ClearValue;

typedef enum BufferCreationFlags
{
    /// Default flag (Buffer will use aliased memory, buffer will not be cpu accessible until mapBuffer is called)
    BUFFER_CREATION_FLAG_NONE = 0x01,
    /// Buffer will allocate its own memory (COMMITTED resource)
    BUFFER_CREATION_FLAG_OWN_MEMORY_BIT = 0x02,
    /// Buffer will be persistently mapped
    BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT = 0x04,
    /// Use ESRAM to store this buffer
    BUFFER_CREATION_FLAG_ESRAM = 0x08,
    /// Flag to specify not to allocate descriptors for the resource
    BUFFER_CREATION_FLAG_NO_DESCRIPTOR_VIEW_CREATION = 0x10,

#ifdef METAL
    /* ICB Flags */
    /// Ihnerit pipeline in ICB
    BUFFER_CREATION_FLAG_ICB_INHERIT_PIPELINE = 0x100,
    /// Ihnerit pipeline in ICB
    BUFFER_CREATION_FLAG_ICB_INHERIT_BUFFERS = 0x200,
    
#endif
} BufferCreationFlags;

MAKE_ENUM_FLAG(uint32_t, BufferCreationFlags)

typedef enum TextureCreationFlags
{
    /// Default flag (Texture will use default allocation strategy decided by the api specific allocator)
    TEXTURE_CREATION_FLAG_NONE = 0,
    /// Texture will allocate its own memory (COMMITTED resource)
    TEXTURE_CREATION_FLAG_OWN_MEMORY_BIT = 0x01,
    /// Texture will be allocated in memory which can be shared among multiple processes
    TEXTURE_CREATION_FLAG_EXPORT_BIT = 0x02,
    /// Texture will be allocated in memory which can be shared among multiple gpus
    TEXTURE_CREATION_FLAG_EXPORT_ADAPTER_BIT = 0x04,
    /// Texture will be imported from a handle created in another process
    TEXTURE_CREATION_FLAG_IMPORT_BIT = 0x08,
    /// Use ESRAM to store this texture
    TEXTURE_CREATION_FLAG_ESRAM = 0x10,
    /// Use on-tile memory to store this texture
    TEXTURE_CREATION_FLAG_ON_TILE = 0x20,
    /// Prevent compression meta data from generating (XBox)
    TEXTURE_CREATION_FLAG_NO_COMPRESSION = 0x40,
    /// Force 2D instead of automatically determining dimension based on width, height, depth
    TEXTURE_CREATION_FLAG_FORCE_2D = 0x80,
    /// Force 3D instead of automatically determining dimension based on width, height, depth
    TEXTURE_CREATION_FLAG_FORCE_3D = 0x100,
    /// Display target
    TEXTURE_CREATION_FLAG_ALLOW_DISPLAY_TARGET = 0x200,
    /// Create an sRGB texture.
    TEXTURE_CREATION_FLAG_SRGB = 0x400,
} TextureCreationFlags;

MAKE_ENUM_FLAG(uint32_t, TextureCreationFlags)

typedef enum GPUPresetLevel
{
    GPU_PRESET_NONE = 0,
    GPU_PRESET_OFFICE,
    //This means unsupported
    GPU_PRESET_LOW,
    GPU_PRESET_MEDIUM,
    GPU_PRESET_HIGH,
    GPU_PRESET_ULTRA,
    GPU_PRESET_COUNT
} GPUPresetLevel;

typedef struct BufferBarrier
{
    Buffer *      pBuffer;
    ResourceState mCurrentState;
    ResourceState mNewState;
    uint8_t mBeginOnly : 1;
    uint8_t mEndOnly : 1;
    uint8_t mAcquire : 1;
    uint8_t mRelease : 1;
    uint8_t mQueueType : 5;
} BufferBarrier;

typedef struct TextureBarrier
{
    Texture *     pTexture;
    ResourceState mCurrentState;
    ResourceState mNewState;
    uint8_t mBeginOnly : 1;
    uint8_t mEndOnly : 1;
    uint8_t mAcquire : 1;
    uint8_t mRelease : 1;
    uint8_t mQueueType : 5;
    /// Specifiy whether following barrier targets particular subresource
    uint8_t mSubresourceBarrier : 1;
    /// Following values are ignored if mSubresourceBarrier is false
    uint8_t mMipLevel : 7;
    uint16_t mArrayLayer;
}            TextureBarrier;

typedef struct RenderTargetBarrier
{
    RenderTarget *pRenderTarget;
    ResourceState mCurrentState;
    ResourceState mNewState;
    uint8_t mBeginOnly : 1;
    uint8_t mEndOnly : 1;
    uint8_t mAcquire : 1;
    uint8_t mRelease : 1;
    uint8_t mQueueType : 5;
    /// Specifiy whether following barrier targets particular subresource
    uint8_t mSubresourceBarrier : 1;
    /// Following values are ignored if mSubresourceBarrier is false
    uint8_t mMipLevel : 7;
    uint16_t mArrayLayer;
}            RenderTargetBarrier;

typedef struct ReadRange
{
    uint64_t mOffset;
    uint64_t mSize;
}            ReadRange;

typedef enum QueryType
{
    QUERY_TYPE_TIMESTAMP = 0,
    QUERY_TYPE_PIPELINE_STATISTICS,
    QUERY_TYPE_OCCLUSION,
    QUERY_TYPE_COUNT,
} QueryType;

typedef struct QueryPoolDesc
{
    QueryType mType;
    uint32_t  mQueryCount;
    uint32_t  mNodeIndex;
}             QueryPoolDesc;

typedef struct QueryDesc
{
    uint32_t mIndex;
}            QueryDesc;

typedef struct QueryPool
{
#if D3D12_SUPPORTED
    ID3D12QueryHeap *pDxQueryHeap;
    D3D12_QUERY_TYPE mType;
    uint32_t         mCount;
#endif
#if defined(VULKAN)
	VkQueryPool       pVkQueryPool;
	VkQueryType       mType;
	uint32_t          mCount;
#endif
} QueryPool;

#if defined (VULKAN)
typedef enum SamplerRange
{
	SAMPLER_RANGE_FULL = 0,
	SAMPLER_RANGE_NARROW = 1,
} SamplerRange;

typedef enum SamplerModelConversion
{
	SAMPLER_MODEL_CONVERSION_RGB_IDENTITY = 0,
	SAMPLER_MODEL_CONVERSION_YCBCR_IDENTITY = 1,
	SAMPLER_MODEL_CONVERSION_YCBCR_709 = 2,
	SAMPLER_MODEL_CONVERSION_YCBCR_601 = 3,
	SAMPLER_MODEL_CONVERSION_YCBCR_2020 = 4,
} SamplerModelConversion;

typedef enum SampleLocation
{
	SAMPLE_LOCATION_COSITED = 0,
	SAMPLE_LOCATION_MIDPOINT = 1,
} SampleLocation;
#endif

/// Data structure holding necessary info to create a Buffer
typedef struct BufferDesc
{
    /// Size of the buffer (in bytes)
    uint64_t mSize;
    /// Alignment
    uint32_t mAlignment;
    /// Decides which memory heap buffer will use (default, upload, readback)
    ResourceMemoryUsage mMemoryUsage;
    /// Creation flags of the buffer
    BufferCreationFlags mFlags;
    /// What type of queue the buffer is owned by
    QueueType mQueueType;
    /// What state will the buffer get created in
    ResourceState mStartState;
    /// Index of the first element accessible by the SRV/UAV (applicable to BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
    uint64_t mFirstElement;
    /// Number of elements in the buffer (applicable to BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
    uint64_t mElementCount;
    /// Size of each element (in bytes) in the buffer (applicable to BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
    uint64_t mStructStride;
    /// ICB draw type
    IndirectArgumentType mICBDrawType;
    /// ICB max vertex buffers slots count
    uint32_t mICBMaxVertexBufferBind;
    /// ICB max vertex buffers slots count
    uint32_t mICBMaxFragmentBufferBind;
    /// Set this to specify a counter buffer for this buffer (applicable to BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
    struct Buffer *pCounterBuffer;
    /// Format of the buffer (applicable to typed storage buffers (Buffer<T>)
    TextureFormat mFormat;
    /// Flags specifying the suitable usage of this buffer (Uniform buffer, Vertex Buffer, Index Buffer,...)
    DescriptorType mDescriptors;
    /// Debug name used in gpu profile
    const char *pName;
    uint32_t *  pSharedNodeIndices;
    uint32_t    mNodeIndex;
    uint32_t    mSharedNodeIndexCount;
}               BufferDesc;

typedef struct DEFINE_ALIGNED(Buffer, 64)
{
    /// CPU address of the mapped buffer (appliacable to buffers created in CPU accessible heaps (CPU, CPU_TO_GPU, GPU_TO_CPU)
    void *pCpuMappedAddress;
#if D3D12_SUPPORTED
    /// GPU Address - Cache to avoid calls to ID3D12Resource::GetGpuVirtualAddress
    D3D12_GPU_VIRTUAL_ADDRESS mDxGpuAddress;
    /// Descriptor handle of the CBV in a CPU visible descriptor heap (applicable to BUFFER_USAGE_UNIFORM)
    D3D12_CPU_DESCRIPTOR_HANDLE mDxDescriptorHandles;
    /// Offset from mDxDescriptors for srv descriptor handle
    uint64_t mDxSrvOffset : 8;
    /// Offset from mDxDescriptors for uav descriptor handle
    uint64_t mDxUavOffset : 8;
    /// Native handle of the underlying resource
    ID3D12Resource *pDxResource;
    /// Contains resource allocation info such as parent heap, offset in heap
    D3D12MA::Allocation *pDxAllocation;
#endif
#if defined(VULKAN)
	/// Native handle of the underlying resource
	VkBuffer                         pVkBuffer;
	/// Buffer view
	VkBufferView                     pVkStorageTexelView;
	VkBufferView                     pVkUniformTexelView;
	/// Contains resource allocation info such as parent heap, offset in heap
	struct VmaAllocation_T*          pVkAllocation;
	uint64_t                         mOffset;
#endif
    uint64_t mSize : 32;
    uint64_t mDescriptors : 20;
    uint64_t mMemoryUsage : 3;
    uint64_t mNodeIndex : 4;
} Buffer;

// One cache line
COMPILE_ASSERT(sizeof(Buffer) == 8 * sizeof(uint64_t));

/// Data structure holding necessary info to create a Texture
typedef struct TextureDesc
{
    /// Texture creation flags (decides memory allocation strategy, sharing access,...)
    TextureCreationFlags mFlags;
    /// Width
    uint32_t mWidth;
    /// Height
    uint32_t mHeight;
    /// Depth (Should be 1 if not a mType is not TEXTURE_TYPE_3D)
    uint32_t mDepth;
    /// Texture array size (Should be 1 if texture is not a texture array or cubemap)
    uint32_t mArraySize;
    /// Number of mip levels
    uint32_t mMipLevels;
    /// Number of multisamples per pixel (currently Textures created with mUsage TEXTURE_USAGE_SAMPLED_IMAGE only support SAMPLE_COUNT_1)
    SampleCount mSampleCount;
    /// The image quality level. The higher the quality, the lower the performance. The valid range is between zero and the value appropriate for mSampleCount
    uint32_t mSampleQuality;
    ///  image format
    TextureFormat mFormat;
    /// Optimized clear value (recommended to use this same value when clearing the rendertarget)
    ClearValue mClearValue;
    /// What state will the texture get created in
    ResourceState mStartState;
    /// Descriptor creation
    DescriptorType mDescriptors;
    /// Pointer to native texture handle if the texture does not own underlying resource
    const void *pNativeHandle;
    /// Debug name used in gpu profile
    const char *pName;
    /// GPU indices to share this texture
    uint32_t *pSharedNodeIndices;
    /// Number of GPUs to share this texture
    uint32_t mSharedNodeIndexCount;
    /// GPU which will own this texture
    uint32_t mNodeIndex;
#if defined(VULKAN)
	VkSamplerYcbcrConversionInfo* pVkSamplerYcbcrConversionInfo;
#endif
} TextureDesc;

// Virtual texture page as a part of the partially resident texture
// Contains memory bindings, offsets and status information
struct VirtualTexturePage
{
    /// Buffer which contains the image data and be used for copying it to Virtual texture
    Buffer *pIntermediateBuffer;
    /// Miplevel for this page
    uint32_t mipLevel;
    /// Array layer for this page
    uint32_t layer;
    /// Index for this page
    uint32_t index;
#if D3D12_SUPPORTED
    /// Offset for this page
    D3D12_TILED_RESOURCE_COORDINATE offset;
    /// Size for this page
    D3D12_TILED_RESOURCE_COORDINATE extent;
    /// Byte size for this page
    uint32_t size;
#endif

#if defined(VULKAN)
	/// Offset for this page
	VkOffset3D offset;
	/// Size for this page
	VkExtent3D extent;
	/// Sparse image memory bind for this page
	VkSparseImageMemoryBind imageMemoryBind;
	/// Byte size for this page
	VkDeviceSize size;
#endif
};

typedef struct VirtualTexture
{
#if D3D12_SUPPORTED
    ID3D12Heap *pSparseImageMemory;
    /// Array for Sparse texture's pages
    void *pSparseCoordinates;
    /// Array for heap memory offsets
    void *pHeapRangeStartOffsets;
#endif
#if defined(VULKAN)
	/// Sparse queue binding information
	VkBindSparseInfo mBindSparseInfo;
	/// Sparse image memory bindings of all memory-backed virtual tables
	void* pSparseImageMemoryBinds;
	/// Sparse ?aque memory bindings for the mip tail (if present)	
	void* pOpaqueMemoryBinds;
	/// First mip level in mip tail
	uint32_t mMipTailStart;
	/// Lstly filled mip level in mip tail
	uint32_t mLastFilledMip;
	/// Memory type for Sparse texture's memory
	uint32_t mSparseMemoryTypeIndex;
	/// Sparse image memory bind info 
	VkSparseImageMemoryBindInfo mImageMemoryBindInfo;
	/// Sparse image opaque memory bind info (mip tail)
	VkSparseImageOpaqueMemoryBindInfo mOpaqueMemoryBindInfo;
	/// First mip level in mip tail
	uint32_t mipTailStart;
#endif
    /// Virtual Texture members
    /// Contains all virtual pages of the texture
    void *pPages;
    /// Visibility data
    Buffer *mVisibility;
    /// PrevVisibility data
    Buffer *mPrevVisibility;
    /// Alive Page's Index
    Buffer *mAlivePage;
    /// Page's Index which should be removed
    Buffer *mRemovePage;
    /// a { uint alive; uint remove; } count of pages which are alive or should be removed
    Buffer *mPageCounts;
    /// Original Pixel image data
    void *mVirtualImageData;
    ///  Total pages count
    uint32_t mVirtualPageTotalCount;
    /// Sparse Virtual Texture Width
    uint64_t mSparseVirtualTexturePageWidth;
    /// Sparse Virtual Texture Height
    uint64_t mSparseVirtualTexturePageHeight;
}            VirtualTexture;

typedef struct DEFINE_ALIGNED(Texture, 64)
{
#if D3D12_SUPPORTED
    /// Descriptor handle of the SRV in a CPU visible descriptor heap (applicable to TEXTURE_USAGE_SAMPLED_IMAGE)
    D3D12_CPU_DESCRIPTOR_HANDLE mDxDescriptorHandles;
    /// Native handle of the underlying resource
    ID3D12Resource *pDxResource;
    /// Contains resource allocation info such as parent heap, offset in heap
    D3D12MA::Allocation *pDxAllocation;
    uint64_t mHandleCount : 24;
    uint64_t mUavStartIndex : 1;
    uint32_t mDescriptorSize;
#endif
#if defined(VULKAN)
	/// Opaque handle used by shaders for doing read/write operations on the texture
	VkImageView                  pVkSRVDescriptor;
	/// Opaque handle used by shaders for doing read/write operations on the texture
	VkImageView*                 pVkUAVDescriptors;
	/// Opaque handle used by shaders for doing read/write operations on the texture
	VkImageView                  pVkSRVStencilDescriptor;
	/// Native handle of the underlying resource
	VkImage                      pVkImage;
	union
	{
		/// Contains resource allocation info such as parent heap, offset in heap
		struct VmaAllocation_T*  pVkAllocation;
		VkDeviceMemory           pVkDeviceMemory;
	};
#endif
    VirtualTexture *pSvt;
    /// Current state of the buffer
    uint32_t mWidth : 16;
    uint32_t mHeight : 16;
    uint32_t mDepth : 16;
    uint32_t mMipLevels : 5;
    uint32_t mArraySizeMinusOne : 11;
    uint32_t mFormat : 8;
    /// Flags specifying which aspects (COLOR,DEPTH,STENCIL) are included in the pVkImageView
    uint32_t mAspectMask : 4;
    uint32_t mNodeIndex : 4;
    uint32_t mUav : 1;
    /// This value will be false if the underlying resource is not owned by the texture (swapchain textures,...)
    uint32_t mOwnsImage : 1;
} Texture;

// One cache line
COMPILE_ASSERT(sizeof(Texture) == 8 * sizeof(uint64_t));

typedef struct RenderTargetDesc
{
    /// Texture creation flags (decides memory allocation strategy, sharing access,...)
    TextureCreationFlags mFlags;
    /// Width
    uint32_t mWidth;
    /// Height
    uint32_t mHeight;
    /// Depth (Should be 1 if not a mType is not TEXTURE_TYPE_3D)
    uint32_t mDepth;
    /// Texture array size (Should be 1 if texture is not a texture array or cubemap)
    uint32_t mArraySize;
    /// Number of mip levels
    uint32_t mMipLevels;
    /// MSAA
    SampleCount mSampleCount;
    /// Internal image format
    TextureFormat mFormat;
    /// What state will the texture get created in
    ResourceState mStartState;
    /// Optimized clear value (recommended to use this same value when clearing the rendertarget)
    ClearValue mClearValue;
    /// The image quality level. The higher the quality, the lower the performance. The valid range is between zero and the value appropriate for mSampleCount
    uint32_t mSampleQuality;
    /// Descriptor creation
    DescriptorType mDescriptors;
    const void *   pNativeHandle;
    /// Debug name used in gpu profile
    const char *pName;
    /// GPU indices to share this texture
    uint32_t *pSharedNodeIndices;
    /// Number of GPUs to share this texture
    uint32_t mSharedNodeIndexCount;
    /// GPU which will own this texture
    uint32_t mNodeIndex;
}            RenderTargetDesc;

typedef struct DEFINE_ALIGNED(RenderTarget, 64)
{
    Texture *pTexture;

#if D3D12_SUPPORTED
    D3D12_CPU_DESCRIPTOR_HANDLE mDxDescriptors;
    uint32_t                    mDxDescriptorSize;
    uint32_t                    mPadA;
    uint64_t                    mPadB;
    uint64_t                    mPadC;
#endif
#if defined(VULKAN)
	VkImageView                   pVkDescriptor;
	VkImageView*                  pVkSliceDescriptors;
	uint32_t                      mId;
	volatile uint32_t             mUsed;
#endif
    ClearValue mClearValue;
    uint32_t mArraySize : 16;
    uint32_t mDepth : 16;
    uint32_t mWidth : 16;
    uint32_t mHeight : 16;
    uint32_t mDescriptors : 20;
    uint32_t mMipLevels : 10;
    uint32_t mSampleQuality : 5;
    TextureFormat mFormat;
    SampleCount   mSampleCount;
}                 RenderTarget;

COMPILE_ASSERT(sizeof(RenderTarget) <= 32 * sizeof(uint64_t));

typedef struct LoadActionsDesc
{
    ClearValue     mClearColorValues[MAX_RENDER_TARGET_ATTACHMENTS];
    LoadActionType mLoadActionsColor[MAX_RENDER_TARGET_ATTACHMENTS];
    ClearValue     mClearDepth;
    LoadActionType mLoadActionDepth;
    LoadActionType mLoadActionStencil;
}                  LoadActionsDesc;

typedef struct SamplerDesc
{
    FilterType  mMinFilter;
    FilterType  mMagFilter;
    MipMapMode  mMipMapMode;
    AddressMode mAddressU;
    AddressMode mAddressV;
    AddressMode mAddressW;
    float       mMipLodBias;
    float       mMaxAnisotropy;
    CompareMode mCompareFunc;

#if defined(VULKAN)
   struct
   {
      TextureFormat         mFormat;
      SamplerModelConversion  mModel;
      SamplerRange            mRange;
      SampleLocation          mChromaOffsetX;
      SampleLocation          mChromaOffsetY;
      FilterType              mChromaFilter;
      bool                    mForceExplicitReconstruction;
   } mSamplerConversionDesc;
#endif
} SamplerDesc;

typedef struct DEFINE_ALIGNED(Sampler, 16)
{
#if D3D12_SUPPORTED
    /// Description for creating the Sampler descriptor for this sampler
    D3D12_SAMPLER_DESC mDxDesc;
    /// Descriptor handle of the Sampler in a CPU visible descriptor heap
    D3D12_CPU_DESCRIPTOR_HANDLE mDxHandle;
#endif
#if defined(VULKAN)
	/// Native handle of the underlying resource
	VkSampler                       pVkSampler;
	VkSamplerYcbcrConversion        pVkSamplerYcbcrConversion;
	VkSamplerYcbcrConversionInfo    mVkSamplerYcbcrConversionInfo;
#endif
} Sampler;

#if D3D12_SUPPORTED
COMPILE_ASSERT(sizeof(Sampler) == 8 * sizeof(uint64_t));
#elif defined(VULKAN)
COMPILE_ASSERT(sizeof(Sampler) <= 8 * sizeof(uint64_t));
#endif


typedef enum DescriptorUpdateFrequency
{
    DESCRIPTOR_UPDATE_FREQ_NONE = 0,
    DESCRIPTOR_UPDATE_FREQ_PER_FRAME,
    DESCRIPTOR_UPDATE_FREQ_PER_BATCH,
    DESCRIPTOR_UPDATE_FREQ_PER_DRAW,
    DESCRIPTOR_UPDATE_FREQ_COUNT,
} DescriptorUpdateFrequency;

/// Data structure holding the layout for a descriptor
typedef struct DEFINE_ALIGNED(DescriptorInfo, 16)
{
    const char *pName;
    uint32_t mType : 21;
    uint32_t mDim : 4;
    uint32_t mRootDescriptor : 1;
    uint32_t mUpdateFrequency : 3;
    uint32_t mSize;
    /// Index in the descriptor set
    uint32_t mIndexInParent;
    uint32_t mHandleIndex;
#if defined(VULKAN)
	uint32_t                  mVkType;
	uint32_t                  mReg : 20;
	uint32_t                  mRootDescriptorIndex : 3;
	uint32_t                  mVkStages : 8;
#elif defined(DIRECT3D11)
	uint32_t                  mUsedStages : 6;
	uint32_t                  mReg : 20;
	uint32_t                  mPadA;
#elif D3D12_SUPPORTED
    uint64_t mPadA;
#endif
} DescriptorInfo;


typedef enum RootSignatureFlags
{
    /// Default flag
    ROOT_SIGNATURE_FLAG_NONE = 0,
    /// Local root signature used mainly in raytracing shaders
    ROOT_SIGNATURE_FLAG_LOCAL_BIT = 0x1,
} RootSignatureFlags;

MAKE_ENUM_FLAG(uint32_t, RootSignatureFlags)

typedef struct RootSignatureDesc
{
    Shader **          ppShaders;
    uint32_t           mShaderCount;
    uint32_t           mMaxBindlessTextures;
    const char **      ppStaticSamplerNames;
    Sampler **         ppStaticSamplers;
    uint32_t           mStaticSamplerCount;
    RootSignatureFlags mFlags;
}                      RootSignatureDesc;

typedef struct DEFINE_ALIGNED(RootSignature, 64)
{
    /// Number of descriptors declared in the root signature layout
    uint32_t mDescriptorCount;
    /// Graphics or Compute
    PipelineType mPipelineType;
    /// Array of all descriptors declared in the root signature layout
    DescriptorInfo *pDescriptors;
    /// Translates hash of descriptor name to descriptor index in pDescriptors array
    DescriptorIndexMap *pDescriptorNameToIndexMap;
#if D3D12_SUPPORTED
    ID3D12RootSignature *pDxRootSignature;
    uint8_t              mDxRootConstantRootIndices[MAX_ROOT_CONSTANTS_PER_ROOTSIGNATURE];
    uint8_t              mDxViewDescriptorTableRootIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint8_t              mDxSamplerDescriptorTableRootIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint8_t              mDxRootDescriptorRootIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint32_t             mDxCumulativeViewDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint32_t             mDxCumulativeSamplerDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint16_t             mDxViewDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint16_t             mDxSamplerDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint8_t              mDxRootDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint32_t             mDxRootConstantCount;
    uint64_t             mPadA;
    uint64_t             mPadB;
#endif
#if defined(VULKAN)
	VkDescriptorSetLayout      mVkDescriptorSetLayouts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint32_t                   mVkCumulativeDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint16_t                   mVkDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint8_t                    mVkDynamicDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint8_t                    mVkRaytracingDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	VkPipelineLayout           pPipelineLayout;
	VkDescriptorUpdateTemplate mUpdateTemplates[DESCRIPTOR_UPDATE_FREQ_COUNT];
	VkDescriptorSet            mVkEmptyDescriptorSets[DESCRIPTOR_UPDATE_FREQ_COUNT];
	void**                     pUpdateTemplateData[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint32_t                   mVkPushConstantCount;
	uint32_t                   mPadA;
	uint64_t                   mPadB[7];
#endif
} RootSignature;

#if defined(VULKAN)
// 4 cache lines
COMPILE_ASSERT(sizeof(RootSignature) == 32 * sizeof(uint64_t));
#else
// 2 cache lines
COMPILE_ASSERT(sizeof(RootSignature) <= 16 * sizeof(uint64_t));
#endif

typedef struct DescriptorData
{
    /// User can either set name of descriptor or index (index in pRootSignature->pDescriptors array)
    /// Name of descriptor
    const char *pName;

    union
    {
        struct
        {
            /// Offset to bind the buffer descriptor
            const uint64_t *pOffsets;
            const uint64_t *pSizes;
        };

        // Descriptor set buffer extraction options
        struct
        {
            uint32_t    mDescriptorSetBufferIndex;
            Shader *    mDescriptorSetShader;
            ShaderStage mDescriptorSetShaderStage;
        };

        struct
        {
            uint32_t mUAVMipSlice;
            bool     mBindMipChain;
        };

        bool mBindStencilResource;
    };

    /// Array of resources containing descriptor handles or constant to be used in ring buffer memory - DescriptorRange can hold only one resource type array
    union
    {
        /// Array of texture descriptors (srv and uav textures)
        Texture **ppTextures;
        /// Array of sampler descriptors
        Sampler **ppSamplers;
        /// Array of buffer descriptors (srv, uav and cbv buffers)
        Buffer **ppBuffers;
        /// Array of pipline descriptors
        Pipeline **ppPipelines;
        /// DescriptorSet buffer extraction
        DescriptorSet **ppDescriptorSet;
        /// Custom binding (raytracing acceleration structure ...)
        AccelerationStructure **ppAccelerationStructures;
    };

    /// Number of resources in the descriptor(applies to array of textures, buffers,...)
    uint32_t mCount;
    uint32_t mIndex = (uint32_t)-1;
    bool     mExtractBuffer = false;
}            DescriptorData;

typedef struct DEFINE_ALIGNED(DescriptorSet, 64)
{
#if D3D12_SUPPORTED
    /// Start handle to cbv srv uav descriptor table
    uint64_t mCbvSrvUavHandle;
    /// Start handle to sampler descriptor table
    uint64_t mSamplerHandle;
    /// Stride of the cbv srv uav descriptor table (number of descriptors * descriptor size)
    uint32_t mCbvSrvUavStride;
    /// Stride of the sampler descriptor table (number of descriptors * descriptor size)
    uint32_t                   mSamplerStride;
    const RootSignature *      pRootSignature;
    D3D12_GPU_VIRTUAL_ADDRESS *pRootAddresses;
    ID3D12RootSignature *      pRootSignatureHandle;
    uint64_t mMaxSets : 16;
    uint64_t mUpdateFrequency : 3;
    uint64_t mNodeIndex : 4;
    uint64_t mRootAddressCount : 1;
    uint64_t mCbvSrvUavRootIndex : 4;
    uint64_t mSamplerRootIndex : 4;
    uint64_t mRootDescriptorRootIndex : 4;
    uint64_t mPipelineType : 3;
#elif defined(VULKAN)
	VkDescriptorSet*              pHandles;
	const RootSignature*          pRootSignature;
	/// Values passed to vkUpdateDescriptorSetWithTemplate. Initialized to default descriptor values.
	union DescriptorUpdateData**  ppUpdateData;
	struct SizeOffset*            pDynamicSizeOffsets;
	uint32_t                      mMaxSets;
	uint8_t                       mDynamicOffsetCount;
	uint8_t                       mUpdateFrequency;
	uint8_t                       mNodeIndex;
	uint8_t                       mPadA;
#endif
} DescriptorSet;

typedef struct CmdPoolDesc
{
    Queue *pQueue;
    bool   mTransient;
}          CmdPoolDesc;

typedef struct CmdPool
{
#if D3D12_SUPPORTED
    ID3D12CommandAllocator *pDxCmdAlloc;
#endif
#if defined(VULKAN)
	VkCommandPool           pVkCmdPool;
#endif
    Queue *pQueue;
}          CmdPool;

typedef struct CmdDesc
{
    CmdPool *pPool;
    bool     mSecondary;
}            CmdDesc;

typedef enum MarkerType
{
    MARKER_TYPE_DEFAULT = 0x0,
    MARKER_TYPE_IN = 0x1,
    MARKER_TYPE_OUT = 0x2,
    MARKER_TYPE_IN_OUT = 0x3,
} MarkerType;

typedef struct DEFINE_ALIGNED(Cmd, 64)
{
#if D3D12_SUPPORTED
#if defined(XBOX)
	DmaCmd                       mDma;
#endif
    ID3D12GraphicsCommandList *pDxCmdList;

    // Cached in beginCmd to avoid fetching them during rendering
    struct DescriptorHeap *     pBoundHeaps[2];
    D3D12_GPU_DESCRIPTOR_HANDLE mBoundHeapStartHandles[2];

    // Command buffer state
    const ID3D12RootSignature *pBoundRootSignature;
    DescriptorSet *            pBoundDescriptorSets[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint16_t                   mBoundDescriptorSetIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
    uint32_t mNodeIndex : 4;
    uint32_t mType : 3;
    CmdPool *pCmdPool;
    uint32_t mPadA;
#if !defined(XBOX)
    uint64_t mPadB;
#endif
#endif
#if defined(VULKAN)
	VkCommandBuffer              pVkCmdBuf;
	VkRenderPass                 pVkActiveRenderPass;
	VkPipelineLayout             pBoundPipelineLayout;
	uint32_t                     mNodeIndex : 4;
	uint32_t                     mType : 3;
	uint32_t                     mPadA;
	CmdPool*                     pCmdPool;
	uint64_t                     mPadB[9];
#endif
    Renderer *pRenderer;
    Queue *   pQueue;
}             Cmd;

COMPILE_ASSERT(sizeof(Cmd) <= 64 * sizeof(uint64_t));

typedef enum FenceStatus
{
    FENCE_STATUS_COMPLETE = 0,
    FENCE_STATUS_INCOMPLETE,
    FENCE_STATUS_NOTSUBMITTED,
} FenceStatus;

typedef struct Fence
{
#if D3D12_SUPPORTED
    ID3D12Fence *pDxFence;
    HANDLE       pDxWaitIdleFenceEvent;
    uint64_t     mFenceValue;
    uint64_t     mPadA;
#endif
#if defined(VULKAN)
	VkFence              pVkFence;
	uint32_t             mSubmitted : 1;
	uint32_t             mPadA;
	uint64_t             mPadB;
	uint64_t             mPadC;
#endif
} Fence;

typedef struct Semaphore
{
#if D3D12_SUPPORTED
    // DirectX12 does not have a concept of semaphores
    // All synchronization is done using fences
    // Simlate semaphore signal and wait using DirectX12 fences

    // Semaphores used in DirectX12 only in queueSubmit
    // queueSubmit -> How the semaphores work in DirectX12

    // pp_wait_semaphores -> queue->Wait is manually called on each fence in this
    // array before calling ExecuteCommandLists to make the fence work like a wait semaphore

    // pp_signal_semaphores -> Manually call queue->Signal on each fence in this array after
    // calling ExecuteCommandLists and increment the underlying fence value

    // queuePresent does not use the wait semaphore since the swapchain Present function
    // already does the synchronization in this case
    ID3D12Fence *pDxFence;
    HANDLE       pDxWaitIdleFenceEvent;
    uint64_t     mFenceValue;
    uint64_t     mPadA;
#endif
#if defined(VULKAN)
	VkSemaphore          pVkSemaphore;
	uint32_t             mCurrentNodeIndex : 5;
	uint32_t             mSignaled : 1;
	uint32_t             mPadA;
	uint64_t             mPadB;
	uint64_t             mPadC;
#endif
} Semaphore;

typedef struct QueueDesc
{
    QueueType     mType;
    QueueFlag     mFlag;
    QueuePriority mPriority;
    uint32_t      mNodeIndex;
}                 QueueDesc;

typedef struct Queue
{
#if D3D12_SUPPORTED
    ID3D12CommandQueue *pDxQueue;
    uint32_t mType : 3;
    uint32_t mNodeIndex : 4;
    Fence *pFence;
#endif
#if defined(VULKAN)
	VkQueue              pVkQueue;
	Mutex*               pSubmitMutex;
	uint32_t             mVkQueueFamilyIndex : 5;
	uint32_t             mVkQueueIndex : 5;
	uint32_t             mType : 3;
	uint32_t             mNodeIndex : 4;
	uint32_t             mGpuMode : 3;
	uint32_t             mFlags;
	float                mTimestampPeriod;
#endif
} Queue;

typedef struct ShaderMacro
{
    const char *definition;
    const char *value;
}               ShaderMacro;

#if defined(TARGET_IOS)
typedef struct ShaderStageDesc
{
	const char*  pName;
	const char*  pCode;
	const char*  pEntryPoint;
	ShaderMacro* pMacros;
	uint32_t     mMacroCount;
} ShaderStageDesc;

typedef struct ShaderDesc
{
	ShaderStage     mStages;
	ShaderStageDesc mVert;
	ShaderStageDesc mFrag;
	ShaderStageDesc mGeom;
	ShaderStageDesc mHull;
	ShaderStageDesc mDomain;
	ShaderStageDesc mComp;
} ShaderDesc;
#endif

typedef struct BinaryShaderStageDesc
{
    /// Byte code array
    void *      pByteCode;
    uint32_t    mByteCodeSize;
    const char *pEntryPoint;
}               BinaryShaderStageDesc;

typedef struct BinaryShaderDesc
{
    ShaderStage mStages;
    /// Specify whether shader will own byte code memory
    uint32_t mOwnByteCode : 1;
    BinaryShaderStageDesc mVert;
    BinaryShaderStageDesc mFrag;
    BinaryShaderStageDesc mGeom;
    BinaryShaderStageDesc mHull;
    BinaryShaderStageDesc mDomain;
    BinaryShaderStageDesc mComp;
}                         BinaryShaderDesc;

typedef struct Shader
{
    ShaderStage mStages;
    uint32_t    mNumThreadsPerGroup[3];
#if D3D12_SUPPORTED
    // TODO add Shader Encoding here
    //IDxcBlobEncoding**            pShaderBlobs;
    LPCWSTR *pEntryNames;
#endif
#if defined(VULKAN)
	VkShaderModule*               pShaderModules;
	char**                        pEntryNames;
#endif
    PipelineReflection *pReflection;
}                       Shader;

typedef struct BlendStateDesc
{
    /// Source blend factor per render target.
    BlendConstant mSrcFactors[MAX_RENDER_TARGET_ATTACHMENTS];
    /// Destination blend factor per render target.
    BlendConstant mDstFactors[MAX_RENDER_TARGET_ATTACHMENTS];
    /// Source alpha blend factor per render target.
    BlendConstant mSrcAlphaFactors[MAX_RENDER_TARGET_ATTACHMENTS];
    /// Destination alpha blend factor per render target.
    BlendConstant mDstAlphaFactors[MAX_RENDER_TARGET_ATTACHMENTS];
    /// Blend mode per render target.
    BlendMode mBlendModes[MAX_RENDER_TARGET_ATTACHMENTS];
    /// Alpha blend mode per render target.
    BlendMode mBlendAlphaModes[MAX_RENDER_TARGET_ATTACHMENTS];
    /// Write mask per render target.
    int32_t mMasks[MAX_RENDER_TARGET_ATTACHMENTS];
    /// Mask that identifies the render targets affected by the blend state.
    BlendStateTargets mRenderTargetMask;
    /// Set whether alpha to coverage should be enabled.
    bool mAlphaToCoverage;
    /// Set whether each render target has an unique blend function. When false the blend function in slot 0 will be used for all render targets.
    bool mIndependentBlend;
}        BlendStateDesc;

typedef struct DepthStateDesc
{
    bool        mDepthTest;
    bool        mDepthWrite;
    CompareMode mDepthFunc;
    bool        mStencilTest;
    uint8_t     mStencilReadMask;
    uint8_t     mStencilWriteMask;
    CompareMode mStencilFrontFunc;
    StencilOp   mStencilFrontFail;
    StencilOp   mDepthFrontFail;
    StencilOp   mStencilFrontPass;
    CompareMode mStencilBackFunc;
    StencilOp   mStencilBackFail;
    StencilOp   mDepthBackFail;
    StencilOp   mStencilBackPass;
}               DepthStateDesc;

typedef struct RasterizerStateDesc
{
    CullMode  mCullMode;
    int32_t   mDepthBias;
    float     mSlopeScaledDepthBias;
    FillMode  mFillMode;
    bool      mMultiSample;
    bool      mScissor;
    FrontFace mFrontFace;
    bool      mDepthClampEnable;
}             RasterizerStateDesc;

typedef enum VertexAttribRate
{
    VERTEX_ATTRIB_RATE_VERTEX = 0,
    VERTEX_ATTRIB_RATE_INSTANCE = 1,
    VERTEX_ATTRIB_RATE_COUNT,
} VertexAttribRate;

typedef struct VertexAttrib
{
    ShaderSemantic   mSemantic;
    uint32_t         mSemanticNameLength;
    char             mSemanticName[MAX_SEMANTIC_NAME_LENGTH];
    TextureFormat    mFormat;
    uint32_t         mBinding;
    uint32_t         mLocation;
    uint32_t         mOffset;
    VertexAttribRate mRate;
}                    VertexAttrib;

typedef struct VertexLayout
{
    uint32_t     mAttribCount;
    VertexAttrib mAttribs[MAX_VERTEX_ATTRIBS];
}                VertexLayout;

/************************************************************************/
// #pGlobalRootSignature - Root Signature used by all shaders in the ppShaders array
// #ppShaders - Array of all shaders which can be called during the raytracing operation
//	  This includes the ray generation shader, all miss, any hit, closest hit shaders
// #pHitGroups - Name of the hit groups which will tell the pipeline about which combination of hit shaders to use
// #mPayloadSize - Size of the payload struct for passing data to and from the shaders.
//	  Example - float4 payload sent by raygen shader which will be filled by miss shader as a skybox color
//				  or by hit shader as shaded color
// #mAttributeSize - Size of the intersection attribute. As long as user uses the default intersection shader
//	  this size is sizeof(float2) which represents the ZW of the barycentric co-ordinates of the intersection
/************************************************************************/
typedef struct RaytracingPipelineDesc
{
    Raytracing *        pRaytracing;
    RootSignature *     pGlobalRootSignature;
    Shader *            pRayGenShader;
    RootSignature *     pRayGenRootSignature;
    Shader **           ppMissShaders;
    RootSignature **    ppMissRootSignatures;
    RaytracingHitGroup *pHitGroups;
    RootSignature *     pEmptyRootSignature;
    unsigned            mMissShaderCount;
    unsigned            mHitGroupCount;
    // #TODO : Remove this after adding shader reflection for raytracing shaders
    unsigned mPayloadSize;
    // #TODO : Remove this after adding shader reflection for raytracing shaders
    unsigned mAttributeSize;
    unsigned mMaxTraceRecursionDepth;
    unsigned mMaxRaysCount;
}            RaytracingPipelineDesc;


typedef struct GraphicsPipelineDesc
{
    Shader *             pShaderProgram;
    RootSignature *      pRootSignature;
    VertexLayout *       pVertexLayout;
    BlendStateDesc *     pBlendState;
    DepthStateDesc *     pDepthState;
    RasterizerStateDesc *pRasterizerState;
    TextureFormat *      pColorFormats;
    uint32_t             mRenderTargetCount;
    SampleCount          mSampleCount;
    uint32_t             mSampleQuality;
    TextureFormat        mDepthStencilFormat;
    PrimitiveTopology    mPrimitiveTopo;
    bool                 mSupportIndirectCommandBuffer;
}                        GraphicsPipelineDesc;

typedef struct ComputePipelineDesc
{
    Shader *       pShaderProgram;
    RootSignature *pRootSignature;
}                  ComputePipelineDesc;

typedef struct PipelineDesc
{
    PipelineType mType;

    union
    {
        ComputePipelineDesc    mComputeDesc;
        GraphicsPipelineDesc   mGraphicsDesc;
        RaytracingPipelineDesc mRaytracingDesc;
    };

    PipelineCache *pCache;
    void *         pPipelineExtensions;
    uint32_t       mExtensionCount;
    const char *   pName;
}                  PipelineDesc;


typedef struct DEFINE_ALIGNED(Pipeline, 64)
{
#if D3D12_SUPPORTED
    ID3D12PipelineState *pDxPipelineState;
#ifdef ENABLE_RAYTRACING
    ID3D12StateObject *pDxrPipeline;
#endif
    ID3D12RootSignature *  pRootSignature;
    PipelineType           mType;
    D3D_PRIMITIVE_TOPOLOGY mDxPrimitiveTopology;
    uint64_t               mPadB[3];
#endif
#if defined(VULKAN)
	VkPipeline                  pVkPipeline;
	PipelineType                mType;
	uint32_t                    mShaderStageCount;
	//In DX12 this information is stored in ID3D12StateObject.
	//But for Vulkan we need to store it manually
	const char**                ppShaderStageNames;
	uint64_t                    mPadB[4];
#endif
} Pipeline;

// One cache line
COMPILE_ASSERT(sizeof(Pipeline) == 8 * sizeof(uint64_t));

typedef enum PipelineCacheFlags
{
    PIPELINE_CACHE_FLAG_NONE = 0x0,
    PIPELINE_CACHE_FLAG_EXTERNALLY_SYNCHRONIZED = 0x1,
} PipelineCacheFlags;

MAKE_ENUM_FLAG(uint32_t, PipelineCacheFlags);

typedef struct PipelineCacheDesc
{
    /// Initial pipeline cache data (can be NULL which means empty pipeline cache)
    void *pData;
    /// Initial pipeline cache size
    size_t             mSize;
    PipelineCacheFlags mFlags;
}                      PipelineCacheDesc;

typedef struct PipelineCache
{
#if D3D12_SUPPORTED
    ID3D12PipelineLibrary *pLibrary;
    void *                 pData;
#endif
#if defined(VULKAN)
	VkPipelineCache        pCache;
#endif
} PipelineCache;

typedef struct SwapChainDesc
{
    /// Window handle
    WindowHandle mWindowHandle;
    /// Queues which should be allowed to present
    Queue **ppPresentQueues;
    /// Number of present queues
    uint32_t mPresentQueueCount;
    /// Number of backbuffers in this swapchain
    uint32_t mImageCount;
    /// Width of the swapchain
    uint32_t mWidth;
    /// Height of the swapchain
    uint32_t mHeight;
    /// Color format of the swapchain
    TextureFormat mColorFormat;
    /// Clear value
    ClearValue mColorClearValue;
    /// Set whether swap chain will be presented using vsync
    bool mEnableVsync;
    /// We can toggle to using FLIP model if app desires.
    bool mUseFlipSwapEffect;
}        SwapChainDesc;

typedef struct SwapChain
{
    /// Render targets created from the swapchain back buffers
    RenderTarget **ppRenderTargets;
#if defined(XBOX)
	uint64_t                 mFramePipelineToken;
	/// Sync interval to specify how interval for vsync
	uint32_t                 mDxSyncInterval : 3;
	uint32_t                 mFlags : 10;
	uint32_t                 mImageCount : 3;
	uint32_t                 mEnableVsync : 1;
	uint32_t                 mIndex;
	void*                    pWindow;
	Queue*                   pPresentQueue;
	uint64_t                 mPadB[5];
#elif D3D12_SUPPORTED
    /// Use IDXGISwapChain3 for now since IDXGISwapChain4
    /// isn't supported by older devices.
    IDXGISwapChain3 *pDxSwapChain;
    /// Sync interval to specify how interval for vsync
    uint32_t mDxSyncInterval : 3;
    uint32_t mFlags : 10;
    uint32_t mImageCount : 3;
    uint32_t mEnableVsync : 1;
    uint32_t mPadA;
    uint64_t mPadB[5];
#endif
#if defined(VULKAN)
	/// Present queue if one exists (queuePresent will use this queue if the hardware has a dedicated present queue)
	VkQueue                  pPresentQueue;
	VkSwapchainKHR           pSwapChain;
	VkSurfaceKHR             pVkSurface;
	SwapChainDesc*           pDesc;
	uint32_t                 mPresentQueueFamilyIndex : 5;
	uint32_t                 mImageCount : 3;
	uint32_t                 mEnableVsync : 1;
	uint32_t                 mPadA;
#endif
} SwapChain;

typedef enum ShaderTarget
{
    // 5.1 is supported on all DX12 hardware
    shader_target_5_1,
    shader_target_6_0,
    shader_target_6_1,
    shader_target_6_2,
    shader_target_6_3,
    //required for Raytracing
} ShaderTarget;

typedef enum GpuMode
{
    GPU_MODE_SINGLE = 0,
    GPU_MODE_LINKED,
    // #TODO GPU_MODE_UNLINKED,
} GpuMode;

typedef struct RendererDesc
{
    RendererApi  mApi;
    ShaderTarget mShaderTarget;
    GpuMode      mGpuMode;
#if defined(VULKAN)
	const char**                 ppInstanceLayers;
	const char**                 ppInstanceExtensions;
	const char**                 ppDeviceExtensions;
	uint32_t                     mInstanceLayerCount;
	uint32_t                     mInstanceExtensionCount;
	uint32_t                     mDeviceExtensionCount;
	/// Flag to specify whether to request all queues from the gpu or just one of each type
	/// This will affect memory usage - Around 200 MB more used if all queues are requested
	bool                         mRequestAllAvailableQueues;
#endif
#if D3D12_SUPPORTED
    D3D_FEATURE_LEVEL mDxFeatureLevel;
#endif
    /// This results in new validation not possible during API calls on the CPU, by creating patched shaders that have validation added directly to the shader.
    /// However, it can slow things down a lot, especially for applications with numerous PSOs. Time to see the first render frame may take several minutes
    bool mEnableGPUBasedValidation;
}        RendererDesc;

typedef struct GPUVendorPreset
{
    char mVendorId[MAX_GPU_VENDOR_STRING_LENGTH];
    char mModelId[MAX_GPU_VENDOR_STRING_LENGTH];
    char mRevisionId[MAX_GPU_VENDOR_STRING_LENGTH];
    // OPtional as not all gpu's have that. Default is : 0x00
    GPUPresetLevel mPresetLevel;
    char           mGpuName[MAX_GPU_VENDOR_STRING_LENGTH];
    //If GPU Name is missing then value will be empty string
    char mGpuDriverVersion[MAX_GPU_VENDOR_STRING_LENGTH];
    char mGpuDriverDate[MAX_GPU_VENDOR_STRING_LENGTH];
}        GPUVendorPreset;

typedef struct GPUCapBits
{
    bool canShaderReadFrom[TextureFormat::Count];
    bool canShaderWriteTo[TextureFormat::Count];
    bool canRenderTargetWriteTo[TextureFormat::Count];
}        GPUCapBits;

typedef enum DefaultResourceAlignment
{
    RESOURCE_BUFFER_ALIGNMENT = 4U,
} DefaultResourceAlignment;

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

MAKE_ENUM_FLAG(uint32_t, WaveOpsSupportFlags);

typedef struct GPUSettings
{
    uint32_t            mUniformBufferAlignment;
    uint32_t            mUploadBufferTextureAlignment;
    uint32_t            mUploadBufferTextureRowAlignment;
    uint32_t            mMaxVertexInputBindings;
    uint32_t            mMaxRootSignatureDWORDS;
    uint32_t            mWaveLaneCount;
    WaveOpsSupportFlags mWaveOpsSupportFlags;
    GPUVendorPreset     mGpuVendorPreset;

    uint32_t mMultiDrawIndirect : 1;
    uint32_t mROVsSupported : 1;
    uint32_t mTessellationSupported : 1;
    uint32_t mGeometryShaderSupported : 1;
} GPUSettings;

typedef struct DEFINE_ALIGNED(Renderer, 64)
{
#if D3D12_SUPPORTED
    // API specific descriptor heap and memory allocator
    struct DescriptorHeap **  pCPUDescriptorHeaps;
    struct DescriptorHeap **  pCbvSrvUavHeaps;
    struct DescriptorHeap **  pSamplerHeaps;
    class D3D12MA::Allocator *pResourceAllocator;
#if defined(XBOX)
	IDXGIFactory2*                  pDXGIFactory;
	IDXGIAdapter*                   pDxActiveGPU;
	ID3D12Device*                   pDxDevice;
	EsramManager*                   pESRAMManager;
#elif D3D12_SUPPORTED
    IDXGIFactory6 *pDXGIFactory;
    IDXGIAdapter4 *pDxActiveGPU;
    ID3D12Device * pDxDevice;
#if defined(DRED)
	ID3D12DeviceRemovedExtendedDataSettings* pDredSettings;
#else
    uint64_t mPadA;
#endif
#endif
    ID3D12Debug *    pDXDebug;
    ID3D12InfoQueue *pDxDebugValidation;
#endif
#if defined(VULKAN)
	VkInstance                      pVkInstance;
	VkPhysicalDevice                pVkActiveGPU;
	VkPhysicalDeviceProperties2*    pVkActiveGPUProperties;
	VkDevice                        pVkDevice;
#ifdef USE_DEBUG_UTILS_EXTENSION
	VkDebugUtilsMessengerEXT        pVkDebugUtilsMessenger;
#else
	VkDebugReportCallbackEXT        pVkDebugReport;
#endif
	uint32_t**                      pAvailableQueueCount;
	uint32_t**                      pUsedQueueCount;
	struct DescriptorPool*          pDescriptorPool;
	struct VmaAllocator_T*          pVmaAllocator;
	uint32_t                        mRaytracingExtension : 1;
	union
	{
		struct
		{
			uint8_t                 mGraphicsQueueFamilyIndex;
			uint8_t                 mTransferQueueFamilyIndex;
			uint8_t                 mComputeQueueFamilyIndex;
		};
		uint8_t                     mQueueFamilyIndices[3];
	};
#endif
#if defined(USE_NSIGHT_AFTERMATH)
	// GPU crash dump tracker using Nsight Aftermath instrumentation
	AftermathTracker                mAftermathTracker;
	bool                            mAftermathSupport;
	bool                            mDiagnosticsConfigSupport;
	bool                            mDiagnosticCheckPointsSupport;
#endif
    struct NullDescriptors *pNullDescriptors;
    char *                  pName;
    GPUSettings *           pActiveGpuSettings;
    ShaderMacro *           pBuiltinShaderDefines;
    GPUCapBits *            pCapBits;
    uint32_t mLinkedNodeCount : 4;
    uint32_t mGpuMode : 3;
    uint32_t mShaderTarget : 4;
    uint32_t mApi : 5;
    uint32_t mEnableGpuBasedValidation : 1;
    uint32_t mBuiltinShaderDefinesCount;
}            Renderer;

// 3 cache lines
COMPILE_ASSERT(sizeof(Renderer) <= 24 * sizeof(uint64_t));

// Indirect command sturcture define
typedef struct IndirectArgumentDescriptor
{
    IndirectArgumentType mType;
    const char *         pName;
    uint32_t             mIndex;
}                        IndirectArgumentDescriptor;

typedef struct CommandSignatureDesc
{
    RootSignature *             pRootSignature;
    uint32_t                    mIndirectArgCount;
    IndirectArgumentDescriptor *pArgDescs;
    /// Set to true if indirect argument struct should not be aligned to 16 bytes
    bool mPacked;
}        CommandSignatureDesc;

typedef struct CommandSignature
{
#if D3D12_SUPPORTED
    ID3D12CommandSignature *pDxHandle;
#endif
#if defined(VULKAN)
	IndirectArgumentType    mDrawType;
	uint32_t                mStride;
#endif
} CommandSignature;

typedef struct DescriptorSetDesc
{
    RootSignature *           pRootSignature;
    DescriptorUpdateFrequency mUpdateFrequency;
    uint32_t                  mMaxSets;
    uint32_t                  mNodeIndex;
}                             DescriptorSetDesc;

typedef struct QueueSubmitDesc
{
    uint32_t    mCmdCount;
    Cmd **      ppCmds;
    Fence *     pSignalFence;
    uint32_t    mWaitSemaphoreCount;
    Semaphore **ppWaitSemaphores;
    uint32_t    mSignalSemaphoreCount;
    Semaphore **ppSignalSemaphores;
    bool        mSubmitDone;
}               QueueSubmitDesc;

typedef struct QueuePresentDesc
{
    SwapChain * pSwapChain;
    uint32_t    mWaitSemaphoreCount;
    Semaphore **ppWaitSemaphores;
    uint8_t     mIndex;
    bool        mSubmitDone;
}               QueuePresentDesc;

#define API_INTERFACE

// clang-format off
// API functions
// allocates memory and initializes the renderer -> returns pRenderer
//
API_INTERFACE void HAWL_CALLCONV initRenderer(const char *        app_name,
                                              const RendererDesc *p_settings,
                                              Renderer **         pRenderer);
API_INTERFACE void HAWL_CALLCONV removeRenderer(Renderer *pRenderer);

API_INTERFACE void HAWL_CALLCONV addFence(Renderer *pRenderer, Fence **p_fence);
API_INTERFACE void HAWL_CALLCONV removeFence(Renderer *pRenderer, Fence *p_fence);

API_INTERFACE void HAWL_CALLCONV addSemaphore(Renderer *pRenderer, Semaphore **p_semaphore);
API_INTERFACE void HAWL_CALLCONV removeSemaphore(Renderer *pRenderer, Semaphore *p_semaphore);

API_INTERFACE void HAWL_CALLCONV addQueue(Renderer *pRenderer, QueueDesc *pQDesc, Queue **pQueue);
API_INTERFACE void HAWL_CALLCONV removeQueue(Renderer *pRenderer, Queue *pQueue);

API_INTERFACE void HAWL_CALLCONV addSwapChain(Renderer *           pRenderer,
                                              const SwapChainDesc *p_desc,
                                              SwapChain **         p_swap_chain);
API_INTERFACE void HAWL_CALLCONV removeSwapChain(Renderer *pRenderer, SwapChain *p_swap_chain);

// command pool functions
API_INTERFACE void HAWL_CALLCONV addCmdPool(Renderer *         pRenderer,
                                            const CmdPoolDesc *p_desc,
                                            CmdPool **         p_cmd_pool);
API_INTERFACE void HAWL_CALLCONV removeCmdPool(Renderer *pRenderer, CmdPool *p_CmdPool);
API_INTERFACE void HAWL_CALLCONV addCmd(Renderer *pRenderer, const CmdDesc *p_desc, Cmd **p_cmd);
API_INTERFACE void HAWL_CALLCONV removeCmd(Renderer *pRenderer, Cmd *pCmd);
API_INTERFACE void HAWL_CALLCONV addCmd_n(Renderer *     pRenderer,
                                          const CmdDesc *p_desc,
                                          uint32_t       cmd_count,
                                          Cmd ***        p_cmds);
API_INTERFACE void HAWL_CALLCONV removeCmd_n(Renderer *pRenderer, uint32_t cmd_count, Cmd **p_cmds);

//
// All buffer, texture loading handled by resource system -> IResourceLoader.*
//

API_INTERFACE void HAWL_CALLCONV addRenderTarget(Renderer *              pRenderer,
                                                 const RenderTargetDesc *pDesc,
                                                 RenderTarget **         ppRenderTarget);
API_INTERFACE void HAWL_CALLCONV removeRenderTarget(Renderer *    pRenderer,
                                                    RenderTarget *pRenderTarget);
API_INTERFACE void HAWL_CALLCONV addSampler(Renderer *         pRenderer,
                                            const SamplerDesc *pDesc,
                                            Sampler **         p_sampler);
API_INTERFACE void HAWL_CALLCONV removeSampler(Renderer *pRenderer, Sampler *p_sampler);

// shader functions
#if defined(TARGET_IOS)
API_INTERFACE void HAWL_CALLCONV addShader(Renderer* pRenderer, const ShaderDesc* p_desc, Shader** p_shader_program);
#endif
API_INTERFACE void HAWL_CALLCONV addShaderBinary(Renderer *              pRenderer,
                                                 const BinaryShaderDesc *p_desc,
                                                 Shader **               p_shader_program);
API_INTERFACE void HAWL_CALLCONV removeShader(Renderer *pRenderer, Shader *p_shader_program);

API_INTERFACE void HAWL_CALLCONV addRootSignature(Renderer *               pRenderer,
                                                  const RootSignatureDesc *pDesc,
                                                  RootSignature **         pRootSignature);
API_INTERFACE void HAWL_CALLCONV removeRootSignature(Renderer *     pRenderer,
                                                     RootSignature *pRootSignature);

// pipeline functions
API_INTERFACE void HAWL_CALLCONV addPipeline(Renderer *          pRenderer,
                                             const PipelineDesc *p_pipeline_settings,
                                             Pipeline **         p_pipeline);
API_INTERFACE void HAWL_CALLCONV removePipeline(Renderer *pRenderer, Pipeline *p_pipeline);
API_INTERFACE void HAWL_CALLCONV addPipelineCache(Renderer *               pRenderer,
                                                  const PipelineCacheDesc *pDesc,
                                                  PipelineCache **         ppPipelineCache);
API_INTERFACE void HAWL_CALLCONV getPipelineCacheData(Renderer *     pRenderer,
                                                      PipelineCache *pPipelineCache,
                                                      size_t *       pSize,
                                                      void *         pData);
API_INTERFACE void HAWL_CALLCONV removePipelineCache(Renderer *     pRenderer,
                                                     PipelineCache *pPipelineCache);

// Descriptor Set functions
API_INTERFACE void HAWL_CALLCONV addDescriptorSet(Renderer *               pRenderer,
                                                  const DescriptorSetDesc *pDesc,
                                                  DescriptorSet **         pDescriptorSet);
API_INTERFACE void HAWL_CALLCONV removeDescriptorSet(Renderer *     pRenderer,
                                                     DescriptorSet *pDescriptorSet);
API_INTERFACE void HAWL_CALLCONV updateDescriptorSet(Renderer *            pRenderer,
                                                     uint32_t              index,
                                                     DescriptorSet *       pDescriptorSet,
                                                     uint32_t              count,
                                                     const DescriptorData *pParams);

// command buffer functions
API_INTERFACE void HAWL_CALLCONV resetCmdPool(Renderer *pRenderer, CmdPool *pCmdPool);
API_INTERFACE void HAWL_CALLCONV beginCmd(Cmd *p_cmd);
API_INTERFACE void HAWL_CALLCONV endCmd(Cmd *p_cmd);
API_INTERFACE void HAWL_CALLCONV cmdBindRenderTargets(Cmd *                  p_cmd,
                                                      uint32_t               render_target_count,
                                                      RenderTarget **        p_render_targets,
                                                      RenderTarget *         p_depth_stencil,
                                                      const LoadActionsDesc *loadActions,
                                                      uint32_t *             pColorArraySlices,
                                                      uint32_t *             pColorMipSlices,
                                                      uint32_t               depthArraySlice,
                                                      uint32_t               depthMipSlice);
API_INTERFACE void HAWL_CALLCONV cmdSetViewport(Cmd * p_cmd,
                                                float x,
                                                float y,
                                                float width,
                                                float height,
                                                float min_depth,
                                                float max_depth);
API_INTERFACE void HAWL_CALLCONV cmdSetScissor(Cmd *    p_cmd,
                                               uint32_t x,
                                               uint32_t y,
                                               uint32_t width,
                                               uint32_t height);
API_INTERFACE void HAWL_CALLCONV cmdSetStencilReferenceValue(Cmd *p_cmd, uint32_t val);
API_INTERFACE void HAWL_CALLCONV cmdBindPipeline(Cmd *p_cmd, Pipeline *p_pipeline);
API_INTERFACE void HAWL_CALLCONV cmdBindDescriptorSet(Cmd *          pCmd,
                                                      uint32_t       index,
                                                      DescriptorSet *pDescriptorSet);
API_INTERFACE void HAWL_CALLCONV cmdBindPushConstants(Cmd *          pCmd,
                                                      RootSignature *pRootSignature,
                                                      const char *   pName,
                                                      const void *   pConstants);
API_INTERFACE void HAWL_CALLCONV cmdBindPushConstantsByIndex(
    Cmd *          pCmd,
    RootSignature *pRootSignature,
    uint32_t       paramIndex,
    const void *   pConstants);
API_INTERFACE void HAWL_CALLCONV cmdBindIndexBuffer(Cmd *    p_cmd,
                                                    Buffer * p_buffer,
                                                    uint32_t indexType,
                                                    uint64_t offset);
API_INTERFACE void HAWL_CALLCONV cmdBindVertexBuffer(Cmd *           p_cmd,
                                                     uint32_t        buffer_count,
                                                     Buffer **       pp_buffers,
                                                     const uint32_t *pStrides,
                                                     const uint64_t *pOffsets);
API_INTERFACE void HAWL_CALLCONV cmdDraw(Cmd *p_cmd, uint32_t vertex_count, uint32_t first_vertex);
API_INTERFACE void HAWL_CALLCONV cmdDrawInstanced(Cmd *    pCmd,
                                                  uint32_t vertexCount,
                                                  uint32_t firstVertex,
                                                  uint32_t instanceCount,
                                                  uint32_t firstInstance);
API_INTERFACE void HAWL_CALLCONV cmdDrawIndexed(Cmd *    p_cmd,
                                                uint32_t index_count,
                                                uint32_t first_index,
                                                uint32_t first_vertex);
API_INTERFACE void HAWL_CALLCONV cmdDrawIndexedInstanced(Cmd *    pCmd,
                                                         uint32_t indexCount,
                                                         uint32_t firstIndex,
                                                         uint32_t instanceCount,
                                                         uint32_t firstVertex,
                                                         uint32_t firstInstance);
API_INTERFACE void HAWL_CALLCONV cmdDispatch(Cmd *    p_cmd,
                                             uint32_t group_count_x,
                                             uint32_t group_count_y,
                                             uint32_t group_count_z);

// Transition Commands
API_INTERFACE void HAWL_CALLCONV cmdResourceBarrier(Cmd *                p_cmd,
                                                    uint32_t             buffer_barrier_count,
                                                    BufferBarrier *      p_buffer_barriers,
                                                    uint32_t             texture_barrier_count,
                                                    TextureBarrier *     p_texture_barriers,
                                                    uint32_t             rt_barrier_count,
                                                    RenderTargetBarrier *p_rt_barriers);

// Virtual Textures

API_INTERFACE void HAWL_CALLCONV cmdUpdateVirtualTexture(Cmd *pCmd, Texture *pTexture);

//
// All buffer, texture update handled by resource system -> IResourceLoader.*
//

// queue/fence/swapchain functions
API_INTERFACE void HAWL_CALLCONV acquireNextImage(Renderer *pRenderer,
                                                  SwapChain *p_swap_chain,
                                                  Semaphore *p_signal_semaphore,
                                                  Fence *p_fence,
                                                  uint32_t *p_image_index);
API_INTERFACE void HAWL_CALLCONV queueSubmit(Queue *p_queue, const QueueSubmitDesc *p_desc);
API_INTERFACE PresentStatus HAWL_CALLCONV queuePresent(Queue *p_queue,
                                                       const QueuePresentDesc *p_desc);
API_INTERFACE void HAWL_CALLCONV waitQueueIdle(Queue *p_queue);
API_INTERFACE void HAWL_CALLCONV getFenceStatus(Renderer *   pRenderer,
                                                Fence *      p_fence,
                                                FenceStatus *p_fence_status);
API_INTERFACE void HAWL_CALLCONV waitForFences(Renderer *pRenderer,
                                               uint32_t  fenceCount,
                                               Fence **  ppFences);
API_INTERFACE void HAWL_CALLCONV toggleVSync(Renderer *pRenderer, SwapChain **ppSwapchain);

//Returns the recommended format for the swapchain.
//If true is passed for the hintHDR parameter, it will return an HDR format IF the platform supports it
//If false is passed or the platform does not support HDR a non HDR format is returned.
API_INTERFACE TextureFormat HAWL_CALLCONV getRecommendedSwapchainFormat(bool hintHDR);

//indirect Draw functions
API_INTERFACE void HAWL_CALLCONV addIndirectCommandSignature(
    Renderer *                  pRenderer,
    const CommandSignatureDesc *p_desc,
    CommandSignature **         ppCommandSignature);
API_INTERFACE void HAWL_CALLCONV removeIndirectCommandSignature(
    Renderer *        pRenderer,
    CommandSignature *pCommandSignature);
API_INTERFACE void HAWL_CALLCONV cmdExecuteIndirect(Cmd *             pCmd,
                                                    CommandSignature *pCommandSignature,
                                                    uint              maxCommandCount,
                                                    Buffer *          pIndirectBuffer,
                                                    uint64_t          bufferOffset,
                                                    Buffer *          pCounterBuffer,
                                                    uint64_t          counterBufferOffset);
/************************************************************************/
// GPU Query Interface
/************************************************************************/
API_INTERFACE void HAWL_CALLCONV getTimestampFrequency(Queue *pQueue, double *pFrequency);
API_INTERFACE void HAWL_CALLCONV addQueryPool(Renderer *           pRenderer,
                                              const QueryPoolDesc *pDesc,
                                              QueryPool **         ppQueryPool);
API_INTERFACE void HAWL_CALLCONV removeQueryPool(Renderer *pRenderer, QueryPool *pQueryPool);
API_INTERFACE void HAWL_CALLCONV cmdResetQueryPool(Cmd *      pCmd,
                                                   QueryPool *pQueryPool,
                                                   uint32_t   startQuery,
                                                   uint32_t   queryCount);
API_INTERFACE void HAWL_CALLCONV cmdBeginQuery(Cmd *pCmd, QueryPool *pQueryPool, QueryDesc *pQuery);
API_INTERFACE void HAWL_CALLCONV cmdEndQuery(Cmd *pCmd, QueryPool *pQueryPool, QueryDesc *pQuery);
API_INTERFACE void HAWL_CALLCONV cmdResolveQuery(Cmd *      pCmd,
                                                 QueryPool *pQueryPool,
                                                 Buffer *   pReadbackBuffer,
                                                 uint32_t   startQuery,
                                                 uint32_t   queryCount);
/************************************************************************/
// Stats Info Interface
/************************************************************************/
API_INTERFACE void HAWL_CALLCONV calculateMemoryStats(Renderer *pRenderer, char **stats);
API_INTERFACE void HAWL_CALLCONV calculateMemoryUse(Renderer *pRenderer,
                                                    uint64_t *usedBytes,
                                                    uint64_t *totalAllocatedBytes);
API_INTERFACE void HAWL_CALLCONV freeMemoryStats(Renderer *pRenderer, char *stats);
/************************************************************************/
// Debug Marker Interface
/************************************************************************/
API_INTERFACE void HAWL_CALLCONV cmdBeginDebugMarker(Cmd *       pCmd,
                                                     float       r,
                                                     float       g,
                                                     float       b,
                                                     const char *pName);
API_INTERFACE void HAWL_CALLCONV cmdEndDebugMarker(Cmd *pCmd);
API_INTERFACE void HAWL_CALLCONV cmdAddDebugMarker(Cmd *       pCmd,
                                                   float       r,
                                                   float       g,
                                                   float       b,
                                                   const char *pName);
#if D3D12_SUPPORTED
API_INTERFACE uint32_t HAWL_CALLCONV cmdWriteMarker(Cmd *      pCmd,
                                                    MarkerType markerType,
                                                    uint32_t   markerValue,
                                                    Buffer *   pBuffer,
                                                    size_t     offset,
                                                    bool       useAutoFlags = false);
#endif
/************************************************************************/
// Resource Debug Naming Interface
/************************************************************************/
API_INTERFACE void HAWL_CALLCONV setBufferName(Renderer *  pRenderer,
                                               Buffer *    pBuffer,
                                               const char *pName);
API_INTERFACE void HAWL_CALLCONV setTextureName(Renderer *  pRenderer,
                                                Texture *   pTexture,
                                                const char *pName);
API_INTERFACE void HAWL_CALLCONV setRenderTargetName(Renderer *    pRenderer,
                                                     RenderTarget *pRenderTarget,
                                                     const char *  pName);
API_INTERFACE void HAWL_CALLCONV setPipelineName(Renderer *  pRenderer,
                                                 Pipeline *  pPipeline,
                                                 const char *pName);
/************************************************************************/
/************************************************************************/
// clang-format on
