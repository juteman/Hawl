/*
 * Copyright (c) 2020 juteman
 *
 * This file is part of ReForge
 * (see https://github.com/juteman/Hawl).
 *
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

#pragma once
#include "BaseType.h"
#include <assert.h>
#include <atomic>
#include <type_traits>
#include <utility>

namespace Hawl::SmartPtr
{
/// RefCnt Interface example
class IRefCountObject
{
public:
    virtual ~IRefCountObject()
    {
    }

    virtual UINT32 AddRef() const = 0;
    virtual UINT32 Release() const = 0;
    virtual UINT32 GetRefCount() const = 0;
};

class AtomicRefCountObject : public IRefCountObject
{
public:
    AtomicRefCountObject()
        : m_refCounter(0)
    {
    }

    virtual ~AtomicRefCountObject()
    {
        assert(m_refCounter == 0);
    }

    UINT32 AddRef() const
    {
        return (UINT32)++m_refCounter;
    }

    UINT32 Release() const
    {
        --m_refCounter;
        UINT temp = (UINT32)m_refCounter;
        if (temp == 0)
            delete this;
        return temp;
    }

    UINT32 GetRefCount() const
    {
        return m_refCounter;
    }

private:
    mutable std::atomic<UINT32> m_refCounter;
};

/**
 * \brief A type implement the method addRef() and Release() can use this Is designed to instead of
 * ComPtr for DXGI in Direct12
 * \tparam T Direct  factory to Used
 */
template <typename T>
class RefCountPtr
{
public:
    inline RefCountPtr() noexcept = default;

    explicit RefCountPtr(T *pObject, bool IsAddRef = true) noexcept
        : m_pObject{pObject}
    {
        if (m_pObject && IsAddRef)
            m_pObject->AddRef();
    }

    RefCountPtr(const RefCountPtr &Ptr) noexcept
        : m_pObject{Ptr.m_pObject}
    {
        if (m_pObject)
            m_pObject->AddRef();
    }

    template <typename U>
    explicit RefCountPtr(const RefCountPtr<U> &Ptr)
        : m_pObject{static_cast<T *>(Ptr.Get())}
    {
        if (m_pObject)
            m_pObject->AddRef();
    }

    RefCountPtr(RefCountPtr &&Ptr) noexcept
        : m_pObject{std::move(Ptr.m_pObject)}
    {
        Ptr.m_pObject = nullptr;
    }

    ~RefCountPtr()
    {
        if (m_pObject)
            m_pObject->Release();
    }

    RefCountPtr &operator=(T *pObject) noexcept
    {
        // Call AddRef before Release, in case the new reference is the same as the old reference.
        T *oldObj = m_pObject;
        m_pObject = pObject;
        if (m_pObject)
        {
            m_pObject->AddRef();
        }
        if (oldObj)
        {
            oldObj->Release();
        }
        return *this;
    }

    RefCountPtr &operator=(const RefCountPtr &Ptr) noexcept
    {
        return *this = Ptr.m_pObject;
    }

    template <typename U>
    RefCountPtr &operator=(const RefCountPtr<U> &Ptr)
    {
        return *this = Ptr.Get();
    }

    RefCountPtr &operator=(RefCountPtr &&Ptr) noexcept
    {
        if (this != &Ptr)
        {
            if (m_pObject)
                m_pObject->Release();

            m_pObject = Ptr.m_pObject;
            Ptr.m_pObject = nullptr;
        }
        return *this;
    }

    inline T &operator*() noexcept
    {
        return *m_pObject;
    }

    inline const T &operator*() const noexcept
    {
        return *m_pObject;
    }

    inline T *operator->() noexcept
    {
        return m_pObject;
    }

    inline const T *operator->() const noexcept
    {
        return m_pObject;
    }

    /// operator  T* can Return the reference
    operator T *() noexcept
    {
        return m_pObject;
    }

    operator T *() const noexcept
    {
        return m_pObject;
    }

    bool operator==(const RefCountPtr &Ptr) const noexcept
    {
        return m_pObject == Ptr.m_pObject;
    }

    bool operator!=(const RefCountPtr &Ptr) const noexcept
    {
        return m_pObject != Ptr.m_pObject;
    }

    T **InitAndGetAddressOf()
    {
        *this = nullptr;
        return &m_pObject;
    }

    T *const *GetAddressOf() const
    {
        return &m_pObject;
    }

    T **GetAddressOf()
    {
        return &m_pObject;
    }

    T *Get() const
    {
        return m_pObject;
    }

    inline friend bool IsValidRef(const RefCountPtr &Ptr)
    {
        return Ptr.m_pObject != nullptr;
    }

    bool IsValid() const
    {
        return m_pObject != nullptr;
    }

    inline void SafeRelease()
    {
        *this = nullptr;
    }

    UINT32 GetRefCount()
    {
        UINT32 Result = 0;
        if (m_pObject)
        {
            Result = m_pObject->GetRefCount();
            assert(Result > 0);
        }
        return Result;
    }

    void swap(RefCountPtr &Ptr) noexcept
    {
        std::swap(m_pObject, Ptr.m_pObject);
    }

private:
    T *m_pObject = nullptr;
};
} // namespace Hawl::SmartPtr
