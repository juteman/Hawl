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
#include "DX12Handle.h"
#include "DX12Helper.h"
#include "EASTL/array.h"
#include "Logger.h"
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include "Device.h"
namespace Hawl
{

typedef enum D3D12_DESCRIPTOR_HEAP_TYPE
{
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV = 0,
    D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
    D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
    D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
    D3D12_DESCRIPTOR_HEAP_TYPE_NUM_COUNT
} D3D12_DESCRIPTOR_HEAP_TYPE;

/**
 * \brief Log the adapter information
 * \param desc3  description struct
 */
void LogAdapter(DXGI_ADAPTER_DESC1 desc1)
{
    // Convert wstring to string
    std::wstring      convert = desc1.Description;
    const std::string desc(convert.begin(), convert.end());
    Logger::info("{}", desc);
    Logger::info("{}", desc1.DeviceId);
    Logger::info("{}", desc1.VendorId);
    Logger::info("{}", desc1.DedicatedVideoMemory);
}


DeviceHandle CreateDevice(IDXGIFactory6 *pFactory6, D3D_FEATURE_LEVEL requestedFeatureLevel)
{
    const static eastl::array<D3D_FEATURE_LEVEL, 4> D3DFeatureLevels{{D3D_FEATURE_LEVEL_12_1,
                                                                      D3D_FEATURE_LEVEL_12_0,
                                                                      D3D_FEATURE_LEVEL_11_1,
                                                                      D3D_FEATURE_LEVEL_11_0}};

    AdapterHandle     adapterHandle;
    DeviceHandle      deviceHandle;
    D3D_FEATURE_LEVEL selectedFeatureLevel;

    auto CreateMaxFeatureLevel = [&](const eastl::array<D3D_FEATURE_LEVEL, 4>& featureLevelArray,
                                     UINT32                             featureLevelCount) -> bool {
        for (uint32_t i = 0; i < featureLevelCount; i++)
        {
            if (SUCCEEDED(D3D12CreateDevice(adapterHandle.Get(),
                                            featureLevelArray[i],
                                            IID_PPV_ARGS(deviceHandle.GetAddressOf()))))
            {
                selectedFeatureLevel = featureLevelArray[i];
                return true;
            }
        }

        return false;
    };

    bool bSelectedAdapter = false;

    for (UINT32 i = 0; pFactory6->EnumAdapterByGpuPreference(
                           i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapterHandle)) !=
                       DXGI_ERROR_NOT_FOUND;
         i++)
    {
        DXGI_ADAPTER_DESC1 desc{};

        CHECK_DX12_RESULT(adapterHandle->GetDesc1(&desc))

        // No soft render
        if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
        {
            bSelectedAdapter = true;
            LogAdapter(desc);
            break;
        }
    }

    if (!bSelectedAdapter)
    {
        // If no GPU was found, just select the first and select default adapter device
        Logger::warn(
            "Could not find a GPU matching conditions specified in environment variables.");
        CHECK_DX12_RESULT(pFactory6->EnumAdapters1(0, adapterHandle.GetAddressOf()));
    }

    if (requestedFeatureLevel == 0)
    {
        CreateMaxFeatureLevel(D3DFeatureLevels, D3DFeatureLevels.size());
    }
    else
    {
        D3D12CreateDevice(adapterHandle.Get(), requestedFeatureLevel, IID_PPV_ARGS(deviceHandle.GetAddressOf()));
        selectedFeatureLevel = requestedFeatureLevel;
    }

    if(deviceHandle.Get() != nullptr)
    {
        Logger::info("Successfully created device with feature level: {}", D3DFeatureLevelToString(selectedFeatureLevel));
        return deviceHandle;
    }

    Logger::error("Failed created D3D device");
    return nullptr;
}


ISwapChain3Handle CreateSwapChain(IDXGIFactory6 *pFactory6, const Window* pWindow, ID3D12CommandQueue* pCommandQueue)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc1{};


}



} // namespace Hawl
