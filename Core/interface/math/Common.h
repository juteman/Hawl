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
#include "BaseType.h"

static inline uint32 RoundUp(const uint32 &value, const uint32 &multiple)
{
    return ((value + multiple - 1) / multiple) * multiple;
}

static inline uint64 RoundUp64(const uint64 &value, const uint64 &multiple)
{
    return ((value + multiple - 1) / multiple) * multiple;
}

static inline uint32_t RoundDown(const uint32 &value, const uint32 &multiple)
{
    return value - value % multiple;
}

static inline uint64_t RoundDown64(const uint64 &value, const uint64 &multiple)
{
    return value - value % multiple;
}
