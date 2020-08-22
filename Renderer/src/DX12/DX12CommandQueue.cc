/*
 * Copyright (c) 2020 juteman
 *
 * This file is part of ReForge
 * (see https://github.com/juteman/Hawl).
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

#include "IRenderer.h"

namespace Hawl
{
typedef enum D3D12_COMMAND_QUEUE_TYPE
{
    D3D12_COMMAND_QUEUE_TYPE_COPY,
    D3D12_COMMAND_QUEUE_TYPE_COMPUTE,
    D3D12_COMMAND_QUEUE_TYPE_DIRECT,
    D3D12_COMMAND_QUEUE_TYPE_COUNT
} D3D12_COMMAND_QUEUE_TYPE;

void Renderer::AddCommandQueue(bool isDebug)
{
    // lambda function to get the Command Queue type
    auto GetQueueType = [&](D3D12_COMMAND_QUEUE_TYPE type) {
        switch (type)
        {
        case D3D12_COMMAND_QUEUE_TYPE_COPY:
            return D3D12_COMMAND_LIST_TYPE_COPY;
        case D3D12_COMMAND_QUEUE_TYPE_COMPUTE:
            return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        case D3D12_COMMAND_QUEUE_TYPE_DIRECT:
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        default:
            throw std::exception("Unknown command queue type");
        }
    };


}

} // namespace Hawl
