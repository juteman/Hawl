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
#ifndef HAWL_HEAP_H
#  define HAWL_HEAP_H
#  include "BaseType.h"
namespace HAWL {

template<typename T>
class HawlGC
{
private:
  /// 禁止拷贝
  HawlGC(const HawlGC&) = delete;
  HawlGC& operator=(const HawlGC&) = delete;

  /// 禁止在堆上分配数组对象
  void* operator new[](size_t size);
  void  operator delete[](void* ptr);

public:
  void*        operator new(size_t size);
  static void* allocateObject(size_t size);

  /// 在垃圾回收下delete应不可用，无需手动释放
  void operator delete(void* ptr);

protected:
  HawlGC() {}
};
}
#endif