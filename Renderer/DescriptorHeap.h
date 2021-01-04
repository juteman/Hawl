#pragma once
#include "BaseType.h"

#include <d3d12.h>
#include <EASTL/vector.h>
#include <tbb/atomic.h>
#include <tbb/mutex.h>
#include <EASTL/array.h>
#include "DX12Device.h"
namespace Hawl
{
    
struct DescriptorHeap
{

    struct DescriptorHandle
    {
        D3D12_CPU_DESCRIPTOR_HANDLE mCpu;
        D3D12_GPU_DESCRIPTOR_HANDLE mGpu;
    };

    /// DX Heap
    ID3D12DescriptorHeap *pCurrentHeap;
    /// Lock for multi-threaded descriptor allocations
    tbb::mutex *                 pMutex;
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
    tbb::atomic<uint32> mUsedDescriptors;
};

class DescriptorHeapsImpl
{


public:
    /**
     * \brief Init the all the descriptor heap
     * \param deviceImpl use ID3D12Device to create descriptor heap
     */
    DescriptorHeapsImpl(DeviceImpl& deviceImpl);

private:
    eastl::array<DescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE> mCPUDescriptorHeaps;
    DescriptorHeap mCbvSrvUavHeaps;
    DescriptorHeap mSamplerHeap;
};


}
