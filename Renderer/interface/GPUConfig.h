/**
 *  Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *
 *  Port from The-Forge (see https://github.com/ConfettiFX/The-Forge).
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
#include "IRenderer.h"


//Reads the gpu config and sets the preset level of all available gpu's
void setGPUPresetLevel(Renderer* pRenderer, uint32_t gpuCount, GPUSettings* pGpuSettings)
{
	FileStream fh = {};
	if (!fsOpenStreamFromPath(RD_GPU_CONFIG, "gpu.cfg", FM_READ, &fh))
	{
		LOGF(LogLevel::eWARNING, "gpu.cfg could not be found, setting preset to Low as a default.");
		return;
	}

	char configStr[1024] = {};
	while (!fsStreamAtEnd(&fh))
	{
		fsReadFromStreamLine(&fh, configStr);
		checkForPresetLevel(configStr, pRenderer, gpuCount, pGpuSettings);
		// Do something with the tok
	}

	fsCloseStream(&fh);
}

