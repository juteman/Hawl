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
#include "Renderer.h"
#include <vector>

namespace Hawl
{



struct DX12AdapterDesc
{
    /// Max D3D12 feature level supported.
    /// If not supported or not find any adapter,
    /// it will be 0.
    D3D_FEATURE_LEVEL maxFeatureSupported;

    /// Store the Adapter Information
    DXGI_ADAPTER_DESC3 info;

    /// Store the  output info
    std::vector<DXGI_OUTPUT_DESC> outputInfo;
};

class DX12Device
{
    HAWL_DISABLE_COPY(DX12Device)
  public:
    DX12Device() = default;
    virtual ~DX12Device() = default;

    /**
     * \brief Get the best Adapter to created
     */
    void GetHardwareAdapter();

    /**
     * \brief Log the adapter information
     */
    void LogAdapter() const;

    /**
     * \brief print the adapter output information
     */
    void LogAdapterOutPut();
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
    SmartPtr::RefCountPtr<IDXGIAdapter4> m_dxgiAdapter4;
    SmartPtr::RefCountPtr<IDXGIDevice4> m_device4;

  public:
    DX12AdapterDesc adapterDesc;
};
} // namespace Hawl