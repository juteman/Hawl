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
#include "EASTL/string.h"
#include "volk.h"

class VulkanException
{
  public:
    VulkanException() = default;
    VulkanException(VkResult &           result,
                    const eastl::string &functionName,
                    const eastl::string &filename,
                    int                  lineNumber)
        : errCode(result), functionName(functionName), fileName(filename), lineNumber(lineNumber)
    {

    }

    eastl::string ToString() const
    {
        return functionName + " failed in " + fileName + "; line " + eastl::to_string(lineNumber);
    }

    VkResult      errCode = VK_SUCCESS;
    eastl::string functionName;
    eastl::string fileName;
    int           lineNumber = -1;
};

#ifndef CHECK_VULKAN_RESULT
#define CHECK_VULKAN_RESULT(x)                                                                     \
    {                                                                                              \
        VkResult result = (x);                                                                     \
        if (result != VK_SUCCESS)                                                                  \
        {                                                                                          \
            throw VulkanException(result, #x, __FILE__, __LINE__);                                 \
        }                                                                                          \
    }
#endif