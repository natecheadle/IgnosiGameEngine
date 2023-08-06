#pragma once

#include "HitShape.h"

namespace Ignosi::Modules::Physics
{
    class HitRectangle : public HitShape
    {
      public:
      protected:
        std::unique_ptr<DebugSpriteEntity> CreateDebugSprite() const override;
    };
} // namespace Ignosi::Modules::Physics
