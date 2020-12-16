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
#include "EASTL/string.h"
#include <comdef.h>
#include <string>
#include <windows.h>
#include <exception>
#include <utility>
#include <d3d12.h>

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

class DX12Exception : std::exception
{
public:
    DX12Exception() = default;

    DX12Exception(HRESULT        hr,
                  eastl::wstring functionName,
                  eastl::wstring filename,
                  int            lineNumber)
        : errCode{hr}, functionName{std::move(functionName)}, fileName{std::move(filename)}, lineNumber{lineNumber}
    {
    }

    eastl::wstring ToString() const
    {
        return functionName + L" failed in " + fileName + L"; line " +
               eastl::to_wstring(lineNumber);
    }

    HRESULT        errCode = S_OK;
    eastl::wstring functionName;
    eastl::wstring fileName;
    INT32          lineNumber = -1;
};

#ifndef CHECK_DX12_RESULT
#define CHECK_DX12_RESULT(x)                                                                       \
    {                                                                                              \
        HRESULT hr = (x);                                                                          \
        if (FAILED(hr))                                                                            \
        {                                                                                          \
            eastl::wstring wfn = AnsiToWString(__FILE__);                                            \
            throw DX12Exception(hr, L#x, wfn, __LINE__);                                           \
        }                                                                                          \
    }
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
