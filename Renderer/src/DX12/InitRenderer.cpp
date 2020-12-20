#include "IRenderer.h"

namespace Hawl
{
/************************************************************************/
// Renderer Init Remove
/************************************************************************/
void initRenderer(const char *appName, const RendererDesc *pDesc, Renderer **ppRenderer)
{

    Renderer *pRenderer = (Renderer *)tf_calloc_memalign(1, alignof(Renderer), sizeof(Renderer));
    ASSERT(pRenderer);

    pRenderer->mGpuMode = pDesc->mGpuMode;
    pRenderer->mShaderTarget = pDesc->mShaderTarget;
    pRenderer->mEnableGpuBasedValidation = pDesc->mEnableGPUBasedValidation;
    pRenderer->pName = (char *)tf_calloc(strlen(appName) + 1, sizeof(char));
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
        pRenderer->pCbvSrvUavHeaps =
            (DescriptorHeap **)tf_malloc(pRenderer->mLinkedNodeCount * sizeof(DescriptorHeap *));
        pRenderer->pSamplerHeaps =
            (DescriptorHeap **)tf_malloc(pRenderer->mLinkedNodeCount * sizeof(DescriptorHeap *));

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

        D3D12MA::ALLOCATION_CALLBACKS allocationCallbacks = {};
        allocationCallbacks.pAllocate = [](size_t size, size_t alignment, void *) {
            return tf_memalign(alignment, size);
        };
        allocationCallbacks.pFree = [](void *ptr, void *) { tf_free(ptr); };
        desc.pAllocationCallbacks = &allocationCallbacks;
        CHECK_HRESULT(D3D12MA::CreateAllocator(&desc, &pRenderer->pResourceAllocator));
    }
    /************************************************************************/
    /************************************************************************/
    add_default_resources(pRenderer);

    hook_post_init_renderer(pRenderer);

    // Set shader macro based on runtime information
    ShaderMacro rendererShaderDefines[] = {
        // Descriptor set indices
        {"UPDATE_FREQ_NONE", "space0"},
        {"UPDATE_FREQ_PER_FRAME", "space1"},
        {"UPDATE_FREQ_PER_BATCH", "space2"},
        {"UPDATE_FREQ_PER_DRAW", "space3"},
    };
    pRenderer->mBuiltinShaderDefinesCount =
        sizeof(rendererShaderDefines) / sizeof(rendererShaderDefines[0]);
    pRenderer->pBuiltinShaderDefines =
        (ShaderMacro *)tf_calloc(pRenderer->mBuiltinShaderDefinesCount, sizeof(ShaderMacro));
    for (uint32_t i = 0; i < pRenderer->mBuiltinShaderDefinesCount; ++i)
    {
        pRenderer->pBuiltinShaderDefines[i] = rendererShaderDefines[i];
    }

    // Renderer is good!
    *ppRenderer = pRenderer;
}
} // namespace Hawl