/**
 *  Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
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
#include <string>
#include "EASTL/internal/config.h"

#ifdef _MSC_VER
#define Hawl_CALLCONV __cdecl
#else
#define Hawl_CALLCONV
#endif

#define HAWL_DISABLE_COPY(TypeName)                                                                \
    TypeName(const TypeName &) = delete;                                                           \
    void operator=(const TypeName &) = delete;

#ifndef ArraySize
#define ArraySize(a) (sizeof(a) / sizeof(a[0]))
#endif

// Add C export define
#if PLATFORM_WIN32
#define HAWL_EXPORT __declspec(dllexport)
#define HAWL_IMPORT __declspec(dllimport)
#else
#define HAWL_EXPORT __attribute__((visibility("default")))
#define HAWL_IMPORT
#endif

#ifndef FORCEINLINE
#if defined(__GNUC__)
#define FORCEINLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif
#endif

#ifdef __cplusplus
#define HAWL_C_API extern "C"
#endif

#ifdef _DEBUG
#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif
#else
#define DEBUG_BREAK()
#endif


#define SafeDelete(a)                                                                              \
    {                                                                                              \
        delete a;                                                                                  \
        a = nullptr;                                                                               \
    }
#define SafeDeleteArray(a)                                                                         \
    {                                                                                              \
        delete[] a;                                                                                \
        a = nullptr;                                                                               \
    }

#ifndef TO_STRING
#define TO_STRING(x)  #x
#endif

#include "BaseType.h"

namespace Hawl
{
/**
 * \brief aligned a number to up to near multiple
 *
 * For example RoundUp(5, 7) => 7 , RoundUp(10, 3) => 12 , RoundUp(127, 32) => 128
 */
static inline uint32 RoundUp(uint32 value, uint32 multiple)
{
    return ((value + multiple - 1) / multiple) * multiple;
}


/**
 * \brief aligned a number to up to near multiple for uint64
 *
 * For example RoundUp(5, 7) => 7 , RoundUp(10, 3) => 12 , RoundUp(127, 32) => 128
 */
static inline uint64 RoundUp64(uint64 value, uint64 multiple)
{
    return ((value + multiple - 1) / multiple) * multiple;
}
} // namespace Hawl
