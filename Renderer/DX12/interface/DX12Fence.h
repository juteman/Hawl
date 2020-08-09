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
#include "Renderer.h"
namespace Hawl
{
/**
 * \brief Fence Class is used to synchronize the GPU and CPU
 */
class DX12Fence
{
    DX12Fence() : m_fenceValue(0)
    {
    }

  public:
    ~DX12Fence();

    /**
     * \brief Get the handle of fence
     * \return return the internal handle
     */
    SmartPtr::RefCountPtr<ID3D12Fence1> &GetHandle()
    {
        return m_fence1;
    }


    /**
     * \brief Create fence 
     */
    void Create();

  private:
    UINT                                m_frameIndex;
    HANDLE                              m_fenceEvent;
    SmartPtr::RefCountPtr<ID3D12Fence1> m_fence1;
    UINT64                              m_fenceValue;
};
} // namespace Hawl
