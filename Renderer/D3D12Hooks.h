#pragma once
#include "IRenderer.h"

typedef struct GpuDesc
{
	Renderer*                         pRenderer = nullptr;
#if defined(XBOX)
	IDXGIAdapter*                     pGpu = NULL;
#else
	IDXGIAdapter4*                    pGpu = nullptr;
#endif
	D3D_FEATURE_LEVEL                 mMaxSupportedFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
	D3D12_FEATURE_DATA_D3D12_OPTIONS  mFeatureDataOptions;
	D3D12_FEATURE_DATA_D3D12_OPTIONS1 mFeatureDataOptions1;
	SIZE_T                            mDedicatedVideoMemory = 0;
	char                              mVendorId[MAX_GPU_VENDOR_STRING_LENGTH];
	char                              mDeviceId[MAX_GPU_VENDOR_STRING_LENGTH];
	char                              mRevisionId[MAX_GPU_VENDOR_STRING_LENGTH];
	char                              mName[MAX_GPU_VENDOR_STRING_LENGTH];
	GPUPresetLevel                    mPreset;
}                                     GpuDesc;

extern HMODULE hook_get_d3d12_module_handle();

extern void hook_post_init_renderer(Renderer* pRenderer);
extern void hook_post_remove_renderer(Renderer* pRenderer);
extern void hook_enable_debug_layer(Renderer* pRenderer);

extern HRESULT hook_create_device(void* pAdapter, D3D_FEATURE_LEVEL featureLevel, ID3D12Device** ppDevice);
extern HRESULT hook_create_command_queue(ID3D12Device* pDevice, const D3D12_COMMAND_QUEUE_DESC* qDesc, ID3D12CommandQueue** ppQueue);
extern HRESULT hook_create_copy_cmd(ID3D12Device* pDevice, uint32_t nodeMask, ID3D12CommandAllocator* pAlloc, Cmd* pCmd);
extern void    hook_remove_copy_cmd(Cmd* pCmd);
extern HRESULT hook_create_graphics_pipeline_state(ID3D12Device* pDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc, void* pExtensions, uint32_t extensionCout, ID3D12PipelineState** ppPipeline);
extern HRESULT hook_create_compute_pipeline_state(ID3D12Device* pDevice, const D3D12_COMPUTE_PIPELINE_STATE_DESC* pDesc, void* pExtensions, uint32_t extensionCout, ID3D12PipelineState** ppPipeline);
extern HRESULT hook_create_special_resource(
	Renderer* pRenderer,
	const D3D12_RESOURCE_DESC* pDesc,
	const D3D12_CLEAR_VALUE* pClearValue,
	D3D12_RESOURCE_STATES startState,
	uint32_t flags,
	ID3D12Resource** ppOutResource);
//HRESULT create_swap_chain(struct Renderer* pRenderer, struct SwapChain* pSwapChain, DXGI_SWAP_CHAIN_DESC1* desc, IDXGISwapChain1** swapchain);

extern TextureFormat hook_get_recommended_swapchain_format(bool hintHDR);
extern uint32_t hook_get_swapchain_image_index(SwapChain* pSwapChain);
extern HRESULT hook_acquire_next_image(ID3D12Device* pDevice, SwapChain* pSwapChain);
extern HRESULT hook_queue_present(Queue* pQueue, SwapChain* pSwapChain, uint32_t swapChainImageIndex);
extern void hook_dispatch(Cmd* pCmd, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
extern void hook_signal(Queue* pQueue, ID3D12Fence* pDxFence, uint64_t fenceValue);

extern void hook_fill_gpu_desc(Renderer* pRenderer, D3D_FEATURE_LEVEL featureLevel, GpuDesc* pInOutDesc);
extern void hook_modify_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t* pInOutSize);
extern void hook_modify_heap_flags(DescriptorType type, D3D12_HEAP_FLAGS* pInOutFlags);
extern void hook_modify_buffer_resource_desc(const BufferDesc* pBuffer, D3D12_RESOURCE_DESC* pInOutDesc);
extern void hook_modify_texture_resource_flags(TextureCreationFlags creationFlags, D3D12_RESOURCE_FLAGS* pInOutFlags);
extern void hook_modify_shader_compile_flags(uint32_t stage, bool enablePrimitiveId, const WCHAR** pData, uint32_t* pInOutNumFlags);
extern void hook_modify_rootsignature_flags(uint32_t stages, D3D12_ROOT_SIGNATURE_FLAGS* pInOutFlags);
extern void hook_modify_command_signature_desc(D3D12_COMMAND_SIGNATURE_DESC* pInOutDesc, uint32_t padding);
