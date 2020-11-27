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
#include "Vulkan/VulkanInstance.h"
#include "Logger.h"

namespace Hawl
{
// clang-format off
eastl::vector<eastl::string> InstanceExtension
{
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_GGP)
        VK_GGP_STREAM_DESCRIPTOR_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_VI_NN)
        VK_NN_VI_SURFACE_EXTENSION_NAME,
#endif
        // Debug utils not supported on all devices yet
#ifdef USE_DEBUG_UTILS_EXTENSION
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#else
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
    VK_NV_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        // To legally use HDR formats
    VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
        /************************************************************************/
        // VR Extensions
        /************************************************************************/
    VK_KHR_DISPLAY_EXTENSION_NAME,
    VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME,
        /************************************************************************/
        // Multi GPU Extensions
        /************************************************************************/
#if VK_KHR_device_group_creation
    VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
#endif
};
// clang-format on

VulkanInstance::VulkanInstance() : mVkInstance(VK_NULL_HANDLE)
{
}

VkResult VulkanInstance::Init()
{
    VkResult vkResult = volkInitialize();
    if (vkResult != VK_SUCCESS)
    {
        Logger::error("Failed to initialize the vulkan");
    }
    return vkResult;
}

VkResult VulkanInstance::Create(const eastl::string &        appName,
                                eastl::vector<eastl::string> userDefinedInstanceLayer)
{

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pEngineName = "HAWL";
    appInfo.pApplicationName = appName.data();
    appInfo.apiVersion = volkGetInstanceVersion();
    // if apiVersion is 0, get version failed
    if (appInfo.apiVersion == 0)
    {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    using namespace eastl;

    // Layer and extension
    vector<string> instanceExtensionLoad{};
    UINT32         layerCount = 0;
    UINT32         extensionCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    vector<VkLayerProperties>     layers(layerCount);
    vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    for (const auto &layer : layers)
    {
        Logger::info("{} vkinstance layer", layer.layerName);
    }

    for (const auto &ext : extensions)
    {
        Logger::info("{} extension layer", ext.extensionName);
    }

    for (int i = 0; i < userDefinedInstanceLayer.size(); i++)
    {
        for (const auto layer : layers)
        {
            if (userDefinedInstanceLayer[i].compare(layer.layerName) == 0)
            {
                break;
            }

            Logger::info("{} vkinstace layer is missing", userDefinedInstanceLayer[i]);
            i = (int)(userDefinedInstanceLayer.erase(userDefinedInstanceLayer.begin() + i) -
                      userDefinedInstanceLayer.begin());
        }
    }

    VkInstanceCreateInfo createInfo{};

    return VK_SUCCESS;
}

} // namespace Hawl