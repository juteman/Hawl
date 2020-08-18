/**
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
#include <string>
#ifdef _MSC_VER
#define HAWLCALL __cdecl
#else
#define HAWLCALL
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

#ifdef __cplusplus
#define HAWL_C_API extern "C"
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

namespace Hawl
{
/**
 * \brief Get the current execute program name
 * \return the name of program
 */
const std::string &GetExecutableFilename();

/**
 * \brief Get the absolute name of program
 * \return
 */
const std::string &GetExecutableName();

/**
 * \brief Get the parent path of execute program
 * \return the program current path
 */
const std::string &GetExecutablePath();

} // namespace Hawl
