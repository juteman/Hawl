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
#include "RefCntPtr.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Hawl {
struct DX12AdapterDesc
{
  DX12AdapterDesc()
    : adapterIndex{ -1 }
    , maxFeatureSupported{ (D3D_FEATURE_LEVEL)0 }
  {}

  DX12AdapterDesc(INT32               adapterIndexIn,
                  D3D_FEATURE_LEVEL   maxFeatureSupportedIn,
                  DXGI_ADAPTER_DESC3& adapterDescIn)
    : adapterIndex{ adapterIndexIn }
    , maxFeatureSupported{ maxFeatureSupportedIn }
    , adapterDesc{ adapterDescIn }
  {}

  /// Index of the Adapters
  /// If not find any Adapters, will be -1
  /// The card more power will get the small power
  INT32 adapterIndex;

  /// Max D3D12 feature level supported.
  /// If not supported or not find any adapter,
  /// it will be 0.
  D3D_FEATURE_LEVEL maxFeatureSupported;

  /// Store the Adapter Information
  DXGI_ADAPTER_DESC3 adapterDesc;

  /// manufacturer of the Gpu
  GpuCard Card;
};

enum GpuCard
{
  NVIDA = 0,
  AMD,
  INTEl
};

class DX12Device : public Noncopyable
{
public:
  DX12Device() {}

  void GetHardwareAdapter();
  /// Craate the DXGIFactory with Debug option
  inline void CreateDXGIFactory6(bool isDebug = false);

  inline IDXGIFactory6* GetDXGIFactory6() const { return m_dxgiFactory6; }

protected:
  virtual void CreateDevice(bool isDebug = false);

protected:
  RefCountPtr<IDXGIFactory6> m_dxgiFactory6;
  RefCountPtr<IDXGIAdapter4> m_dxgiAdapter;

  /// Descript of adapter
  DX12AdapterDesc m_adapterDesc;
};
}