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
#define VOLK_IMPLEMENTATION
#include "BaseType.h"
#include "Common.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "volk.h"
#include <vector>

namespace Hawl
{

class VulkanInstance
{
    VulkanInstance();

    void Init();

    VkResult Create(const eastl::string &        appName,
                    eastl::vector<eastl::string> UserDefinedInstanceLayer,
                    eastl::vector<eastl::string> UserDefinedInstanceExtension,
                    const VkAllocationCallbacks* pUserDefinedAllocator);

    FORCEINLINE VkInstance GetInstance()
    {
        return mVkInstance;
    }

    ~VulkanInstance() = default;

  private:
    // here VkInstance is a pointer
    VkInstance mVkInstance;
    HAWL_DISABLE_COPY(VulkanInstance)
};

} // namespace Hawl