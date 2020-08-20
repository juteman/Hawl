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
#include "IRenderer.h"
#include "Logger.h"
#include "volk.h"
#include <BaseType.h>
#include <vector>
namespace Hawl
{
void Renderer::Init(const char *appName, bool isDebug)
{
    // Set the render desc api to vulkan
    m_rendererDesc.rendererApi = VULKAN;

    // TODO here need nvapi or ags

    // Load the vulkan library
    VkResult vkResult = volkInitialize();
    if (vkResult != VK_SUCCESS)
    {
        Logger::error("Failed to initialize the vulkan");
        // TODO Here should not be return but raise a exception
        throw std::runtime_error("Failed to initialize the vulkan");
    }
    CreateInstance(appName);
}

void Renderer::CreateInstance(const char *appName, bool isDebug)
{
    UINT32 layoutCount{0};
    UINT32 extensionCount{0};

    // Get the layout count and layout count
    vkEnumerateInstanceLayerProperties(&layoutCount, nullptr);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkLayerProperties>     layers(layoutCount);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    // Enumerate the properties to the vector
    vkEnumerateInstanceLayerProperties(&layoutCount, layers.data());
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    // log the vkinstance layer and extension layer
    for (const auto &layer : layers)
        Logger::info("{} vkinstance layer", layer.layerName);

    for (const auto &ext : extensions)
        Logger::info("{} extension layer", ext.extensionName);

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Hawl Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    // TODO Get the Created info
}
} // namespace Hawl
