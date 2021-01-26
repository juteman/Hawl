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
#  include "ObjectHeader.h"
#  include "Triats.h"
#  include <cstddef>
#  include <type_traits>

namespace Hawl
{
/// HawlGC
template <typename T>
class HawlGC
{
private:
    /// 禁止拷贝
    HawlGC(const HawlGC &) = delete;
    HawlGC &operator=(const HawlGC &) = delete;

    /// 禁止在堆上分配数组对象
    void *operator new[](size_t size);
    void operator delete[](void *ptr);

public:
    /// 重载new函数，使其能够被垃圾回收
    void *operator new(size_t size)
    {
        Allocate(size);
    }

    static void *Allocate(size_t size);

    /// 在垃圾回收下delete应不可用，无需手动释放
    void operator delete(void *ptr);

protected:
    HawlGC()
    {
    }
};

/* 还有另一种偏特化的方式

template <typename T> struct Base1 {};
template <typename T> struct Base2 {};
class DerivedSingle : private Base1<int> {};
class DerivedProtected : protected Base1<int> {};
class DerivedMultiple : Base1<int>, Base2<int> {};
class DerivedSame : Base1<int>, Base1<char> {};
class NotDerived {};
template <typename T1, typename T2> class Base2v {};
class D : Base2v<int, int> {};

template <template <typename...> class B, typename D, typename = void>
struct test_base_template;

template <template <typename...> class B, typename D>
struct test_base_template<
    B, D, typename std::enable_if<std::is_class<D>::value>::type> {
  template <typename... T> static constexpr std::true_type test(B<T...> *);
  static constexpr std::false_type test(...);
};

template <template <typename...> class B, typename D>
struct test_base_template<
    B, D, typename std::enable_if<!std::is_class<D>::value>::type> {
  static constexpr std::false_type test(...);
};

template <class...> using void_t = void;
template <template <typename...> class B, typename D>
using test_for_same_or_public_inheritance =
    decltype(test_base_template<B, D>::test(static_cast<D *>(nullptr)));

template <template <typename...> class B, typename D, typename = void>
struct is_template_base_of : public std::true_type {};

template <template <typename...> class B, typename D>
struct is_template_base_of<B, D,
                           void_t<test_for_same_or_public_inheritance<B, D>>>
    : public test_for_same_or_public_inheritance<B, D> {};
*/

// clang-format off
/******************************************************************************
这里是一系列的偏特化判断
                  IsSubclassOfTemplate<T, HawlGC>
                   T 是 HawlGC 模板的子类型吗？
                   true                 false
                   *                      *
                *                            *
  NeedsAdjustAndMark<T, true>     class NeedsAdjustAndMark<T, false>
   是子类型直接得到false           这里有关于多重继承，此种情况比较复杂，有砍掉的想法
            *                                    *
            *                                    *
    IsObjectAlive<T, false>       IsObjectAlive<T, true>
    此时判断是否被标记即可得出       
    对象是否存活
******************************************************************************/
// clang-format on

template <
    typename T,
    bool = IsSubclassOfTemplate<typename std::remove_cv<T>::type, HawlGC>::value>
class NeedsAdjustAndMark;

template <typename T>
class NeedsAdjustAndMark<T, true>
{
    static_assert(sizeof(T), "T应该被定义为一个类型");

public:
    static const bool value = false;
};

template <typename T>
const bool NeedsAdjustAndMark<T, true>::value;

template <typename T>
class NeedsAdjustAndMark<T, false>
{
    static_assert(sizeof(T), "T应该被定义为一个类型");

public:
    static const bool value =
        IsHawlGCMixin<typename std::remove_cv<T>::Type>::value;
};

template <typename T>
const bool NeedsAdjustAndMark<T, false>::value;

/// 根据bool的两种判断决定偏特化的类型
template <typename T, bool = NeedsAdjustAndMark<T>::value>
class ObjectAliveTrait;

template <typename T>
class ObjectAliveTrait<T, false>
{
public:
    static bool isObjectAlive(T *object)
    {
        static_assert(sizeof(T), "T应该被定义为一个类型");
        return GCObjectHeader::fromPayload(object)->isMarked();
    }
};

template <typename T>
class ObjectAliveTrait<T, true>
{
public:
    static bool isObjectAlive(T *object)
    {
        static_assert(sizeof(T), "T应该被定义为一个类型");
        return object->isObjectAlive();
    }
};

class GCHeap
{
public:
    /// 启动GC
    static void Start();
    /// 关闭GC
    static void Stop();

    /// 判断对象是否存活
    template <typename T>
    static inline bool IsObjectAlive(T *Object)
    {
        static_assert(sizeof(T), "T应该被定义为一个类型");

        /// nullptr 不会被标记释放
        /// 所以应该认为一个nullptr永远是生存状态
        if (!Object)
            return true;

        /// 根据一系列偏特化判断类型是否存活
        return ObjectAliveTrait<T>::isObjectAlive(Object);
    }
};
}
#endif
