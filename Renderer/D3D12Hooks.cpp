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

void hook_enable_debug_layer(Renderer* pRenderer)
{
	UNREF_PARAM(pRenderer);
#if defined(ENABLE_GRAPHICS_DEBUG)
	pRenderer->pDXDebug->EnableDebugLayer();
	ID3D12Debug1* pDebug1 = NULL;
	if (SUCCEEDED(pRenderer->pDXDebug->QueryInterface(IID_PPV_ARGS(&pDebug1))))
	{
		pDebug1->SetEnableGPUBasedValidation(pRenderer->mEnableGpuBasedValidation);
		pDebug1->Release();
	}
#endif
}
