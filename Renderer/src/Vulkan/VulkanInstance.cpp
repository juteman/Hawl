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
#include "VulkanInstance.h"
#include "EASTL/algorithm.h"
#include "EASTL/sort.h"
#include "Logger.h"
#include "VulkanHelper.h"


namespace Hawl
{
// clang-format off
eastl::vector<eastl::string> baseInstanceExtensions
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
    VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
        // To legally use HDR formats
        /************************************************************************/
        // VR Extensions
        /************************************************************************/
    VK_KHR_DISPLAY_EXTENSION_NAME,
    VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME,
};
// clang-format on

VulkanInstance::VulkanInstance() : mVkInstance(VK_NULL_HANDLE)
{
}

void VulkanInstance::Init()
{
    CHECK_VULKAN_RESULT(volkInitialize())
}

inline VkResult CreateVKAppInfo(VkApplicationInfo &appInfo, const eastl::string &appName)
{
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pEngineName = "HAWL";
    appInfo.pApplicationName = appName.data();
    appInfo.apiVersion = volkGetInstanceVersion();
    // if apiVersion is 0, get version failed
    if (appInfo.apiVersion == 0)
    {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return VK_SUCCESS;
}

VkResult VulkanInstance::Create(const eastl::string &        appName,
                                eastl::vector<eastl::string> userDefinedInstanceLayer,
                                eastl::vector<eastl::string> userDefinedInstanceExtension,
                                const VkAllocationCallbacks *pUserDefinedAllocator)
{
    using namespace eastl;
    VkApplicationInfo applicationInfo{};
    // Init the application info
    auto CreateVKAppInfo = [&]() {
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pEngineName = "HAWL";
        applicationInfo.pApplicationName = appName.data();
        applicationInfo.apiVersion = volkGetInstanceVersion();
        // if apiVersion is 0, get version failed
        if (applicationInfo.apiVersion == 0)
        {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        return VK_SUCCESS;
    };

    auto CreateInstanceExtension = [&](vector<const char *> &extensions) {
        uint32 extensionCount = 0;
        CHECK_VULKAN_RESULT(
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr))
        vector<VkExtensionProperties> supportExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportExtensions.data());

        // set the union to extension for user define extension and base extensions
        sort(userDefinedInstanceExtension.begin(), userDefinedInstanceExtension.end());
        sort(baseInstanceExtensions.begin(), baseInstanceExtensions.end());
        set_union(userDefinedInstanceExtension.begin(),
                  userDefinedInstanceExtension.end(),
                  baseInstanceExtensions.begin(),
                  baseInstanceExtensions.end(),
                  back_inserter(extensions));

        // If extension not contains in enum support extension
        bool findExt;
        for (int i = 0; i < extensions.size(); i++)
        {
            findExt = false;
            for (const auto &supportExtension : supportExtensions)
            {
                if (strcmp(extensions[i], supportExtension.extensionName) == 0)
                {
                    findExt = true;
                    break;
                }
            }

            if (findExt == false)
            {
                i = extensions.erase(extensions.begin() + i) - extensions.begin();
            }
        }
        return VK_SUCCESS;
    };

    auto CreateInstanceLayer = [&](vector<const char *> &layers) {
        uint32 layerCount = 0;
        CHECK_VULKAN_RESULT(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
        vector<VkLayerProperties> supportLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, supportLayers.data());
        vector<string> supportLayersName;

        for (const auto &supportLayer : supportLayers)
        {
            supportLayersName.push_back(supportLayer.layerName);
        }

        sort(supportLayersName.begin(), supportLayersName.end());
        sort(userDefinedInstanceLayer.begin(), userDefinedInstanceLayer.end());

        set_intersection(supportLayersName.begin(),
                         supportLayersName.end(),
                         userDefinedInstanceLayer.begin(),
                         userDefinedInstanceLayer.end(),
                         layers);
        return VK_SUCCESS;
    };

    CHECK_VULKAN_RESULT(CreateVKAppInfo())

    // Layer and extension
    vector<const char *> extensions;
    vector<const char *> layers;
    CHECK_VULKAN_RESULT(CreateInstanceExtension(extensions))
    CHECK_VULKAN_RESULT(CreateInstanceLayer(layers))

    // set create info
    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledLayerCount = static_cast<uint32>(layers.size());
    createInfo.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();
    createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();

    VkResult result = vkCreateInstance(&createInfo, pUserDefinedAllocator, &mVkInstance);
    return result;
}

} // namespace Hawl