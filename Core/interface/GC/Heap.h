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
namespace HAWL {
class Heap
{
public:
  static void Init();
  static void Shutdown();

  template<typename T>
  static inline bool IsObjectAlive(T* object)
  {
    static_assert(sizeof(T), "T 应该是一个已经被定义的类型");
    /// 如果一个指针是一个空指针，应该被标记为或者，这样它才能被
    /// 一个强收集系统收集
    if (!object) {
      return true;
    }

    /// TODO
  }
};
}
#endif