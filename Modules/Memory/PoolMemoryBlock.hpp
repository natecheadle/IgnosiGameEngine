#pragma once

#include <NullMutex.hpp>

#include <cassert>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace nate::Modules::Memory
{
    template <class T>
    class PoolMemoryBlock;

    template <class T>
    class PoolPointer
    {
        friend PoolMemoryBlock<T>;

        size_t              m_PoolIndex;
        PoolMemoryBlock<T>* m_pPool;

      public:
        ~PoolPointer();

        PoolPointer(const PoolPointer<T>& other)               = delete;
        PoolPointer<T>& operator=(const PoolPointer<T>& other) = delete;

        PoolPointer(PoolPointer<T>&& other);
        PoolPointer<T>& operator=(PoolPointer<T>&& other);

        bool IsPoolDestroyed() { return !m_pPool; }

        T*       operator->();
        const T* operator->() const;

        T&       operator*();
        const T& operator*() const;

        T*       get();
        const T* get() const;

      private:
        PoolPointer(size_t poolIndex, PoolMemoryBlock<T>* pPool);

        void OnPoolParentDestroyed() { m_pPool = nullptr; }
        void OnObjectMoved(size_t newLoc) { m_PoolIndex = newLoc; }
    };

    template <class T>
    class PoolMemoryBlock
    {
        friend PoolPointer<T>;

        struct Data
        {
            bool                                IsEmpty{true};
            size_t                              PreviousObject{0};
            size_t                              NextObject{0};
            std::array<std::uint8_t, sizeof(T)> Data;
        };

        std::map<void*, std::function<void()>>        m_OnDestroySubs;
        std::map<size_t, std::function<void(size_t)>> m_OnMoveSubs;

        std::vector<Data> m_Data;
        size_t            m_FirstDataIndex;
        size_t            m_FirstEmptyIndex;
        size_t            m_UsedSize;

      public:
        PoolMemoryBlock(size_t initSize)
            : m_Data(initSize)
            , m_FirstDataIndex(initSize)
            , m_FirstEmptyIndex(0)
            , m_UsedSize(0)
        {
            m_Data.front().PreviousObject = m_Data.size();
            for (size_t i = 1; i < m_Data.size(); ++i)
            {
                m_Data[i - 1].NextObject = i;
                m_Data[i].PreviousObject = i - 1;
            }
            m_Data.back().NextObject = m_Data.size();
        }

        ~PoolMemoryBlock()
        {
            for (auto& sub : m_OnDestroySubs)
            {
                sub.second();
            }
        }

        template <typename... Args>
        PoolPointer<T> CreateObject(Args&&... args)
        {
            // Pool is full
            if (m_FirstEmptyIndex == m_Data.size())
            {
                size_t currentSize = m_Data.size();
                m_Data.resize(currentSize * 2);
                for (size_t i = currentSize; i < m_Data.size(); ++i)
                {
                    m_Data[i].IsEmpty        = true;
                    m_Data[i - 1].NextObject = i;
                    m_Data[i].PreviousObject = i - 1;
                }
                m_Data.back().NextObject = m_Data.size();
            }

            size_t         newObjLocation = m_FirstEmptyIndex;
            PoolPointer<T> newObj(newObjLocation, this);
            m_Data[newObjLocation].IsEmpty = false;
            m_UsedSize++;

            // Call Constructor at location
            // Pool Pointer is holding the object, but is using the
            // array index instead of a pointer.
            new (reinterpret_cast<void*>(m_Data[newObjLocation].Data.data())) T(std::forward<Args>(std::move(args))...);

            // Pool only has one empty space.
            if (m_Data[m_FirstEmptyIndex].NextObject == m_Data.size())
            {
                m_FirstEmptyIndex = m_Data.size();
            }
            else
            {
                m_FirstEmptyIndex = m_Data[newObjLocation].NextObject;
            }

            // Pool is empty
            if (m_FirstDataIndex == m_Data.size())
            {
                m_Data[newObjLocation].PreviousObject = m_Data.size();
                m_Data[newObjLocation].NextObject     = m_Data.size();
                m_FirstDataIndex                      = newObjLocation;
            }
            else if (m_FirstDataIndex > newObjLocation)
            {
                m_Data[newObjLocation].PreviousObject   = m_Data.size();
                m_Data[newObjLocation].NextObject       = newObjLocation;
                m_Data[m_FirstDataIndex].PreviousObject = newObjLocation;
                m_FirstDataIndex                        = newObjLocation;
            }
            else
            {
                size_t previousObj                    = FindPreviousRealObject(newObjLocation);
                m_Data[newObjLocation].PreviousObject = previousObj;
                m_Data[newObjLocation].NextObject     = m_Data[previousObj].NextObject;
                m_Data[previousObj].NextObject        = newObjLocation;
            }

            return newObj;
        }

        size_t UsedSize() const { return m_UsedSize; }
        size_t MaxSize() const { return m_Data.size(); }
        bool   IsFragmented() const { return m_FirstEmptyIndex != m_UsedSize; }
        void   Defragment()
        {
            // Already Defragrmented
            if (!IsFragmented())
                return;

            std::map<size_t, std::function<void(size_t)>> oldSubs(std::move(m_OnMoveSubs));
            m_OnMoveSubs = std::map<size_t, std::function<void(size_t)>>();
            size_t newIndex{0};
            for (size_t i = 0; i < m_Data.size(); ++i)
            {
                if (!m_Data[i].IsEmpty)
                {
                    m_Data[newIndex].IsEmpty        = false;
                    m_Data[newIndex].PreviousObject = newIndex - 1;
                    m_Data[newIndex].NextObject     = newIndex + 1;

                    auto sub = oldSubs.find(i);
                    if (sub != oldSubs.end())
                    {
                        sub->second(newIndex);
                        m_OnMoveSubs.insert({newIndex, std::move(sub->second)});
                    }
                    newIndex++;
                }
            }

            m_Data.front().PreviousObject     = m_Data.size();
            m_Data[m_UsedSize - 1].NextObject = m_Data.size();

            for (size_t i = m_UsedSize; i < m_Data.size(); ++i)
            {
                m_Data[i].IsEmpty        = true;
                m_Data[i].PreviousObject = i - 1;
                m_Data[i].NextObject     = i + 1;
            }
            m_Data[m_UsedSize].PreviousObject = m_Data.size();
            m_Data.back().NextObject          = m_Data.size();

            m_FirstEmptyIndex = m_UsedSize;
            m_FirstDataIndex  = 0;
        }

      private:
        T*       ObjectAt(size_t i) { return reinterpret_cast<T*>(m_Data[i].Data.data()); }
        const T* ObjectAt(size_t i) const { return reinterpret_cast<const T*>(m_Data[i].Data.data()); }

        void DestroyObject(size_t i)
        {
            if (m_Data[i].IsEmpty)
            {
                return;
            }
            m_UsedSize--;

            ObjectAt(i)->~T();

            if (i == m_FirstDataIndex)
            {
                m_FirstDataIndex = m_Data[i].NextObject;
            }

            if (m_Data[i].PreviousObject < m_Data.size())
            {
                m_Data[m_Data[i].PreviousObject].NextObject = m_Data[i].NextObject;
            }
            if (m_Data[i].NextObject < m_Data.size())
            {
                m_Data[m_Data[i].NextObject].PreviousObject = m_Data[i].PreviousObject;
            }

            if (i < m_FirstEmptyIndex)
            {
                m_Data[i].PreviousObject = m_Data.size();
                m_Data[i].NextObject     = m_FirstEmptyIndex;
                m_FirstEmptyIndex        = i;
            }
            else
            {
                size_t prevObj           = FindPreviousEmptyObject(i);
                m_Data[i].PreviousObject = prevObj;
                if (prevObj != m_Data.size())
                {
                    m_Data[i].NextObject       = m_Data[prevObj].NextObject;
                    m_Data[prevObj].NextObject = i;
                }
                else
                {
                    m_Data[i].NextObject = m_Data.size();
                }
            }

            m_Data[i].IsEmpty = true;
        }

        size_t FindPreviousEmptyObject(size_t objI)
        {
            assert(!m_Data[objI].IsEmpty);
            size_t searchI = m_FirstEmptyIndex;
            // This would imply the pool is full.
            if (searchI >= m_Data.size())
                return m_Data.size();

            while (m_Data[searchI].NextObject < objI)
            {
                searchI = m_Data[searchI].NextObject;
            }

            return searchI;
        }

        size_t FindPreviousRealObject(size_t objI)
        {
            assert(!m_Data[objI].IsEmpty);
            size_t searchI = m_FirstDataIndex;
            while (m_Data[searchI].NextObject < objI)
            {
                searchI = m_Data[searchI].NextObject;
            }

            return searchI;
        }

        void SubscribeOnDestroy(void* pSub, std::function<void()> callback)
        {
            m_OnDestroySubs.insert_or_assign(pSub, std::move(callback));
        }

        void UnsubscribeOnDestroy(void* pSub)
        {
            auto it = m_OnDestroySubs.find(pSub);
            if (it != m_OnDestroySubs.end())
            {
                m_OnDestroySubs.erase(it);
            }
        }

        void SubscribeOnMove(size_t sub, std::function<void(size_t)> callback)
        {
            m_OnMoveSubs.insert_or_assign(sub, std::move(callback));
        }

        void UnsubscribeOnMove(size_t sub)
        {
            auto it = m_OnMoveSubs.find(sub);
            if (it != m_OnMoveSubs.end())
            {
                m_OnMoveSubs.erase(it);
            }
        }
    };

    template <class T>
    PoolPointer<T>::PoolPointer(size_t poolIndex, PoolMemoryBlock<T>* pPool)
        : m_PoolIndex(poolIndex)
        , m_pPool(pPool)
    {
        if (m_pPool)
        {
            m_pPool->SubscribeOnDestroy(this, [this]() { OnPoolParentDestroyed(); });
            m_pPool->SubscribeOnMove(m_PoolIndex, [this](size_t newIndex) { OnObjectMoved(newIndex); });
        }
    }

    template <class T>
    PoolPointer<T>::~PoolPointer()
    {
        if (m_pPool)
        {
            m_pPool->DestroyObject(m_PoolIndex);
            m_pPool->UnsubscribeOnDestroy(this);
            m_pPool->UnsubscribeOnMove(m_PoolIndex);
        }
    }

    template <class T>
    PoolPointer<T>::PoolPointer(PoolPointer<T>&& other)
        : m_PoolIndex(other.m_PoolIndex)
        , m_pPool(other.m_pPool)
    {
        other.m_pPool = nullptr;
        if (m_pPool)
        {
            m_pPool->UnsubscribeOnDestroy(&other);
            m_pPool->SubscribeOnDestroy(this, [this]() { OnPoolParentDestroyed(); });

            m_pPool->UnsubscribeOnMove(m_PoolIndex);
            m_pPool->SubscribeOnMove(m_PoolIndex, [this](size_t newIndex) { OnObjectMoved(newIndex); });
        }
    }

    template <class T>
    PoolPointer<T>& PoolPointer<T>::operator=(PoolPointer<T>&& other)
    {
        if (this == &other)
            return *this;

        m_PoolIndex = other.m_PoolIndex;
        m_pPool     = other.m_pPool;

        other.m_pPool = nullptr;
        m_pPool->UnsubscribeOnDestroy(&other);
        m_pPool->SubscribeOnDestroy(this, this->PoolParentDestroyed());

        m_pPool->UnsubscribeOnMove(m_PoolIndex);
        m_pPool->SubscribeOnMove(m_PoolIndex, [this](size_t newIndex) { OnObjectMoved(newIndex); });

        return *this;
    }

    template <class T>
    T* PoolPointer<T>::operator->()
    {
        return get();
    }

    template <class T>
    const T* PoolPointer<T>::operator->() const
    {
        return get();
    }

    template <class T>
    T& PoolPointer<T>::operator*()
    {
        assert(m_pPool);
        assert(!m_pPool->m_Data[m_PoolIndex].IsEmpty);
        return *(m_pPool->ObjectAt(m_PoolIndex));
    }

    template <class T>
    const T& PoolPointer<T>::operator*() const
    {
        assert(m_pPool);
        assert(!m_pPool->m_Data[m_PoolIndex].IsEmpty);
        return *(m_pPool->ObjectAt(m_PoolIndex));
    }
    template <class T>
    T* PoolPointer<T>::get()
    {
        if (!m_pPool)
            return nullptr;

        assert(!m_pPool->m_Data[m_PoolIndex].IsEmpty);
        return m_pPool->ObjectAt(m_PoolIndex);
    }

    template <class T>
    const T* PoolPointer<T>::get() const
    {
        if (!m_pPool)
            return nullptr;

        assert(!m_pPool->m_Data[m_PoolIndex].IsEmpty);
        return m_pPool->ObjectAt(m_PoolIndex);
    }

} // namespace nate::Modules::Memory