/*
 * Copyright (c) 2020 juteman
 *
 * This file is part of ReForge
 * (see https://github.com/juteman/Hawl).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
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
#include "Renderer.h"
#include "Logger.h"
#include "volk.h"
#include <BaseType.h>
#include "EASTL/vector.h"
#include "VulkanHelper.h"

namespace Hawl {

    void Renderer::Init(const char *appName, bool isDebug) {
        // Set the render desc api to vulkan
        m_rendererDesc.rendererApi = VULKAN;

        // TODO here need nvapi or ags

        // Load the vulkan library
        VkResult vkResult = volkInitialize();
        if (vkResult != VK_SUCCESS) {
            Logger::error("Failed to initialize the vulkan");
            // TODO Here should not be return but raise a exception
            throw std::runtime_error("Failed to initialize the vulkan");
        }
        CreateInstance(appName);
    }

    struct VulkanDeviceInfo
    {
        VkPhysicalDeviceProperties properties;
#ifdef DEFAULT_ENABLE_DEBUG_LAYER
        VkDebugReportCallbackEXT debugReportCallbackHandle;
#endif
    };

//    void Renderer::CreateInstance(const char *appName, bool isDebug) {
//        uint32 layoutCount{0};
//        uint32 extensionCount{0};
//        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
//        const bool enableValidationLayers = isDebug ? true : false;
//
//        // Get the layout count and layout count
//        vkEnumerateInstanceLayerProperties(&layoutCount, nullptr);
//        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
//        std::vector<VkLayerProperties> layers(layoutCount);
//        std::vector<VkExtensionProperties> extensions(extensionCount);
//        // Enumerate the properties to the vector
//        vkEnumerateInstanceLayerProperties(&layoutCount, layers.data());
//        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
//
//        // log the  instance layer and extension layer
//        for (const auto &layer : layers) {
//            Logger::info("{} vkinstance layer", layer.layerName);
//        }
//
//        for (const auto &ext : extensions)
//            Logger::info("{} extension layer", ext.extensionName);
//
//        VkApplicationInfo appInfo{};
//        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//        appInfo.pApplicationName = appName;
//        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//        appInfo.pEngineName = "Hawl Engine";
//        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//        appInfo.apiVersion = VK_API_VERSION_1_1;
//
//        // TODO Get the Extensions in create info
//        VkInstanceCreateInfo createInfo{};
//        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//        createInfo.pApplicationInfo = &appInfo;
//        if (enableValidationLayers) {
//            createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
//            createInfo.ppEnabledLayerNames = validationLayers.data();
//        } else {
//            createInfo.enabledLayerCount = 0;
//        }
//
//        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
//    }


    /**
     * This function select best vulkan device by memory size. We choose the max memory device
     * @param devices vector of device
     * @return  biggest memory device
     */
    VkPhysicalDevice selectBestDevice(const eastl::vector<VkPhysicalDevice>& devices)
    {
        VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
        uint64 BiggestMemory = 0;

        for (const VkPhysicalDevice& device : devices)
        {
            VkPhysicalDeviceMemoryProperties properties;
            vkGetPhysicalDeviceMemoryProperties(device, &properties);

            // Get local memory size from device
            uint64 deviceMemory = 0;
            for (uint32 i = 0; i < properties.memoryHeapCount; i++)
            {
                if ((properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) > 0)
                {
                    deviceMemory = properties.memoryHeaps[i].size;
                    break;
                }
            }

            // Save if best found so far
            if (selectedDevice == VK_NULL_HANDLE || deviceMemory > BiggestMemory)
            {
                selectedDevice = device;
                BiggestMemory = deviceMemory;
            }
        }

        return selectedDevice;
    }

    static eastl::vector<VkLayerProperties> EnumerateInstanceLayerProperties() {
        uint32 layerCount = 0;
        CHECK_VULKAN_RESULT(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
        eastl::vector<VkLayerProperties> supportLayerProperties(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, supportLayerProperties.data());

        // print Layer information to log
        for (const auto &layerProperty : supportLayerProperties) {
            Logger::info("VulkanLayer support: {} - Vulkan Spec Version: {} - Implementation Version: {}",
                         layerProperty.layerName, eastl::to_string(layerProperty.specVersion),
                         eastl::to_string(layerProperty.implementationVersion));
        }

        return supportLayerProperties;
    }

    static bool IsLayerSupported(const eastl::string &layer, eastl::vector<VkLayerProperties> supportedLayers) {
        for (const auto &l : supportedLayers) {
            if (l.layerName == layer) return true;
        }
        return false;
    }

    static bool IsExtensionSupported(const eastl::string &extension,
                                     const eastl::vector<VkExtensionProperties> &supportExtensions) {
        for (const auto &supportExtension : supportExtensions) {
            if (extension.compare(supportExtension.extensionName))
                return true;
        }
        return false;
    }


    void enableLayerIfSupport(const char *layerName, const eastl::vector<VkLayerProperties> &supportedLayers,
                              eastl::vector<const char *> &requiredLayers) {
        if (IsLayerSupported(layerName, supportedLayers)) {
            requiredLayers.push_back(layerName);
        } else {
            Logger::warn("Enable requested Vulkan layer {} failed.", layerName);
        }
    }


    void enableExtensionIfSupport(const char *extensionName, const eastl::vector<VkExtensionProperties > &supportedExtensions,
                                  eastl::vector<const char *> &enabledExtensions) {
        if (IsExtensionSupported(extensionName, supportedExtensions)) {
            enabledExtensions.push_back(extensionName);
        } else{
            Logger::warn("Enable requested Vulkan Extensions {} failed.", extensionName);
        }
    }


    static eastl::vector<VkExtensionProperties> EnumInstanceExtensions() {
        uint32 extensionCount = 0;
        CHECK_VULKAN_RESULT(
                vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr))
        eastl::vector<VkExtensionProperties> supportExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportExtensions.data());

        for (const auto &extension : supportExtensions) {
            Logger::info("Support Extension: {} - Vulkan Spec Version: {}", extension.extensionName,
                         eastl::to_string(extension.specVersion));
        }

        return supportExtensions;
    }



    VkInstance CreateInstance() {

        const auto supportLayerProperties = EnumerateInstanceLayerProperties();
        const auto supportExtensions = EnumInstanceExtensions();

        eastl::vector<const char*> enableLayers;

        std::vector<const char *> enableExtensions = {
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
        };


#if HAWL_DEBUG
        enableLayerIfPresent("VK_LAYER_LUNARG_standard_validation", supportLayerProperties, enableLayers);
#endif

        // init create info
        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.enabledLayerCount = uint32(enableLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = enableLayers.data();
        instanceCreateInfo.enabledExtensionCount = uint32(enableExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = enableExtensions.data();

        VkInstance instance;
        CHECK_VULKAN_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &instance))

        return instance;
    }

    VkPhysicalDevice InitPhysicalDevice(VkInstance instance, VulkanDeviceInfo vulkanDeviceInfo)
    {
        uint32 count = 0;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);

        eastl::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(instance, &count, devices.data());

        VkPhysicalDevice suitDevice = selectBestDevice(devices);


        return suitDevice;
    }

} // namespace Hawl
