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

#include "Logger.h"
#include <d3d12.h>
#include <EAAssert/eaassert.h>
#include <EASTL/string.h>

/**
 * \brief This function convert string to wstring
 * \param str  standard string to be convert
 * \return return the string convert to wstring
 */
inline eastl::wstring AnsiToWString(const eastl::string &str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return eastl::wstring(buffer);
}

/**
 * @brief Check if result is failure, if it is, will exit with error message
 * @param result | result code to check
 * @param code  | the code generator result to be checked 
 * @param filename | the filename contains the code
 * @param line | the line of the code  
 * @param device | the adapter device to be checked
*/
FORCEINLINE void CheckDX12Result(HRESULT result, const char* code, const char* filename, uint32 line)
{
    __debugbreak();
    Logger::error(" FAILED with {}  in {} line: {} with HRESULT: {}", code, filename, line, result);
    EA_ASSERT(false);
}

#ifndef CHECK_DX12RESULT
#define CHECK_DX12RESULT(x)  {HRESULT hres = x; if (FAILED(hres)) { CheckDX12Result(hres, #x, __FILE__, __LINE__); }}
#endif


// clang-format off
#define TO_STRING_CASE(a) case a: return #a;

inline eastl::string D3DFeatureLevelToString(D3D_FEATURE_LEVEL featureLevel)
{
    switch (featureLevel)
    {
    TO_STRING_CASE(D3D_FEATURE_LEVEL_9_1)
    TO_STRING_CASE(D3D_FEATURE_LEVEL_9_2)
    TO_STRING_CASE(D3D_FEATURE_LEVEL_9_3)
    TO_STRING_CASE(D3D_FEATURE_LEVEL_10_0)
    TO_STRING_CASE(D3D_FEATURE_LEVEL_10_1)
    TO_STRING_CASE(D3D_FEATURE_LEVEL_11_0)
    TO_STRING_CASE(D3D_FEATURE_LEVEL_11_1)
    TO_STRING_CASE(D3D_FEATURE_LEVEL_12_0)
    TO_STRING_CASE(D3D_FEATURE_LEVEL_12_1)
    default:
        return "D3D FEATURE LEVEL UNDEFINED";
    }
}
#undef TO_STRING_CASE
// clang-format on
