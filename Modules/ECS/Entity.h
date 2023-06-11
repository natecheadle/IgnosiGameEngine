#pragma once

#include <PoolMemoryBlock.hpp>
#include <UID.hpp>

#include <memory>
#include <tuple>

namespace nate::Modules::ECS
{

    template <typename... ComponentTypes>
    class Entity
    {
        std::tuple<Memory::pool_pointer<ComponentTypes>...> m_Components;

        std::uint64_t m_ID{UID()};

      protected:
        Entity(Memory::pool_pointer<ComponentTypes>... args)
            : m_Components(std::move(args)...)
        {
        }

      public:
        virtual ~Entity() = default;

        Entity(const Entity& other) = delete;
        Entity(Entity&& other)      = delete;

        Entity& operator=(const Entity& other) = delete;
        Entity& operator=(Entity&& other)      = delete;

        std::uint64_t ID() const { return m_ID; }

      protected:
        template <typename T>
        void Set(const T& val)
        {
            auto& pComponent = std::get<Memory::pool_pointer<T>>(m_Components);
            *pComponent      = val;
        }

        template <typename T>
        const T& Get() const
        {
            auto& pComponent = std::get<Memory::pool_pointer<T>>(m_Components);
            return *pComponent;
        }

        template <typename T>
        T& Get()
        {
            auto& pComponent = std::get<Memory::pool_pointer<T>>(m_Components);
            return *pComponent;
        }
    };
} // namespace nate::Modules::ECS
