/*
 * Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 * Copyright (c) 2018-2021 The Forge Interactive Inc.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *  and base of the Direct3D12Hooks.cpp
 *	in The-Forge (see https://github.com/ConfettiFX/The-Forge)
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


#include "BaseType.h"
#include "IRenderer.h"
#include "D3D12Hooks.h"
#include <cstdio>

HMODULE hook_get_d3d12_module_handle()
{
	return GetModuleHandle(TEXT("d3d12.dll"));
}

void hook_enable_debug_layer(Renderer* pRenderer)
{
	UNREF_PARAM(pRenderer);
#if defined(GRAPHICS_DEBUG)
	pRenderer->pDXDebug->EnableDebugLayer();
	ID3D12Debug1* pDebug1 = nullptr;
	if (SUCCEEDED(pRenderer->pDXDebug->QueryInterface(IID_PPV_ARGS(&pDebug1))))
	{
		pDebug1->SetEnableGPUBasedValidation(pRenderer->mEnableGpuBasedValidation);
		pDebug1->Release();
	}
#endif
}


extern void hook_fill_gpu_desc(Renderer* pRenderer, D3D_FEATURE_LEVEL featureLevel, GpuDesc* pInOutDesc)
{
	// Query the level of support of Shader Model.
	D3D12_FEATURE_DATA_D3D12_OPTIONS  featureData = {};
	D3D12_FEATURE_DATA_D3D12_OPTIONS1 featureData1 = {};
	// Query the level of support of Wave Intrinsics.
	pRenderer->pDxDevice->CheckFeatureSupport(
		static_cast<D3D12_FEATURE>(D3D12_FEATURE_D3D12_OPTIONS), &featureData, sizeof(featureData));
	pRenderer->pDxDevice->CheckFeatureSupport(
		static_cast<D3D12_FEATURE>(D3D12_FEATURE_D3D12_OPTIONS1), &featureData1, sizeof(featureData1));

	GpuDesc& gpuDesc = *pInOutDesc;
	DXGI_ADAPTER_DESC3 desc = {};
	gpuDesc.pGpu->GetDesc3(&desc);

	gpuDesc.mMaxSupportedFeatureLevel = featureLevel;
	gpuDesc.mDedicatedVideoMemory = desc.DedicatedVideoMemory;
	gpuDesc.mFeatureDataOptions = featureData;
	gpuDesc.mFeatureDataOptions1 = featureData1;
	gpuDesc.pRenderer = pRenderer;

	//save vendor and model Id as string
	//char hexChar[10];
	//convert deviceId and assign it
	sprintf_s(gpuDesc.mDeviceId, "%#x\0", desc.DeviceId);
	//convert modelId and assign it
	sprintf_s(gpuDesc.mVendorId, "%#x\0", desc.VendorId);
	//convert Revision Id
	sprintf_s(gpuDesc.mRevisionId, "%#x\0", desc.Revision);

	//save gpu name (Some situtations this can show description instead of name)
	//char sName[MAX_PATH];
	size_t numConverted = 0;
	wcstombs_s(&numConverted, gpuDesc.mName, desc.Description, MAX_GPU_VENDOR_STRING_LENGTH);
}
