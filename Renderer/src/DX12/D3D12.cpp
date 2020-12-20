#include <d3d12.h>
#include <tbb/spin_mutex.h>
#include <BaseType.h>
#include "EASTL/vector.h"
#include "IShaderReflection.h"

namespace Hawl
{
/************************************************************************/
// Descriptor Heap Structures
/************************************************************************/
/// CPU Visible Heap to store all the resources needing CPU read / write operations -
/// Textures/Buffers/RTV
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
    tbb::spin_mutex *            pMutex;
    ID3D12Device *               pDevice;
    D3D12_CPU_DESCRIPTOR_HANDLE *pHandles;
    /// Start position in the heap
    DescriptorHandle mStartHandle;
    /// Free List used for CPU only descriptor heaps
    eastl::vector<DescriptorHandle> mFreeList;
    /// Description
    D3D12_DESCRIPTOR_HEAP_DESC mDesc;
    /// DescriptorInfo Increment Size
    uint32 mDescriptorSize;
    /// Used
    /*tfrg_atomic32_t*/
    uint32 mUsedDescriptors;
} DescriptorHeap;


typedef struct NullDescriptors
{
    // Default NULL Descriptors for binding at empty descriptor slots to make sure all descriptors are bound at submit
    D3D12_CPU_DESCRIPTOR_HANDLE mNullTextureSRV[TEXTURE_DIM_COUNT];
    D3D12_CPU_DESCRIPTOR_HANDLE mNullTextureUAV[TEXTURE_DIM_COUNT];
    D3D12_CPU_DESCRIPTOR_HANDLE mNullBufferSRV;
    D3D12_CPU_DESCRIPTOR_HANDLE mNullBufferUAV;
    D3D12_CPU_DESCRIPTOR_HANDLE mNullBufferCBV;
    D3D12_CPU_DESCRIPTOR_HANDLE mNullSampler;
} NullDescriptors;
}