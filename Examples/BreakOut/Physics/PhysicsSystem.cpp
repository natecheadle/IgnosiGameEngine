#include "PhysicsSystem.h"

#include "Vector.hpp"

using namespace std::chrono_literals;

namespace nate::Modules::Physics
{
    PhysicsSystem::PhysicsSystem(Memory::PoolMemoryBlock<RigidBody2D>* pRigidBodyPool)
        : ECS::System<RigidBody2D>(pRigidBodyPool)
    {
    }

    void PhysicsSystem::Update(std::chrono::nanoseconds dt)
    {
        float del_t = static_cast<float>(dt / 1.0s);
        auto& pool  = GetPool<RigidBody2D>();
        m_MovingObjects.clear();
        for (auto& body : pool)
        {
            body.Update(del_t);
            if (body.Velocity() != Vector2<float>{0.0f, 0.0f})
            {
                m_MovingObjects.push_back(&body);
            }
        }

        for (auto& pBody : m_MovingObjects)
        {
            for (auto& body : pool)
            {
                if (CheckCollision(*pBody, body))
                {
                    pBody->CollisionOccurred(body);
                    body.CollisionOccurred(*pBody);
                }
            }
        }
    }

    bool PhysicsSystem::CheckCollision(const RigidBody2D& one, const RigidBody2D& two)
    {
        // collision x-axis?
        bool collisionX = one.Position()[0] + one.HitBox()[0] >= two.Position()[0] &&
                          two.Position()[0] + two.HitBox()[0] >= one.Position()[0];
        // collision y-axis?
        bool collisionY = one.Position()[1] + one.HitBox()[1] >= two.Position()[1] &&
                          two.Position()[1] + two.HitBox()[1] >= one.Position()[1];
        // collision only if on both axes
        return collisionX && collisionY;
    }
} // namespace nate::Modules::Physics
