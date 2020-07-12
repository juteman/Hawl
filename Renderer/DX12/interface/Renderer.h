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
#ifndef HAWL_RENDERER_H
#  define HAWL_RENDERER_H

#  include "Common.h"
namespace Hawl {

/// Renderer 主要渲染 API 封装
class Renderer
{
  /// Renderer 的API接口
public:
  /// 初始化Renderer
  void HAWL_CALLCONV Init();
};
}
#endif