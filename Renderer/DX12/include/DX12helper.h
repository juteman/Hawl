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
#include <string>
#include <windows.h>

class DX12Exception
{
public:
  DX12Exception() = default;
  DX12Exception(HRESULT hr, const char* code, const char* filename, UINT32 line)
    : m_err{ hr }
    , m_code{ code }
    , m_file{ filename }
    , m_line{ line } {};

  HRESULT     m_err = S_OK;
  const char* m_code;
  const char* m_file;
  UINT32      m_line;
};

#ifndef CHECK_DX12_RESULT
#  define CHECK_DX12_RESULT(x)                                                 \
    {                                                                          \
      HRESULT hr = (x);                                                        \
      if (FAILED(hr)) {                                                        \
        throw DX12Exception(hr, L#x, __FILE__, __LINE__, nullptr);             \
      }                                                                        \
    }
#endif