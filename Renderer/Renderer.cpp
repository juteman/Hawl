#include "GPUHelper.h"
#include "DX12Helper.h"
#include "mimalloc.h"
#include "nvapi.h"
#include <combaseapi.h>

namespace Hawl
{

void EnableDebugLayer(Renderer* pRenderer)
{
#if defined(GRAPHICS_DEBUG)
	pRenderer->GetDxDebug()->EnableDebugLayer();
	ID3D12Debug1* pDebug1 = nullptr;
	if (SUCCEEDED(pRenderer->GetDxDebug()->QueryInterface(IID_PPV_ARGS(&pDebug1))))
	{
		pDebug1->SetEnableGPUBasedValidation(pRenderer->IsEnableGpuBasedValidation());
		pDebug1->Release();
	}
#endif

#if DRED_ENABLE

#endif
}

bool addDevice(Renderer* pRenderer)
{

#ifdef GRAPHICS_DEBUG
    ID3D12Debug* d= pRenderer->GetDxDebug();
    if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(pRenderer->GetDxDebug()), reinterpret_cast<void **>(&(pRenderer->GetDxDebug())))))
	{
		EnableDebugLayer(pRenderer);
	}


#endif
    // TODO Here can add some debug  factory such as Nsight
    D3D_FEATURE_LEVEL featureLevels[4] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

    uint flags = 0;
#if GRAPHICS_DEBUG
	flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CHECK_DX12_RESULT(CreateDXGIFactory2(flags, IID_PPV_ARGS(&pRenderer->GetFactory6())))

    uint32 gpuCount = 0;
    IDXGIAdapter4* adapter4 = nullptr;
    bool foundSoftwareAdapter = false;

    // Find number of usable GPUs
	// Use DXGI6 interface which lets us specify gpu preference so we dont need to use NVOptimus or AMDPowerExpress exports
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != pRenderer->GetFactory6()->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&adapter4)); ++i)
	{
		DXGI_ADAPTER_DESC3 desc{};
		adapter4->GetDesc3(&desc);

		// Ignore Microsoft Driver
		if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
		{
			for (uint32_t level = 0; level < sizeof(featureLevels) / sizeof(featureLevels[0]); ++level)
			{
				// Make sure the adapter can support a D3D12 device
				if (SUCCEEDED(D3D12CreateDevice(adapter4, featureLevels[level], __uuidof(ID3D12Device), nullptr)))
				{
					GpuDesc gpuDesc = {};
                    // find adapter support D3D feature and adapter4 
					HRESULT hres = adapter4->QueryInterface(IID_PPV_ARGS(&gpuDesc.pGpu));
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

		adapter4->Release();
	}

    // If the only adapter we found is a software adapter, log error message for QA
	if (!gpuCount && foundSoftwareAdapter)
	{
		Logger::error("The only available GPU has DXGI_ADAPTER_FLAG_SOFTWARE. Early exiting");
		assert(false);
		return false;
	}

    // if not find gpu support dx12
    assert(gpuCount);
}


/************************************************************************/
// Renderer Init Remove
/************************************************************************/
Renderer::Renderer(const RendererDesc &desc)
{
    mGpuMode = desc.mGpuMode;
    mShaderTarget = desc.mShaderTarget;
    mEnableGpuBasedValidation = desc.mEnableGPUBasedValidation;
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

        if (addDevice(this))
        {
            Logger::error("add Device failed");
        }
        /************************************************************************/
        // Multi GPU - SLI Node Count
        /************************************************************************/
        uint32_t gpuCount = pDxDevice->GetNodeCount();
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
        (ShaderMacro *)mi_calloc(pRenderer->mBuiltinShaderDefinesCount, sizeof(ShaderMacro));
    for (uint32_t i = 0; i < pRenderer->mBuiltinShaderDefinesCount; ++i)
    {
        pRenderer->pBuiltinShaderDefines[i] = rendererShaderDefines[i];
    }

    // Renderer is good!
    *ppRenderer = pRenderer;
}


} // namespace Hawl
