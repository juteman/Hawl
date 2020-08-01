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
#include "BaseType.h"
#include "Common.h"
#include "SmartPtr/RefCntPtr.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Hawl
{

inline D3D_FEATURE_LEVEL DxFeatureLevel[4] = {
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
};

enum GpuCard
{
    NVIDIA = 0,
    AMD,
    INTEl
};

struct DX12AdapterDesc
{
    DX12AdapterDesc() : adapterIndex{-1}, maxFeatureSupported{static_cast<D3D_FEATURE_LEVEL>(0)}
    {
    }

    DX12AdapterDesc(INT32 &adapterIndexIn, D3D_FEATURE_LEVEL &maxFeatureSupportedIn, DXGI_ADAPTER_DESC3 &adapterDescIn)
        : adapterIndex{adapterIndexIn}, maxFeatureSupported{maxFeatureSupportedIn}, adapterDesc{adapterDescIn}
    {
    }

    /// Index of the Adapters
    /// If not find any Adapters, will be -1
    /// The card more power will get the small power
    INT32 adapterIndex;

    /// Max D3D12 feature level supported.
    /// If not supported or not find any adapter,
    /// it will be 0.
    D3D_FEATURE_LEVEL maxFeatureSupported;

    /// Store the Adapter Information
    DXGI_ADAPTER_DESC3 adapterDesc{};
};

class DX12Device
{
    HAWL_DISABLE_COPY(DX12Device)
  public:
    DX12Device() = default;
    virtual ~DX12Device() = default;

    void GetHardwareAdapter();

    /**
     * \brief Create IDXGIFactory6 assign to class member
     * \param isDebug is in debug mode
     */
    inline void CreateDXGIFactory6(bool isDebug = false);

    /**
     * \brief Get  method for member dxgiFactory6
     * \return return the pointer to IDXGIFactory6
     */
    inline IDXGIFactory6 *GetDXGIFactory6() const
    {
        return m_dxgiFactory6;
    }

  protected:
    virtual void CreateDevice(bool isDebug = false);

    SmartPtr::RefCountPtr<IDXGIFactory6> m_dxgiFactory6;
    SmartPtr::RefCountPtr<IDXGIAdapter4> m_dxgiAdapter;

    DX12AdapterDesc m_adapterDesc;
};
} // namespace Hawl