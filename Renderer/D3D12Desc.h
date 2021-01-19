#pragma once
#include "BaseType.h"
#include "TextureFormat.h"

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
    MAX_GPU_VENDOR_STRING_LENGTH = 64,
    // max size for GPUVendorPreset strings
};
#endif

struct DescriptorHeap;
struct NullDescriptors;

// typedef struct and enum
typedef enum GPUPresetLevel      GPUPresetLevel;
typedef struct Fence             Fence;
typedef enum QueueType           QueueType;
typedef enum QueueFlag           QueueFlag;
typedef enum QueuePriority       QueuePriority;
typedef struct Queue             Queue;
typedef struct QueueDesc         QueueDesc;
typedef struct GPUVendorPreset   GPUVendorPreset;
typedef enum WaveOpsSupportFlags WaveOpsSupportFlags;
typedef struct GPUSettings       GPUSettings;
typedef struct ShaderMacro       ShaderMacro;
typedef struct GPUCapBits        GPUCapBits;
typedef enum ShaderTarget        ShaderTarget;
typedef enum GpuMode             GpuMode;
typedef struct RendererDesc      RendererDesc;
typedef struct GpuDesc           GpuDesc;

enum GPUPresetLevel
{
    GPU_PRESET_NONE = 0,
    GPU_PRESET_OFFICE,
    // This means unsupported
    GPU_PRESET_LOW,
    GPU_PRESET_MEDIUM,
    GPU_PRESET_HIGH,
    GPU_PRESET_ULTRA,
    GPU_PRESET_COUNT
};


struct Fence
{
    ID3D12Fence *pDxFence;
    HANDLE       pDxWaitIdleFenceEvent;
    uint64       mFenceValue;
    uint64       mPadA;
};

enum QueueType
{
    QUEUE_TYPE_GRAPHICS = 0,
    QUEUE_TYPE_TRANSFER,
    QUEUE_TYPE_COMPUTE,
    MAX_QUEUE_TYPE
};

enum QueueFlag
{
    QUEUE_FLAG_NONE = 0x0,
    QUEUE_FLAG_DISABLE_GPU_TIMEOUT = 0x1,
    QUEUE_FLAG_INIT_MICROPROFILE = 0x2,
    MAX_QUEUE_FLAG = 0xFFFFFFFF
};

MAKE_ENUM_FLAG(uint32, QueueFlag)

enum QueuePriority
{
    QUEUE_PRIORITY_NORMAL,
    QUEUE_PRIORITY_HIGH,
    QUEUE_PRIORITY_GLOBAL_REALTIME,
    MAX_QUEUE_PRIORITY
};

struct Queue
{
    ID3D12CommandQueue *pDxQueue;
    uint32 mType : 3;
    uint32 mNodeIndex : 4;
    Fence *pFence;
};

struct QueueDesc
{
    QueueType     mType;
    QueueFlag     mFlag;
    QueuePriority mPriority;
    uint32_t      mNodeIndex;
};

struct GPUVendorPreset
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
};

enum WaveOpsSupportFlags
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
};

MAKE_ENUM_FLAG(uint32, WaveOpsSupportFlags);

/**
 * GPU Settings struct
 */
struct GPUSettings
{
    uint32              mUniformBufferAlignment;
    uint32              mUploadBufferTextureAlignment;
    uint32              mUploadBufferTextureRowAlignment;
    uint32              mMaxVertexInputBindings;
    uint32              mMaxRootSignatureDWORDS;
    uint32              mWaveLaneCount;
    WaveOpsSupportFlags mWaveOpsSupportFlags;
    GPUVendorPreset     mGpuVendorPreset;
    uint32 mMultiDrawIndirect : 1;
    uint32 mROVsSupported : 1;
    uint32 mTessellationSupported : 1;
    uint32 mGeometryShaderSupported : 1;
};

struct ShaderMacro
{
    const char *definition;
    const char *value;
};

struct GPUCapBits
{ 
    bool canShaderReadFrom[TextureFormat::Count];
    bool canShaderWriteTo[TextureFormat::Count];
    bool canRenderTargetWriteTo[TextureFormat::Count];
};


enum ShaderTarget
{
    shader_target_5_1,
    shader_target_6_0,
    shader_target_6_1,
    shader_target_6_2,
    shader_target_6_3,
    //required for Raytracing
};

enum GpuMode
{
    GPU_MODE_SINGLE = 0,
    GPU_MODE_LINKED,
};

struct RendererDesc
{
    ShaderTarget mShaderTarget;
    GpuMode      mGpuMode;
    /// This results in new validation not possible during API calls on the CPU, by creating patched shaders that have validation added directly to the shader.
    /// However, it can slow things down a lot, especially for applications with numerous PSOs. Time to see the first render frame may take several minutes
    bool mEnableGPUBasedValidation;
};

struct GpuDesc
{
    IDXGIAdapter4 *                   pGpu = nullptr;
    D3D_FEATURE_LEVEL                 mMaxSupportedFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
    D3D12_FEATURE_DATA_D3D12_OPTIONS  mFeatureDataOptions;
    D3D12_FEATURE_DATA_D3D12_OPTIONS1 mFeatureDataOptions1;
    SIZE_T                            mDedicatedVideoMemory = 0;
    uint                              mVendorId;
    uint                              mDeviceId;
    uint                              mRevisionId;
    char                              mName[MAX_GPU_VENDOR_STRING_LENGTH];
};
}
