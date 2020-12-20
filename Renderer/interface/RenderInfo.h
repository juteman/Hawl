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

#if D3D12_SUPPORTED
#include <d3dcommon.h>
#endif

namespace Hawl
{
    /**
     * \brief Which render api to use
     */
    typedef enum RendererApi
    {
        D3D12,
        VULKAN
    } RendererApi;

    /**
     * \brief Refers to a form of shader hardware in a graphical processing unit (GPU) where all of
     * the shader stages in the rendering pipeline
     * https://en.wikipedia.org/wiki/High-Level_Shading_Language
     */
    enum ShaderModel
    {
        SHADER_MODEL_5_1,
        SHADER_MODEL_6_0,
        SHADER_MODEL_6_1,
        SHADER_MODEL_6_2,
        SHADER_MODEL_6_3,
        SHADER_MODEL_6_4,
        SHADER_MODEL_6_5
    };



};