#include "DescriptorHeap.h"

#include "Common.h"
#include "DX12Helper.h"



#include <mimalloc.h>


namespace Hawl
{
struct DescriptorHeapProperties
{
    uint32                      mMaxDescriptors;
    D3D12_DESCRIPTOR_HEAP_FLAGS mFlags;
};


DescriptorHeapProperties gCpuDescriptorHeapProperties[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
{
    {1024 * 256, D3D12_DESCRIPTOR_HEAP_FLAG_NONE}, // CBV SRV UAV
    {2048, D3D12_DESCRIPTOR_HEAP_FLAG_NONE},       // Sampler
    {512, D3D12_DESCRIPTOR_HEAP_FLAG_NONE},        // RTV
    {512, D3D12_DESCRIPTOR_HEAP_FLAG_NONE},        // DSV
};


void AddDescriptorHeap(ID3D12Device *                    pDevice,
                       const D3D12_DESCRIPTOR_HEAP_DESC &desc,
                       DescriptorHeap                    descHeap)
{
    uint32 numDescriptors = desc.NumDescriptors;

    descHeap.pMutex = static_cast<tbb::mutex *>(mi_calloc(1, sizeof(tbb::mutex)));
    descHeap.pMutex->set_state(tbb::mutex::INITIALIZED);
    descHeap.pDevice = pDevice;

    // Keep 32 aligned for easy remove
    numDescriptors = RoundUp(numDescriptors, 32);
    D3D12_DESCRIPTOR_HEAP_DESC descAfterRoundUp;

    descAfterRoundUp.NumDescriptors = numDescriptors;
    descAfterRoundUp = desc;
    descHeap.mDesc = descAfterRoundUp;

    CHECK_DX12_RESULT(pDevice->CreateDescriptorHeap(&descAfterRoundUp, IID_PPV_ARGS(&descHeap.pCurrentHeap)));

    descHeap.mStartHandle.mCpu = descHeap.pCurrentHeap->GetCPUDescriptorHandleForHeapStart();
    descHeap.mStartHandle.mGpu = descHeap.pCurrentHeap->GetGPUDescriptorHandleForHeapStart();
    descHeap.mDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(descHeap.mDesc.Type);
    if (descAfterRoundUp.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE){
        descHeap.pHandles = static_cast<D3D12_CPU_DESCRIPTOR_HANDLE *>(mi_calloc(
            descAfterRoundUp.NumDescriptors,
            sizeof(D3D12_CPU_DESCRIPTOR_HANDLE)));
    }
}

DescriptorHeapsImpl::DescriptorHeapsImpl(DeviceImpl &deviceImpl)
{
    for (uint32 i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Flags = gCpuDescriptorHeapProperties[i].mFlags;
        desc.NodeMask = 0; // CPU Descriptor Heap - Node mask is irrelevant
        desc.NumDescriptors = gCpuDescriptorHeapProperties[i].mMaxDescriptors;
        desc.Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i);
        AddDescriptorHeap(deviceImpl.GetDevice(), desc, mCPUDescriptorHeaps[i]);
    }
}
}
