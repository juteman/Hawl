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
#ifndef HAWL_TRIATS_H
#  define HAWL_TRIATS_H
#  include <cstdint>
#  include <type_traits>
namespace Hawl {

/// 用作triats判断是否为Template的子类
/// 在D : public B 的继承条件下不会编译错误
/// 否则指针的调用失败
template<typename D, template<typename... T> class B>
class IsSubclassOfTemplate
{
  /// 定义两种不同的字长，用以sizeof比较大小
  typedef int8_t  TrueType;
  typedef int32_t FalseType;

  /// 这里定义了重载函数，交给编译器重载，若果传入的指针
  /// 所指向的类型为B<W...>或继承类型则选择的重载函数的
  /// 返回类型为TrueType,否则则为FalseType
  template<typename... W>
  static TrueType  SubclassCheck(B<W...>*);
  static FalseType SubclassCheck(...);

  static D* derived;

public:
  /// sizeof 不需要函数的具体实现，根据函数定义的返回值就能确定大小
  static const bool value = sizeof(SubclassCheck(derived)) == sizeof(TrueType);
};

/// 判断该类型是否包含IsGCMixinMarker
template<typename T>
struct IsHawlGCMixin
{
private:
  typedef int8_t  TrueType;
  typedef int32_t FalseType;

  /// 编译器在编译期通过重载判断出T是否有IsGCMixinMarker
  /// 不同的重载返回值不同
  template<typename U>
  static TrueType GCMixinMarkerCheck(typename U::IsGCMixinMarker*);

  template<typename U>
  static FalseType GCMixinMarkerCheck(...);

public:
  /// sizeof 不需要函数的具体实现，根据函数定义的返回值就能确定大小
  static const bool value =
    sizeof(GCMixinMarkerCheck<T>(nullptr)) == sizeof(TrueType);
};

/// Destruct a object in memory
/// @param pObject pointer to the object destruct
template<typename T>
inline void
DestructObject(T* pObject)
{
  if constexpr (!std::is_trivially_destructible<T>::value) {
    // typedef beacuse VC can't destructor object it self
    typedef T TypeSelf;

    pObject->TypeSelf::~TypeSelf();
  }
}
}

#endif