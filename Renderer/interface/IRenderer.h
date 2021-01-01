/*
 * Copyright 2020 juteman
 *
 * This file is base on The-Forge(https://github.com/ConfettiFX/The-Forge)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
#pragma once
#include "EASTL/unique_ptr.h"

using eastl::unique_ptr;

namespace Hawl
{
class RendererImpl;
class DeviceImpl;

class Renderer
{
public:


private:
    unique_ptr<DeviceImpl> mDeviceImplPtr;
};

}
