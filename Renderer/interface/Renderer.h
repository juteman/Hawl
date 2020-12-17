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

#include "Common.h"
#include "RenderInfo.h"
#include "BaseType.h"

#if D3D12_SUPPORTED
#include <vector>
#include "DX12/DX12Handle.h"
#endif


#ifdef RENDERER_SHARED
#define HAWLRENDERERAPI HAWL_C_API HAWL_EXPORT FORCEINLINE
#else
#define HAWLRENDERERAPI
#endif

namespace Hawl {
    enum class TextureFormat : uint32 {
        // Compressed formats
        BC1,          //!< DXT1 R5G6B5A1
        BC2,          //!< DXT3 R5G6B5A4
        BC3,          //!< DXT5 R5G6B5A8
        BC4,          //!< LATC1/ATI1 R8
        BC5,          //!< LATC2/ATI2 RG8
        BC6H,         //!< BC6H RGB16F
        BC7,          //!< BC7 RGB 4-7 bits per color channel, 0-8 bits alpha
        ETC1,         //!< ETC1 RGB8
        ETC2,         //!< ETC2 RGB8
        ETC2A,        //!< ETC2 RGBA8
        ETC2A1,       //!< ETC2 RGB8A1
        PTC12,        //!< PVRTC1 RGB 2BPP
        PTC14,        //!< PVRTC1 RGB 4BPP
        PTC12A,       //!< PVRTC1 RGBA 2BPP
        PTC14A,       //!< PVRTC1 RGBA 4BPP
        PTC22,        //!< PVRTC2 RGBA 2BPP
        PTC24,        //!< PVRTC2 RGBA 4BPP
        ATC,          //!< ATC RGB 4BPP
        ATCE,         //!< ATCE RGBA 8 BPP explicit alpha
        ATCI,         //!< ATCI RGBA 8 BPP interpolated alpha
        ASTC4x4,      //!< ASTC 4x4 8.0 BPP
        ASTC5x5,      //!< ASTC 5x5 5.12 BPP
        ASTC6x6,      //!< ASTC 6x6 3.56 BPP
        ASTC8x5,      //!< ASTC 8x5 3.20 BPP
        ASTC8x6,      //!< ASTC 8x6 2.67 BPP
        ASTC10x5,     //!< ASTC 10x5 2.56 BPP

        Unknown,      // Compressed formats above.

        R1,
        A8,
        R8,
        R8I,
        R8U,
        R8S,
        R16,
        R16I,
        R16U,
        R16F,
        R16S,
        R32I,
        R32U,
        R32F,
        RG8,
        RG8I,
        RG8U,
        RG8S,
        RG16,
        RG16I,
        RG16U,
        RG16F,
        RG16S,
        RG32I,
        RG32U,
        RG32F,
        RGB8,
        RGB8I,
        RGB8U,
        RGB8S,
        RGB9E5F,
        BGRA8,
        RGBA8,
        RGBA8I,
        RGBA8U,
        RGBA8S,
        RGBA16,
        RGBA16I,
        RGBA16U,
        RGBA16F,
        RGBA16S,
        RGBA32I,
        RGBA32U,
        RGBA32F,
        R5G6B5,
        RGBA4,
        RGB5A1,
        RGB10A2,
        RG11B10F,

        UnknownDepth,

        // Depth-stencil blow
        D16,
        D24,
        D24S8,
        D32,
        D16F,
        D24F,
        D32F,
        D0S8,

        Count
    };

} // namespace Hawl
