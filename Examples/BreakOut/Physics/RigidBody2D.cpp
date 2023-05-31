#include "RigidBody2D.h"

namespace nate::Modules::Physics
{
    void RigidBody2D::Update(float dt) {}

    void RigidBody2D::CollisionOccurred(const RigidBody2D& other)
    {
        if (!IsFixed())
        {
            if (other.IsFixed())
            {
            }
            else
            {
            }
        }

        m_OnCollision(other);
    }
} // namespace nate::Modules::Physics
