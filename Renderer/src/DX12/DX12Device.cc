/*
 * Copyright (c) 2020 juteman
 *
 * This file is part of ReForge
 * (see https://github.com/juteman/Hawl).
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
#include "BaseType.h"
#include "DX12/DX12Helper.h"
#include "IRenderer.h"
#include "Logger.h"

#include <Windows.h>
#include <array>
#include <d3d12.h>
#include <string>
namespace Hawl
{
// Feature Level List of Direct3D
const static D3D_FEATURE_LEVEL D3DFeatureLevels[] = {
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
};

/**
 * \brief Log the adapter information
 * \param desc3  description struct
 */
void LogAdapter(DXGI_ADAPTER_DESC3 desc3)
{
    // Convert wstring to string
    std::wstring      convert = desc3.Description;
    const std::string desc(convert.begin(), convert.end());
    Logger::info("{}", desc);
    Logger::info("{}", desc3.DeviceId);
    Logger::info("{}", desc3.VendorId);
    Logger::info("{}", desc3.DedicatedVideoMemory);
}

void Renderer::Init(bool isDebug)
{
    m_rendererDesc.rendererApi = D3D12;
    const UINT32 flags = isDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
    CHECK_DX12_RESULT(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_factory6)));
}

void Renderer::CreateDevice(bool isDebug)
{
    auto CreateMaxFeatureLevel = [&](const D3D_FEATURE_LEVEL *pFeatureLevels,
                                     UINT32                   featureLevelCount) -> bool {
        for (uint32_t i = 0; i < featureLevelCount; i++)
        {
            if (SUCCEEDED(D3D12CreateDevice(
                    m_adapter4.Get(), pFeatureLevels[i], IID_PPV_ARGS(&m_device4))))
            {
                m_rendererDesc.maxFeatureLevel = pFeatureLevels[i];
                return true;
            }
        }

        return false;
    };

    // Enum the adapter by Gpu power
    for (UINT32 i = 0; m_factory6->EnumAdapterByGpuPreference(
                           i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter4)) !=
                       DXGI_ERROR_NOT_FOUND;
         i++)
    {
        DXGI_ADAPTER_DESC3 desc{};

        CHECK_DX12_RESULT(m_adapter4->GetDesc3(&desc));

        // No soft render
        if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
        {
            // if support level Dx11 , the choose the adapter
            if (CreateMaxFeatureLevel(D3DFeatureLevels, ArraySize(D3DFeatureLevels)))
            {
                LogAdapter(desc);
                break;
            }
        }
    }
}

} // namespace Hawl
