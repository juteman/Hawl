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
#include "Common.h"
#include "RenderInfo.h"
#if D3D12_SUPPORTED
#include "DX12/DX12Handle.h"
#endif
#if VULKAN_SUPPORTED
#if PLATFORM_LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#include "volk.h"
#endif

#ifdef RENDERER_SHARED
#define HAWLRENDERERAPI HAWL_C_API HAWL_EXPORT
#else
#define HAWLRENDERERAPI
#endif

namespace Hawl
{

class Renderer
{
  public:
    void HAWLCALL Init(const char *appName, bool isDebug = false);

  private:
    RendererDesc m_rendererDesc = {};
#if D3D12_SUPPORTED
    Factory6Handle m_factory6;
    Adapter4Handle m_adapter4;
    Device4Handle  m_device4;
#elif VULKAN_SUPPORTED
    VkInstance       m_vkInstance;
    VkPhysicalDevice m_vkPhysicalDevice;
    VkDevice         m_vkDevice;
#endif

  private:
    // Render function which is export public
#if D3D12_SUPPORTED
    void CreateDevice(bool isDebug = false);
#elif VULKAN_SUPPORTED
    void CreateInstance(const char *appName, bool isDebug = false);
#endif
};
} // namespace Hawl

/*************************************************************************************
 * Export Renderer class method so can use other language binding
 *************************************************************************************/
HAWLRENDERERAPI HAWLCALL Hawl::Renderer *RendererCreate();
HAWLRENDERERAPI HAWLCALL void            RendererInit(Hawl::Renderer *renderer);
HAWLRENDERERAPI HAWLCALL void            RendererDelete(Hawl::Renderer *renderer);