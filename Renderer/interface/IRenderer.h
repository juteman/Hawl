/*
 * Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
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

#pragma once
#if D3D12_SUPPORTED
#include "d3d12.h"
#endif
#include <dxgi1_6.h>

namespace Server::Graphics
{
struct Renderer
{
#if D3D12_SUPPORTED
    IDXGIFactory6 *pDXGIFactory;
    IDXGIAdapter4 *pDxActiveGPU;
    ID3D12Device *pDxDevice;
    ID3D12Debug *pDXDebug;
    ID3D12InfoQueue *pDxDebugValidation;
#endif

    uint32 mEnableGpuBasedValidation : 1;
};

struct RendererDesc
{
    uint32 mEnableGpuBasedValidation : 1;
};


// clang-format off
// API functions
#define API_INTERFACE

API_INTERFACE void HAWL_CALLCONV Init(Renderer **ppRenderer, RendererDesc *pRendererDesc);
// clang-format on
}
