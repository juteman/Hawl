/*
 * Copyright 2020 juteman
 *
 * This file is base on The-Forge(https://github.com/ConfettiFX/The-Forge)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
#include "Common.h"
#include "Renderer.h"

namespace Hawl
{

#define API_INTERFACE
// clang-format off
// API functions
// allocates memory and initializes the renderer -> returns pRenderer
//
API_INTERFACE void Hawl_CALLCONV initRenderer(const char* app_name, const RendererDesc* p_settings, Renderer** pRenderer);
API_INTERFACE void Hawl_CALLCONV removeRenderer(Renderer* pRenderer);

API_INTERFACE void Hawl_CALLCONV addFence(Renderer* pRenderer, Fence** p_fence);
API_INTERFACE void Hawl_CALLCONV removeFence(Renderer* pRenderer, Fence* p_fence);

API_INTERFACE void Hawl_CALLCONV addQueue(Renderer* pRenderer, QueueDesc* pQDesc, Queue** pQueue);
API_INTERFACE void Hawl_CALLCONV removeQueue(Renderer* pRenderer, Queue* pQueue);

API_INTERFACE void Hawl_CALLCONV addSwapChain(Renderer* pRenderer, const SwapChainDesc* p_desc, SwapChain** p_swap_chain);
API_INTERFACE void Hawl_CALLCONV removeSwapChain(Renderer* pRenderer, SwapChain* p_swap_chain);

// command pool functions
API_INTERFACE void Hawl_CALLCONV addCmdPool(Renderer* pRenderer, const CmdPoolDesc* p_desc, CmdPool** p_cmd_pool);
API_INTERFACE void Hawl_CALLCONV removeCmdPool(Renderer* pRenderer, CmdPool* p_CmdPool);
API_INTERFACE void Hawl_CALLCONV addCmd(Renderer* pRenderer, const CmdDesc* p_desc, Cmd** p_cmd);
API_INTERFACE void Hawl_CALLCONV removeCmd(Renderer* pRenderer, Cmd* pCmd);
API_INTERFACE void Hawl_CALLCONV addCmd_n(Renderer* pRenderer, const CmdDesc* p_desc, uint32_t cmd_count, Cmd*** p_cmds);
API_INTERFACE void Hawl_CALLCONV removeCmd_n(Renderer* pRenderer, uint32_t cmd_count, Cmd** p_cmds);

//
// All buffer, texture loading handled by resource system -> IResourceLoader.*
//

API_INTERFACE void Hawl_CALLCONV addRenderTarget(Renderer* pRenderer, const RenderTargetDesc* pDesc, RenderTarget** ppRenderTarget);
API_INTERFACE void Hawl_CALLCONV removeRenderTarget(Renderer* pRenderer, RenderTarget* pRenderTarget);
API_INTERFACE void Hawl_CALLCONV addSampler(Renderer* pRenderer, const SamplerDesc* pDesc, Sampler** p_sampler);
API_INTERFACE void Hawl_CALLCONV removeSampler(Renderer* pRenderer, Sampler* p_sampler);

API_INTERFACE void Hawl_CALLCONV addShaderBinary(Renderer* pRenderer, const BinaryShaderDesc* p_desc, Shader** p_shader_program);
API_INTERFACE void Hawl_CALLCONV removeShader(Renderer* pRenderer, Shader* p_shader_program);

API_INTERFACE void Hawl_CALLCONV addRootSignature(Renderer* pRenderer, const RootSignatureDesc* pDesc, RootSignature** pRootSignature);
API_INTERFACE void Hawl_CALLCONV removeRootSignature(Renderer* pRenderer, RootSignature* pRootSignature);

// pipeline functions
API_INTERFACE void Hawl_CALLCONV addPipeline(Renderer* pRenderer, const PipelineDesc* p_pipeline_settings, Pipeline** p_pipeline);
API_INTERFACE void Hawl_CALLCONV removePipeline(Renderer* pRenderer, Pipeline* p_pipeline);
API_INTERFACE void Hawl_CALLCONV addPipelineCache(Renderer* pRenderer, const PipelineCacheDesc* pDesc, PipelineCache** ppPipelineCache);
API_INTERFACE void Hawl_CALLCONV getPipelineCacheData(Renderer* pRenderer, PipelineCache* pPipelineCache, size_t* pSize, void* pData);
API_INTERFACE void Hawl_CALLCONV removePipelineCache(Renderer* pRenderer, PipelineCache* pPipelineCache);

// Descriptor Set functions
API_INTERFACE void Hawl_CALLCONV addDescriptorSet(Renderer* pRenderer, const DescriptorSetDesc* pDesc, DescriptorSet** pDescriptorSet);
API_INTERFACE void Hawl_CALLCONV removeDescriptorSet(Renderer* pRenderer, DescriptorSet* pDescriptorSet);
API_INTERFACE void Hawl_CALLCONV updateDescriptorSet(Renderer* pRenderer, uint32_t index, DescriptorSet* pDescriptorSet, uint32_t count, const DescriptorData* pParams);

// command buffer functions
API_INTERFACE void Hawl_CALLCONV resetCmdPool(Renderer* pRenderer, CmdPool* pCmdPool);
API_INTERFACE void Hawl_CALLCONV beginCmd(Cmd* p_cmd);
API_INTERFACE void Hawl_CALLCONV endCmd(Cmd* p_cmd);
API_INTERFACE void Hawl_CALLCONV cmdBindRenderTargets(Cmd* p_cmd, uint32_t render_target_count, RenderTarget** p_render_targets, RenderTarget* p_depth_stencil, const LoadActionsDesc* loadActions, uint32_t* pColorArraySlices, uint32_t* pColorMipSlices, uint32_t depthArraySlice, uint32_t depthMipSlice);
API_INTERFACE void Hawl_CALLCONV cmdSetViewport(Cmd* p_cmd, float x, float y, float width, float height, float min_depth, float max_depth);
API_INTERFACE void Hawl_CALLCONV cmdSetScissor(Cmd* p_cmd, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
API_INTERFACE void Hawl_CALLCONV cmdSetStencilReferenceValue(Cmd* p_cmd, uint32_t val);
API_INTERFACE void Hawl_CALLCONV cmdBindPipeline(Cmd* p_cmd, Pipeline* p_pipeline);
API_INTERFACE void Hawl_CALLCONV cmdBindDescriptorSet(Cmd* pCmd, uint32_t index, DescriptorSet* pDescriptorSet);
API_INTERFACE void Hawl_CALLCONV cmdBindPushConstants(Cmd* pCmd, RootSignature* pRootSignature, const char* pName, const void* pConstants);
API_INTERFACE void Hawl_CALLCONV cmdBindPushConstantsByIndex(Cmd* pCmd, RootSignature* pRootSignature, uint32_t paramIndex, const void* pConstants);
API_INTERFACE void Hawl_CALLCONV cmdBindIndexBuffer(Cmd* p_cmd, Buffer* p_buffer, uint32_t indexType, uint64_t offset);
API_INTERFACE void Hawl_CALLCONV cmdBindVertexBuffer(Cmd* p_cmd, uint32_t buffer_count, Buffer** pp_buffers, const uint32_t* pStrides, const uint64_t* pOffsets);
API_INTERFACE void Hawl_CALLCONV cmdDraw(Cmd* p_cmd, uint32_t vertex_count, uint32_t first_vertex);
API_INTERFACE void Hawl_CALLCONV cmdDrawInstanced(Cmd* pCmd, uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance);
API_INTERFACE void Hawl_CALLCONV cmdDrawIndexed(Cmd* p_cmd, uint32_t index_count, uint32_t first_index, uint32_t first_vertex);
API_INTERFACE void Hawl_CALLCONV cmdDrawIndexedInstanced(Cmd* pCmd, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
API_INTERFACE void Hawl_CALLCONV cmdDispatch(Cmd* p_cmd, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);

// Transition Commands
API_INTERFACE void Hawl_CALLCONV cmdResourceBarrier(Cmd* p_cmd, uint32_t buffer_barrier_count, BufferBarrier* p_buffer_barriers, uint32_t texture_barrier_count, TextureBarrier* p_texture_barriers, uint32_t rt_barrier_count, RenderTargetBarrier* p_rt_barriers);

// Virtual Textures

API_INTERFACE void Hawl_CALLCONV cmdUpdateVirtualTexture(Cmd* pCmd, Texture* pTexture);

//
// All buffer, texture update handled by resource system -> IResourceLoader.*
//

// queue/fence/swapchain functions
API_INTERFACE void Hawl_CALLCONV acquireNextImage(Renderer* pRenderer, SwapChain* p_swap_chain, Semaphore* p_signal_semaphore, Fence* p_fence, uint32_t* p_image_index);
API_INTERFACE void Hawl_CALLCONV queueSubmit(Queue* p_queue, const QueueSubmitDesc* p_desc);
API_INTERFACE PresentStatus Hawl_CALLCONV queuePresent(Queue* p_queue, const QueuePresentDesc* p_desc);
API_INTERFACE void Hawl_CALLCONV waitQueueIdle(Queue* p_queue);
API_INTERFACE void Hawl_CALLCONV getFenceStatus(Renderer* pRenderer, Fence* p_fence, FenceStatus* p_fence_status);
API_INTERFACE void Hawl_CALLCONV waitForFences(Renderer* pRenderer, uint32_t fenceCount, Fence** ppFences);
API_INTERFACE void Hawl_CALLCONV toggleVSync(Renderer* pRenderer, SwapChain** ppSwapchain);

//Returns the recommended format for the swapchain.
//If true is passed for the hintHDR parameter, it will return an HDR format IF the platform supports it
//If false is passed or the platform does not support HDR a non HDR format is returned.
API_INTERFACE TinyImageFormat Hawl_CALLCONV getRecommendedSwapchainFormat(bool hintHDR);

//indirect Draw functions
API_INTERFACE void Hawl_CALLCONV addIndirectCommandSignature(Renderer* pRenderer, const CommandSignatureDesc* p_desc, CommandSignature** ppCommandSignature);
API_INTERFACE void Hawl_CALLCONV removeIndirectCommandSignature(Renderer* pRenderer, CommandSignature* pCommandSignature);
API_INTERFACE void Hawl_CALLCONV cmdExecuteIndirect(Cmd* pCmd, CommandSignature* pCommandSignature, uint maxCommandCount, Buffer* pIndirectBuffer, uint64_t bufferOffset, Buffer* pCounterBuffer, uint64_t counterBufferOffset);
/************************************************************************/
// GPU Query Interface
/************************************************************************/
API_INTERFACE void Hawl_CALLCONV getTimestampFrequency(Queue* pQueue, double* pFrequency);
API_INTERFACE void Hawl_CALLCONV addQueryPool(Renderer* pRenderer, const QueryPoolDesc* pDesc, QueryPool** ppQueryPool);
API_INTERFACE void Hawl_CALLCONV removeQueryPool(Renderer* pRenderer, QueryPool* pQueryPool);
API_INTERFACE void Hawl_CALLCONV cmdResetQueryPool(Cmd* pCmd, QueryPool* pQueryPool, uint32_t startQuery, uint32_t queryCount);
API_INTERFACE void Hawl_CALLCONV cmdBeginQuery(Cmd* pCmd, QueryPool* pQueryPool, QueryDesc* pQuery);
API_INTERFACE void Hawl_CALLCONV cmdEndQuery(Cmd* pCmd, QueryPool* pQueryPool, QueryDesc* pQuery);
API_INTERFACE void Hawl_CALLCONV cmdResolveQuery(Cmd* pCmd, QueryPool* pQueryPool, Buffer* pReadbackBuffer, uint32_t startQuery, uint32_t queryCount);
/************************************************************************/
// Stats Info Interface
/************************************************************************/
API_INTERFACE void Hawl_CALLCONV calculateMemoryStats(Renderer* pRenderer, char** stats);
API_INTERFACE void Hawl_CALLCONV calculateMemoryUse(Renderer* pRenderer, uint64_t* usedBytes, uint64_t* totalAllocatedBytes);
API_INTERFACE void Hawl_CALLCONV freeMemoryStats(Renderer* pRenderer, char* stats);
/************************************************************************/
// Debug Marker Interface
/************************************************************************/
API_INTERFACE void Hawl_CALLCONV cmdBeginDebugMarker(Cmd* pCmd, float r, float g, float b, const char* pName);
API_INTERFACE void Hawl_CALLCONV cmdEndDebugMarker(Cmd* pCmd);
API_INTERFACE void Hawl_CALLCONV cmdAddDebugMarker(Cmd* pCmd, float r, float g, float b, const char* pName);
#if defined(DIRECT3D12)
API_INTERFACE uint32_t Hawl_CALLCONV cmdWriteMarker(Cmd* pCmd, MarkerType markerType, uint32_t markerValue, Buffer* pBuffer, size_t offset, bool useAutoFlags = false);
#endif
/************************************************************************/
// Resource Debug Naming Interface
/************************************************************************/
API_INTERFACE void Hawl_CALLCONV setBufferName(Renderer* pRenderer, Buffer* pBuffer, const char* pName);
API_INTERFACE void Hawl_CALLCONV setTextureName(Renderer* pRenderer, Texture* pTexture, const char* pName);
API_INTERFACE void Hawl_CALLCONV setRenderTargetName(Renderer* pRenderer, RenderTarget* pRenderTarget, const char* pName);
API_INTERFACE void Hawl_CALLCONV setPipelineName(Renderer* pRenderer, Pipeline* pPipeline, const char* pName);
/************************************************************************/
/************************************************************************/
// clang-format on
} // namespace Hawl
