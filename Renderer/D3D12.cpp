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
#include "BaseType.h"
#include "IRenderer.h"
#include "mimalloc.h"
#include "EAAssert/eaassert.h"
#include "d3d12.h"
#include "DX12Handle.h"

namespace Server::Graphics
{
void AddDevice(Renderer *pRenderer)
{
#if GRAPHICS_DEBUG
    if (SUCCEEDED(
        D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void **>(&(pRenderer->
            pDXDebug)))))
    {
        pRenderer->pDXDebug->EnableDebugLayer();
        ID3D12Debug1 *pDebug1 = nullptr;
        if (SUCCEEDED(pRenderer->pDXDebug->QueryInterface(IID_PPV_ARGS(&pDebug1))))
        {
            pDebug1->SetEnableGPUBasedValidation(pRenderer->mEnableGpuBasedValidation);
            pDebug1->Release();
        }
    }
#endif


}

void Init(Renderer **ppRenderer, RendererDesc *pRendererDesc)
{
    // ppRenderer and pRendererDesc is not nullptr
    EA_ASSERT(ppRenderer);
    EA_ASSERT(pRendererDesc);

    // Allocate renderer memory space
    auto *pRenderer = static_cast<Renderer *>(mi_calloc_aligned(
        1,
        sizeof(Renderer),
        alignof(Renderer)));

    pRenderer->mEnableGpuBasedValidation = pRendererDesc->mEnableGpuBasedValidation;

    AddDevice(pRenderer);

    DevicePtr pDevice;
}
}
