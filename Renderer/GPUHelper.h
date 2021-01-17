/**
 *  Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
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
#include "amd_ags.h"
#include "nvapi.h"
#include "Logger.h"

#if defined(_WIN64)
#define AMDAGS
#define NVAPI
static AGSContext *pAgsContext = nullptr;
static AGSGPUInfo  gAgsGpuInfo = {};
#else
enum AGSReturnCode
{
	AGS_SUCCESS,                    ///< Successful function call
	AGS_FAILURE,                    ///< Failed to complete call for some unspecified reason
	AGS_INVALID_ARGS,               ///< Invalid arguments into the function
	AGS_OUT_OF_MEMORY,              ///< Out of memory when allocating space internally
	AGS_MISSING_D3D_DLL,            ///< Returned when a D3D dll fails to load
	AGS_LEGACY_DRIVER,              ///< Returned if a feature is not present in the installed driver
	AGS_NO_AMD_DRIVER_INSTALLED,    ///< Returned if the AMD GPU driver does not appear to be installed
	AGS_EXTENSION_NOT_SUPPORTED,    ///< Returned if the driver does not support the requested driver extension
	AGS_ADL_FAILURE,                ///< Failure in ADL (the AMD Display Library)
	AGS_DX_FAILURE                  ///< Failure from DirectX runtime
};

typedef enum NvAPI_Status
{
	NVAPI_OK = 0,          //!< Success. Request is completed.
	NVAPI_ERROR = -1,      //!< Generic error
} NvAPI_Status;
#endif


static AGSReturnCode agsInit()
{
#if defined(AMDAGS)
    AGSConfiguration config = {};
    return agsInitialize(AGS_MAKE_VERSION(AMD_AGS_VERSION_MAJOR,
                                          AMD_AGS_VERSION_MINOR,
                                          AMD_AGS_VERSION_PATCH),
        &config,
        &pAgsContext,
        &gAgsGpuInfo
        );
#endif
    return AGS_SUCCESS;
}

static NvAPI_Status nvapiInit()
{
#if defined(NVAPI)
	return NvAPI_Initialize();
#endif

	return NvAPI_Status::NVAPI_OK;
}

static void agsPrintDriverInfo()
{
#if defined(AMDAGS)
	if (pAgsContext)
	{
		Logger::info("AMD Display Driver Version {}", gAgsGpuInfo.driverVersion);
		Logger::info("AMD Radeon Software Version {}", gAgsGpuInfo.radeonSoftwareVersion);
	}
#endif
}

static void nvapiPrintDriverInfo()
{
#if defined(NVAPI)
	NvU32 driverVersion = 0;
	NvAPI_ShortString buildBranch = {};
	NvAPI_Status status = NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, buildBranch);
	if (NvAPI_Status::NVAPI_OK == status)
	{
		Logger::info("NVIDIA Display Driver Version {}", driverVersion);
		Logger::info("NVIDIA Build Branch {}", buildBranch);
	}
#endif
}

static void agsExit()
{
#if defined(AMDAGS)
	agsDeInitialize(pAgsContext);
#endif
}

static void nvapiExit()
{
#if defined(NVAPI)
	NvAPI_Unload();
#endif
}